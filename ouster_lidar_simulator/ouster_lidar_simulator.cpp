#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601  // Target Windows 7 or later
#endif

#include <ouster/lidar_scan.h>
#include <Eigen/Core>
#include <ouster/types.h>

#include <boost/asio.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>
#include <optional>

#include "OusterHTTPServer.hpp"
#include "PcapPlayer.h"

#ifdef _WIN32
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define htobe64(x) _byteswap_uint64(x)
#define htobe32(x) _byteswap_ulong(x)
#define htobe16(x) _byteswap_ushort(x)
#define be64toh(x) _byteswap_uint64(x)
#define be32toh(x) _byteswap_ulong(x) 
#define be16toh(x) _byteswap_ushort(x)
#define htons(x) _byteswap_ushort(x)
#define htonl(x) _byteswap_ulong(x)
#else
#include <arpa/inet.h>
#include <endian.h>
#endif

#include "ouster_configuration.hpp"
#include "FrameValidator.hpp"

template <typename T>
using img_t = Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

class OusterLiDARSimulator
{
private:
	ouster::sensor::sensor_info sensor_info_;
	boost::asio::io_context& io_context_;
	boost::asio::ip::udp::socket lidar_socket_;
	boost::asio::ip::tcp::socket imu_socket_;
	boost::asio::ip::udp::endpoint client_endpoint_;
	std::unique_ptr<PcapPlayer> pcap_player_;
	bool use_pcap_ = false;
	bool configured_ = false;
	std::atomic<bool> running_{ false };
	uint32_t frame_id_{ 0 };
	std::chrono::microseconds packet_delay_;

	// Use SDK packet format
	std::unique_ptr<ouster::sensor::packet_format> pf_;

	// Default sensor values
	const uint16_t default_signal_ = 200;
	const uint8_t default_reflectivity_ = 128;
	const uint16_t default_near_ir_ = 100;

	size_t packet_header_size_;
	size_t column_header_size_;
	size_t channel_data_size_;
	size_t col_footer_size_;
	size_t packet_footer_size_;
	size_t pixels_per_column_;
	size_t columns_per_packet_;
	size_t columns_per_frame_;
	size_t num_packets_per_frame_;
	size_t fps_;
	size_t packet_size_;
	size_t column_size_;

	ouster::sensor::AzimuthWindow azimuth_window_{ 0, 360000 };
	double signal_multiplier_{ 1.0 };
	ouster::sensor::ReturnOrder return_order_{ ouster::sensor::ReturnOrder::ORDER_STRONGEST_TO_WEAKEST };
	int range_threshold_cm_{ 0 };
	double imu_gyro_scale_{ 0.0 };
	double imu_accel_scale_{ 0.0 };
	bool apply_window_{ false };

	std::vector<uint8_t> packet_buf_;
	FrameValidator frame_validator_;

public:
	OusterLiDARSimulator(const OusterConfiguration& configuration,
		boost::asio::io_context& ioc) : 
		io_context_(ioc),
		lidar_socket_(io_context_),
		imu_socket_(io_context_),
		packet_header_size_(0),
		column_header_size_(0),
		channel_data_size_(0),
		col_footer_size_(0),
		packet_footer_size_(0)
	{
		read_sensor_info(configuration);
		init_pcap_player(configuration); // Initialize PCAP player if configured. will set use_pcap_ flag

		auto& client_ip = sensor_info_.config.udp_dest.value();
		auto& lidar_port = sensor_info_.config.udp_port_lidar.value();

		client_endpoint_ = boost::asio::ip::udp::endpoint(
			boost::asio::ip::address::from_string(client_ip), lidar_port);

		pf_ = std::make_unique<ouster::sensor::packet_format>(
			sensor_info_.format.udp_profile_lidar, 
			sensor_info_.format.pixels_per_column, 
			sensor_info_.format.columns_per_packet);

		// Debug output packet format details
		std::cout << "Initialized packet format:" << std::endl;
		std::cout << "  UDP Profile: " << ouster::sensor::to_string(sensor_info_.format.udp_profile_lidar) << std::endl;
		std::cout << "  Pixels per column: " << sensor_info_.format.pixels_per_column << std::endl;
		std::cout << "  Columns per packet: " << sensor_info_.format.columns_per_packet << std::endl;

		columns_per_frame_ = sensor_info_.format.columns_per_frame;
		columns_per_packet_ = sensor_info_.format.columns_per_packet;
		pixels_per_column_ = sensor_info_.format.pixels_per_column;

		fps_ = sensor_info_.format.fps;

		num_packets_per_frame_ = columns_per_frame_ / columns_per_packet_;
		lidar_socket_.open(boost::asio::ip::udp::v4());
		packet_size_ = pf_->lidar_packet_size;
		column_size_ = pf_->col_size;
		packet_buf_.resize(packet_size_);

		std::cout << "UDP Socket Configuration:" << std::endl
			<< "  Destination: " << client_endpoint_.address().to_string() << std::endl
			<< "  Port: " << client_endpoint_.port() << std::endl
			<< "  Socket open: " << lidar_socket_.is_open() << std::endl;

		// Optional: Set socket buffer size
		boost::asio::socket_base::send_buffer_size option(1024 * 1024);  // 1MB buffer
		lidar_socket_.set_option(option);

		packet_delay_ = std::chrono::microseconds(1000000 / (fps_ * num_packets_per_frame_));

		std::cout << "Simulator timing configuration:" << std::endl
			<< "  FPS: " << fps_ << std::endl
			<< "  Packets per frame: " << num_packets_per_frame_ << std::endl
			<< "  Packet delay: " << packet_delay_.count() << " microseconds" << std::endl;
	}

	void init_pcap_player(const OusterConfiguration& configuration)
	{
		if (configuration.exists("pcap.file"))
		{
			std::string pcap_file = configuration.get<std::string>("pcap.file");
			bool loop = configuration.get<bool>("pcap.loop");

			pcap_player_ = std::make_unique<PcapPlayer>(pcap_file, loop);
			use_pcap_ = pcap_player_->is_initialized();

			if (use_pcap_)
			{
				std::cout << "PCAP playback configuration:" << std::endl
					<< "  File: " << pcap_file << std::endl
					<< "  FPS: " << fps_ << std::endl
					<< "  Packets per frame: " << num_packets_per_frame_ << std::endl
					<< "  Target packet interval: "
					<< (1000000.0 / (fps_ * num_packets_per_frame_)) << " microseconds" << std::endl;
			}
		}
	}

	ouster::sensor::sensor_info& get_sensor_info() { return sensor_info_; }

	void onConfigChange()
	{
		bool recalc_needed = false;
		auto& config = sensor_info_.config;

		if (config.udp_dest)
		{
			client_endpoint_.address(boost::asio::ip::address::from_string(*config.udp_dest));
		}

		if (config.udp_port_lidar)
		{
			updateLidarPort(*config.udp_port_lidar);
		}

		if (config.udp_port_imu)
		{
			updateImuPort(*config.udp_port_imu);
		}

		if (config.lidar_mode)
		{
			columns_per_frame_ = ouster::sensor::n_cols_of_lidar_mode(*config.lidar_mode);
			fps_ = ouster::sensor::frequency_of_lidar_mode(*config.lidar_mode);
			recalc_needed = true;
		}

		if (config.udp_profile_lidar)
		{
			bool is_legacy = (*config.udp_profile_lidar == ouster::sensor::UDPProfileLidar::PROFILE_LIDAR_LEGACY);
			packet_header_size_ = is_legacy ? 0 : 32;
			col_footer_size_ = is_legacy ? 4 : 0;
			packet_footer_size_ = is_legacy ? 0 : 32;

			// Update packet format with new profile
			pf_ = std::make_unique<ouster::sensor::packet_format>(*config.udp_profile_lidar, pixels_per_column_,
				columns_per_packet_);

			auto x = *config.udp_profile_lidar;
			recalc_needed = true;
		}

		if (config.columns_per_packet)
		{
			columns_per_packet_ = *config.columns_per_packet;
			recalc_needed = true;
		}

		if (config.azimuth_window)
		{
			updateAzimuthWindow(*config.azimuth_window);
		}

		if (config.signal_multiplier)
		{
			updateSignalMultiplier(*config.signal_multiplier);
		}

		if (config.return_order)
		{
			updateReturnOrder(*config.return_order);
		}

		if (config.gyro_fsr || config.accel_fsr)
		{
			updateIMUSettings();
		}

		if (config.min_range_threshold_cm)
		{
			updateRangeThreshold(*config.min_range_threshold_cm);
		}

		if (recalc_needed)
		{
			recalculatePacketSizes();
		}
	}

	void updateLidarPort(uint16_t new_port)
	{
		try
		{
			lidar_socket_.close();
			lidar_socket_.open(boost::asio::ip::udp::v4());
			client_endpoint_.port(new_port);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error updating lidar port: " << e.what() << std::endl;
		}
	}

	void updateImuPort(uint16_t new_port)
	{
		try
		{
			imu_socket_.close();
			imu_socket_.open(boost::asio::ip::tcp::v4());
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error updating IMU port: " << e.what() << std::endl;
		}
	}

	void updateAzimuthWindow(const ouster::sensor::AzimuthWindow& window)
	{
		azimuth_window_ = window;
		apply_window_ = (window.first != 0 || window.second != 360000);

		// Recalculate effective columns per frame based on window
		if (apply_window_)
		{
			int window_size = window.second - window.first;
			if (window_size < 0) window_size += 360000;
			columns_per_frame_ = (sensor_info_.format.columns_per_frame * window_size) / 360000;
			recalculatePacketSizes();
		}
		else
		{
			columns_per_frame_ = sensor_info_.format.columns_per_frame;
			recalculatePacketSizes();
		}
	}

	void updateSignalMultiplier(double multiplier)
	{
		if (multiplier >= 0.25 && multiplier <= 4.0)
		{
			signal_multiplier_ = multiplier;
		}
		else
		{
			std::cerr << "Signal multiplier must be between 0.25 and 4.0" << std::endl;
		}
	}

	void updateReturnOrder(ouster::sensor::ReturnOrder order)
	{
		return_order_ = order;
		// This will affect how we fill the pixel data in simulate_lidar_frame
		// For now, we'll just store the setting as we're only simulating single
		// returns
	}

	void updateIMUSettings()
	{
		if (sensor_info_.config.gyro_fsr)
		{
			switch (*sensor_info_.config.gyro_fsr)
			{
			case ouster::sensor::FullScaleRange::FSR_NORMAL:
				// Normal range: typically lower range but higher precision
				imu_gyro_scale_ = 1000.0;  // �1000 degrees per second
				break;
			case ouster::sensor::FullScaleRange::FSR_EXTENDED:
				// Extended range: higher range but lower precision
				imu_gyro_scale_ = 2000.0;  // �2000 degrees per second
				break;
			default:
				imu_gyro_scale_ = 1000.0;  // Default to normal range
			}
		}

		if (sensor_info_.config.accel_fsr)
		{
			switch (*sensor_info_.config.accel_fsr)
			{
			case ouster::sensor::FullScaleRange::FSR_NORMAL:
				// Normal range: typically lower range but higher precision
				imu_accel_scale_ = 8.0;
				break;
			case ouster::sensor::FullScaleRange::FSR_EXTENDED:
				// Extended range: higher range but lower precision
				imu_accel_scale_ = 16.0;
				break;
			default:
				imu_accel_scale_ = 8.0;  // Default to normal range
			}
		}
	}

	void updateRangeThreshold(int threshold_cm)
	{
		if (threshold_cm >= 0 && threshold_cm <= 24000)
		{  // 240m max range
			range_threshold_cm_ = threshold_cm;
		}
		else
		{
			std::cerr << "Range threshold must be between 0 and 24000 cm" << std::endl;
		}
	}

	void recalculatePacketSizes()
	{
		// For non-LEGACY profiles:
		packet_header_size_ = 32;
		col_footer_size_ = 0;
		packet_footer_size_ = 32;

		column_size_ = pf_->col_size;
		packet_size_ = pf_->lidar_packet_size;
		packet_buf_.resize(packet_size_);
	}

	void on_config_complete()
	{
		onConfigChange();
		configured_ = true;

		if (sensor_info_.config.operating_mode)
		{
			running_ = (*sensor_info_.config.operating_mode == ouster::sensor::OperatingMode::OPERATING_NORMAL);

			if (running_)
			{
				std::cout << "Starting UDP data stream with:"
					<< "\n  Destination: " << client_endpoint_.address().to_string()
					<< "\n  Port: " << client_endpoint_.port()
					<< "\n  Frame ID: " << frame_id_
					<< "\n  Packet size: " << packet_size_
					<< std::endl;
			}
			else
			{
				std::cout << "Operating mode is not NORMAL, UDP stream disabled" << std::endl;
			}
		}
	}

	void run()
	{
		while (!configured_)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		while (running_)
		{
			simulate_lidar_frame();
		}
	}

	void stop() { running_ = false; }

private:
	void read_sensor_info(const OusterConfiguration& configuration)
	{
		//sensor_info_.name = configuration.get<std::string>("sensor_info.name", "OS-1-64");
		sensor_info_.status = "INITIALIZING", // one of - INITIALIZING, STANDBY, RUNNING, UNCONFIGURED, WARMUP, ERROR
		sensor_info_.prod_line = configuration.get<std::string>("sensor_info.prod_line");
		sensor_info_.prod_pn = configuration.get<std::string>("sensor_info.prod_pn");
		sensor_info_.sn = configuration.get<std::string>("sensor_info.sn");
		sensor_info_.fw_rev = configuration.get<std::string>("sensor_info.fw_rev");
		sensor_info_.image_rev = configuration.get<std::string>("sensor_info.image_rev");
		sensor_info_.init_id = configuration.get<uint32_t>("sensor_info.init_id");
		sensor_info_.build_date = configuration.get<std::string>("sensor_info.build_date");
		sensor_info_.beam_altitude_angles = calc_altitude_angles_from_config(configuration);
		sensor_info_.beam_azimuth_angles = calc_azimuth_angles_from_config(configuration);
		sensor_info_.lidar_origin_to_beam_origin_mm = configuration.get<double>("sensor_info.lidar_origin_to_beam_origin_mm");
		sensor_info_.beam_to_lidar_transform = configuration.get<Eigen::Matrix4d>("sensor_info.beam_to_lidar_transform");
		sensor_info_.imu_to_sensor_transform = configuration.get<Eigen::Matrix4d>("sensor_info.imu_to_sensor_transform");
		sensor_info_.lidar_to_sensor_transform = configuration.get<Eigen::Matrix4d>("sensor_info.lidar_to_sensor_transform");
		sensor_info_.extrinsic = configuration.get<Eigen::Matrix4d>("sensor_info.extrinsic");

		auto window = configuration.get<std::vector<int>>("sensor_info.format.column_window");
		sensor_info_.format.column_window = std::make_pair(window[0], window[1]);

		// For UDP profiles
		std::string lidar_profile = configuration.get<std::string>("sensor_info.format.udp_profile_lidar");
		sensor_info_.format.udp_profile_lidar = ouster::sensor::udp_profile_lidar_of_string(lidar_profile).value_or(ouster::sensor::UDPProfileLidar::PROFILE_RNG19_RFL8_SIG16_NIR16);

		std::string imu_profile = configuration.get<std::string>("sensor_info.format.udp_profile_imu");
		sensor_info_.format.udp_profile_imu = ouster::sensor::udp_profile_imu_of_string(imu_profile).value_or(ouster::sensor::UDPProfileIMU::PROFILE_IMU_LEGACY);

		sensor_info_.format.pixels_per_column = configuration.get<uint32_t>("sensor_info.format.pixels_per_column");
		sensor_info_.format.columns_per_packet = configuration.get<uint32_t>("sensor_info.format.columns_per_packet");
		sensor_info_.format.columns_per_frame = configuration.get<uint32_t>("sensor_info.format.columns_per_frame");
		sensor_info_.format.pixel_shift_by_row = configuration.get<std::vector<int>>("sensor_info.format.pixel_shift_by_row");
		sensor_info_.format.fps = configuration.get<uint16_t>("sensor_info.format.fps");

		sensor_info_.config.columns_per_packet = sensor_info_.format.columns_per_packet;
		sensor_info_.config.udp_dest = configuration.get<std::string>("sensor_info.config.udp_dest");
		sensor_info_.config.udp_port_lidar = configuration.get<uint16_t>("sensor_info.config.udp_port_lidar");
		sensor_info_.config.udp_port_imu = configuration.get<uint16_t>("sensor_info.config.udp_port_lidar");

		sensor_info_.config.udp_profile_lidar = sensor_info_.format.udp_profile_lidar;
		sensor_info_.config.udp_profile_imu = sensor_info_.format.udp_profile_imu;

		std::string timestamp_mode_str = configuration.get<std::string>("sensor_info.config.timestamp_mode");
		sensor_info_.config.timestamp_mode = ouster::sensor::timestamp_mode_of_string(timestamp_mode_str);

		std::string lidar_mode_str = configuration.get<std::string>("sensor_info.config.lidar_mode");
		sensor_info_.config.lidar_mode = ouster::sensor::lidar_mode_of_string(lidar_mode_str);

		std::string operating_mode_str = configuration.get<std::string>("sensor_info.config.operating_mode");
		sensor_info_.config.operating_mode = ouster::sensor::operating_mode_of_string(operating_mode_str);

		std::string return_order_str = configuration.get<std::string>("sensor_info.config.return_order");
		sensor_info_.config.return_order = ouster::sensor::return_order_of_string(return_order_str);

		std::string gyro_fsr_str = configuration.get<std::string>("sensor_info.config.gyro_fsr");
		sensor_info_.config.gyro_fsr = ouster::sensor::full_scale_range_of_string(gyro_fsr_str);

		std::string accel_fsr_str = configuration.get<std::string>("sensor_info.config.accel_fsr");
		sensor_info_.config.accel_fsr = ouster::sensor::full_scale_range_of_string(accel_fsr_str);

		sensor_info_.config.min_range_threshold_cm = configuration.get<int>("sensor_info.config.min_range_threshold_cm");

		auto azimuth_window = configuration.get<std::vector<int>>("sensor_info.config.azimuth_window");
		sensor_info_.config.azimuth_window = std::make_pair(azimuth_window[0], azimuth_window[1]);

		sensor_info_.config.signal_multiplier = configuration.get<double>("sensor_info.config.signal_multiplier");

		auto multipurpose_io_mode_str = configuration.get<std::string>("sensor_info.config.multipurpose_io_mode");
		sensor_info_.config.multipurpose_io_mode = ouster::sensor::multipurpose_io_mode_of_string(multipurpose_io_mode_str);

		auto nmea_in_polarity_str = configuration.get<std::string>("sensor_info.config.nmea_in_polarity");
		sensor_info_.config.nmea_in_polarity = ouster::sensor::polarity_of_string(nmea_in_polarity_str);

		sensor_info_.config.nmea_ignore_valid_char = configuration.get<bool>("sensor_info.config.nmea_ignore_valid_char");

		auto nmea_baud_rate_str = configuration.get<std::string>("sensor_info.config.nmea_baud_rate");
		sensor_info_.config.nmea_baud_rate = ouster::sensor::nmea_baud_rate_of_string(nmea_baud_rate_str);

		sensor_info_.config.nmea_leap_seconds = configuration.get<int>("sensor_info.config.nmea_leap_seconds");

		auto sync_pulse_in_polarity_str = configuration.get<std::string>("sensor_info.config.sync_pulse_in_polarity");
		sensor_info_.config.sync_pulse_in_polarity = ouster::sensor::polarity_of_string(sync_pulse_in_polarity_str);

		auto sync_pulse_out_polarity_str = configuration.get<std::string>("sensor_info.config.sync_pulse_out_polarity");
		sensor_info_.config.sync_pulse_out_polarity = ouster::sensor::polarity_of_string(sync_pulse_out_polarity_str);

		sensor_info_.config.sync_pulse_out_angle = configuration.get<int>("sensor_info.config.sync_pulse_out_angle");
		sensor_info_.config.sync_pulse_out_frequency = configuration.get<int>("sensor_info.config.sync_pulse_out_frequency");
		sensor_info_.config.sync_pulse_out_pulse_width = configuration.get<int>("sensor_info.config.sync_pulse_out_pulse_width");

		sensor_info_.config.phase_lock_enable = configuration.get<bool>("sensor_info.config.phase_lock_enable");
		sensor_info_.config.phase_lock_offset = configuration.get<int>("sensor_info.config.phase_lock_offset");

		sensor_info_.cal.reflectivity_status = configuration.get<bool>("sensor_info.calibration.reflectivity_status");
		sensor_info_.cal.reflectivity_timestamp = configuration.get<std::string>("sensor_info.calibration.reflectivity_timestamp");
	}

	std::vector<double> calc_altitude_angles_from_config(const OusterConfiguration& configuration)
	{
		int num_beams = configuration.get<int>("sensor_info.num_beams");
		float min = configuration.get<float>("sensor_info.altitude_angles.min");
		float max = configuration.get<float>("sensor_info.altitude_angles.max");

		// Calculate altitude angles
		std::vector<double> alt_angles;
		alt_angles.resize(num_beams);

		double step = (double)(max - min) / (num_beams - 1);
		for (int i = 0; i < num_beams; i++)
		{
			alt_angles[i] = min + i * step;
		}

		return alt_angles;
	}

	std::vector<double> calc_azimuth_angles_from_config(const OusterConfiguration& configuration)
	{
		int num_beams = configuration.get<int>("sensor_info.num_beams");
		std::vector<double> azi_angles;

		try
		{
			float min = configuration.get<float>("sensor_info.azimuth_angles.min");
			float max = configuration.get<float>("sensor_info.azimuth_angles.max");

			azi_angles.resize(num_beams);

			double step = (double)(max - min) / (num_beams - 1);
			for (int i = 0; i < num_beams; i++)
			{
				azi_angles[i] = min + i * step;
			}
		}
		catch (...)
		{
			auto pattern = configuration.get<std::vector<double>>("sensor_info.azimuth_angles.pattern");
			azi_angles.reserve(num_beams);

			int items_per_cycle = pattern.size();
			int cycles = num_beams / items_per_cycle;

			if (num_beams % items_per_cycle != 0)
			{
				throw std::runtime_error("Number of beams must be divisible by azimuth pattern size");
			}

			for (int i = 0; i < cycles; i++)
			{
				azi_angles.insert(azi_angles.end(), pattern.begin(), pattern.end());
			}
		}

		return azi_angles;
	}

	void write_header_field_be(uint8_t* dest, uint64_t value, size_t num_bytes)
	{
		for (size_t i = 0; i < num_bytes; i++)
		{
			dest[i] = (value >> ((num_bytes - 1 - i) * 8)) & 0xFF;  // Write in big-endian order
		}
	}

	void debug_packet_format()
	{
		std::cout << "\nPacket Format Details:" << std::endl;
		std::cout << "UDP Profile: " << ouster::sensor::to_string(pf_->udp_profile_lidar) << std::endl;
		std::cout << "Packet Size: " << pf_->lidar_packet_size << std::endl;
		std::cout << "Header Size: " << pf_->packet_header_size << std::endl;
		std::cout << "Footer Size: " << pf_->packet_footer_size << std::endl;
		std::cout << "Col Size: " << pf_->col_size << std::endl;
		std::cout << "Col Header Size: " << pf_->col_header_size << std::endl;
		std::cout << "Col Footer Size: " << pf_->col_footer_size << std::endl;

		// Print available fields
		std::cout << "\nAvailable Fields:" << std::endl;
		for (auto it = pf_->begin(); it != pf_->end(); ++it)
		{
			std::cout << "  " << it->first << ": "
				<< ouster::sensor::to_string(it->second) << std::endl;
		}
	}

	void debug_packet_detailed(const std::vector<uint8_t>& packet_buf)
	{
		const uint8_t* data = packet_buf.data();
		std::cout << "\nDetailed Packet Analysis:" << std::endl;

		// Print first 64 bytes in hex
		std::cout << "First 64 bytes:" << std::endl;
		for (int i = 0; i < 64; i++)
		{
			if (i % 16 == 0) std::cout << "\n";
			std::cout << std::hex << std::setw(2) << std::setfill('0')
				<< (int)data[i] << " ";
		}
		std::cout << std::dec << std::endl;

		// Print key packet fields
		std::cout << "Packet fields:" << std::endl;
		std::cout << "  Frame ID: " << pf_->frame_id(data) << std::endl;
		std::cout << "  Init ID: " << pf_->init_id(data) << std::endl;

		const uint8_t* col0 = pf_->nth_col(0, data);
		std::cout << "  First column:" << std::endl;
		std::cout << "    Measurement ID: " << pf_->col_measurement_id(col0) << std::endl;
		std::cout << "    Timestamp: " << pf_->col_timestamp(col0) << std::endl;
		std::cout << "    Status: 0x" << std::hex << pf_->col_status(col0)
			<< std::dec << std::endl;
	}

	void debug_range_data(const img_t<uint32_t>& range_data, size_t frame_id)
	{
		std::cout << "\n=== Range Data Debug (Frame " << frame_id << ") ===" << std::endl;
		std::cout << "Range data dimensions: " << range_data.rows() << "x" << range_data.cols() << std::endl;

		// Print some sample ranges
		std::cout << "\nFirst column ranges (mm):" << std::endl;
		for (int i = 0; i < std::min(10, (int)range_data.rows()); i++)
		{
			std::cout << "Beam " << i << ": " << range_data(i, 0) << std::endl;
		}

		std::cout << "\nRange samples across columns (beam 0):" << std::endl;
		for (int j = 0; j < std::min(10, (int)range_data.cols()); j++)
		{
			std::cout << "Col " << j << ": " << range_data(0, j) << std::endl;
		}
	}

	void debug_packet(const uint8_t* packet_buf, size_t packet_idx,
		const ouster::sensor::packet_format& pf)
	{
		std::cout << "\n=== Packet " << packet_idx << " Debug ===" << std::endl;

		// Print packet headers
		std::cout << "Frame ID: " << pf.frame_id(packet_buf) << std::endl;
		std::cout << "Init ID: " << pf.init_id(packet_buf) << std::endl;

		// Debug first column in packet
		const uint8_t* col0 = pf.nth_col(0, packet_buf);
		std::cout << "\nFirst Column:" << std::endl;
		std::cout << "Measurement ID: " << pf.col_measurement_id(col0) << std::endl;
		std::cout << "Timestamp: " << pf.col_timestamp(col0) << std::endl;
		std::cout << "Status: 0x" << std::hex << pf.col_status(col0) << std::dec << std::endl;
	}

	void simulate_lidar_frame()
	{
		static auto last_frame_time = std::chrono::high_resolution_clock::now();

		if (use_pcap_)
			simulate_frame_from_pcap();
		else
			simulate_generated_frame();

		auto current_time = std::chrono::high_resolution_clock::now();
		auto frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			current_time - last_frame_time);
		//std::cout << "Frame generation took: " << frame_duration.count() << "ms" << std::endl;
		last_frame_time = current_time;
	}

	void simulate_frame_from_pcap()
	{
		static auto last_frame_time = std::chrono::high_resolution_clock::now();
		const auto frame_duration = std::chrono::microseconds(100000); // 10Hz = 100ms = 100000us

		auto frame_start = std::chrono::high_resolution_clock::now();
		auto packet_count = 0;

		// Calculate packet timing within frame
		const auto packet_interval = frame_duration / num_packets_per_frame_;

		while (packet_count < num_packets_per_frame_)
		{
			std::vector<uint8_t> packet_buf;
			if (pcap_player_->read_next_packet(packet_buf))
			{
				auto target_packet_time = frame_start + (packet_interval * packet_count);
				auto now = std::chrono::high_resolution_clock::now();

				if (now < target_packet_time)
				{
					std::this_thread::sleep_until(target_packet_time);
				}

				send_packet(packet_buf);
				packet_count++;
			}
			else
			{
				std::cout << "Failed to read next packet" << std::endl;
				break;
			}
		}

		// Wait for next frame start if needed
		auto target_next_frame = last_frame_time + frame_duration;
		auto now = std::chrono::high_resolution_clock::now();
		if (now < target_next_frame)
		{
			std::this_thread::sleep_until(target_next_frame);
		}

		last_frame_time = std::chrono::high_resolution_clock::now();

		// Log timing info
		//auto frame_end = std::chrono::high_resolution_clock::now();
		//auto actual_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
		//	frame_end - frame_start).count();
		//std::cout << "Frame generated in " << actual_duration << "ms" << std::endl;
	}

	void simulate_generated_frame()
	{
		const size_t H = pixels_per_column_;
		const size_t W = columns_per_frame_;
		using img_t = Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
		img_t range_data(H, W);

		// Create a vertical plane at distance 5m
		const double distance_mm = 5000.0;  // 5m in mm
		const auto& altitude_angles = sensor_info_.beam_altitude_angles;
		const auto& azimuth_angles = sensor_info_.beam_azimuth_angles;

		// Transformation matrices
		Eigen::Matrix4d beam_to_lidar = sensor_info_.beam_to_lidar_transform;
		Eigen::Matrix4d lidar_to_sensor = sensor_info_.lidar_to_sensor_transform;
		Eigen::Matrix4d extrinsic = sensor_info_.extrinsic;

		// Debugging: Print transformation matrices
		std::cout << "beam_to_lidar_transform:\n" << beam_to_lidar << std::endl;
		std::cout << "lidar_to_sensor_transform:\n" << lidar_to_sensor << std::endl;
		std::cout << "extrinsic:\n" << extrinsic << std::endl;

		// For each column (azimuth angle)
		for (size_t col = 0; col < W; col++)
		{
			double azimuth = (static_cast<double>(col) / W) * 2.0 * M_PI;  // 0 to 2π

			// Calculate X and Y for this azimuth at distance
			double x = distance_mm * std::cos(azimuth);
			double y = distance_mm * std::sin(azimuth);

			// Calculate range for each beam based on altitude angle
			for (size_t beam = 0; beam < H; beam++)
			{
				double altitude = altitude_angles[beam] * M_PI / 180.0;  // Convert to radians
				double z = distance_mm * std::sin(altitude);
				double range = std::sqrt(x * x + y * y + z * z);

				// Apply transformations
				Eigen::Vector4d point(x, y, z, 1.0);
				point = beam_to_lidar * point;
				point = lidar_to_sensor * point;
				point = extrinsic * point;

				// Debugging: Print transformed point
				if (col < 5 && beam < 5)
				{
					std::cout << "Original point: (" << x << ", " << y << ", " << z << ")" << std::endl;
					std::cout << "Transformed point: (" << point(0) << ", " << point(1) << ", " << point(2) << ")" << std::endl;
				}

				// Update range data
				range_data(beam, col) = static_cast<uint32_t>(range);
			}
		}

		// Debug output
		std::cout << "\nSimulated ranges for plane at distance "
			<< distance_mm << "mm" << std::endl;

		std::cout << "First 5 columns (distance should be constant):" << std::endl;
		for (size_t col = 0; col < 5; col++)
		{
			double azimuth = (static_cast<double>(col) / W) * 2.0 * M_PI;
			std::cout << "Col " << col << " (azimuth=" << (azimuth * 180.0 / M_PI)
				<< "°): " << range_data(0, col) << "mm" << std::endl;
		}

		debug_range_data(range_data, frame_id_);

		// Send packets as before
		ouster::sensor::impl::packet_writer pw(*pf_);
		const size_t total_packets = W / columns_per_packet_;

		uint64_t frame_start_ts = std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();

		for (size_t packet_idx = 0; packet_idx < total_packets; ++packet_idx)
		{
			std::memset(packet_buf_.data(), 0, packet_size_);

			// Set packet headers
			pw.set_packet_type(packet_buf_.data(), uint16_t{ 1 });
			pw.set_frame_id(packet_buf_.data(), frame_id_);
			pw.set_init_id(packet_buf_.data(), sensor_info_.init_id);
			pw.set_prod_sn(packet_buf_.data(), std::stoull(sensor_info_.sn));

			// Set column data
			for (size_t c = 0; c < columns_per_packet_; c++)
			{
				uint16_t m_id = packet_idx * columns_per_packet_ + c;
				uint8_t* col_buf = pw.nth_col(c, packet_buf_.data());

				pw.set_col_timestamp(col_buf, frame_start_ts + m_id);
				pw.set_col_measurement_id(col_buf, m_id);
				pw.set_col_status(col_buf, 0x1);
			}

			// Write range data for this packet
			auto range_view = range_data.block(0, packet_idx * columns_per_packet_,
				H, columns_per_packet_);
			pw.set_block(Eigen::Ref<const img_t>(range_view), "RANGE", packet_buf_.data());

			// Set CRC if needed
			if (pf_->udp_profile_lidar != ouster::sensor::UDPProfileLidar::PROFILE_LIDAR_LEGACY)
			{
				uint64_t crc = pf_->calculate_crc(packet_buf_.data());
				std::memcpy(packet_buf_.data() + packet_size_ - sizeof(crc),
					&crc, sizeof(crc));
			}

			debug_packet(packet_buf_.data(), packet_idx, *pf_);
			send_packet(packet_buf_);
			std::this_thread::sleep_for(packet_delay_);
		}

		frame_id_++;
	}

	void send_packet(const std::vector<uint8_t>& packet_buf)
	{
		try
		{
			lidar_socket_.send_to(
				boost::asio::buffer(packet_buf.data(), packet_buf.size()),
				client_endpoint_);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error sending packet: " << e.what() << std::endl;
		}
	}
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config.json>" << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		auto& configuration = OusterConfiguration::getInstance();
		configuration.load(argv[1]);

		boost::asio::io_context io_context;
		OusterLiDARSimulator simulator(configuration, io_context);

		OusterHTTPServer http_server(
			io_context, 
			[&simulator]() { simulator.on_config_complete(); }, 
			simulator.get_sensor_info());

		std::thread io_thread([&io_context]() { io_context.run(); });
		simulator.run();

		io_thread.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}