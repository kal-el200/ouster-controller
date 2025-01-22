#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <functional>
#include <memory>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <ouster/types.h>

using json = nlohmann::json;

namespace http = boost::beast::http;

class OusterHTTPServer
{
private:
	ouster::sensor::sensor_info sensor_info_;
	boost::asio::io_context& io_context_;
	boost::asio::ip::tcp::acceptor acceptor_;
	std::function<void()> on_config_complete_;

public:
	OusterHTTPServer(boost::asio::io_context& ioc,
		std::function<void()> config_callback,
		ouster::sensor::sensor_info& info) :
		io_context_(ioc),
		on_config_complete_(config_callback),
		sensor_info_(info),
		acceptor_(ioc)
	{
		auto& bind_address = sensor_info_.config.udp_dest.value();

		acceptor_.open(boost::asio::ip::tcp::v4());
		acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor_.bind({ boost::asio::ip::make_address(bind_address), 80 });
		acceptor_.listen();
		accept();
	}

	ouster::sensor::sensor_info& get_info() { return sensor_info_; }

private:
	void accept()
	{
		acceptor_.async_accept(
			[this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					std::make_shared<HTTPSession>(std::move(socket), on_config_complete_, *this)->start();
				}
				accept();
			});
	}

	class HTTPSession : public std::enable_shared_from_this<HTTPSession>
	{
		boost::asio::ip::tcp::socket socket_;
		boost::beast::flat_buffer buffer_;
		http::request<http::string_body> request_;
		http::response<http::string_body> response_;
		boost::asio::steady_timer timer_;
		std::function<void()> on_config_complete_;
		OusterHTTPServer& server_;

	public:
		HTTPSession(boost::asio::ip::tcp::socket socket, std::function<void()> config_callback,
			OusterHTTPServer& server)
			: socket_(std::move(socket)),
			timer_(socket_.get_executor()),
			on_config_complete_(config_callback),
			server_(server)
		{
			std::cout << "New HTTP session from "
				<< socket_.remote_endpoint().address().to_string()
				<< ":" << socket_.remote_endpoint().port() << std::endl;
		}

		void start() { read_request(); }

	private:
		std::string array_to_json(const std::vector<double>& arr)
		{
			std::string result = "[";
			for (size_t i = 0; i < arr.size(); ++i)
			{
				if (i > 0) result += ",";
				result += std::to_string(arr[i]);
			}
			return result + "]";
		}

		std::string array_to_json(const std::vector<int>& arr)
		{
			std::string result = "[";
			for (size_t i = 0; i < arr.size(); ++i)
			{
				if (i > 0) result += ",";
				result += std::to_string(arr[i]);
			}
			return result + "]";
		}

		std::string matrix4d_to_json(const Eigen::Matrix4d& matrix)
		{
			std::string result = "[";
			for (int i = 0; i < 16; ++i)
			{
				if (i > 0) result += ",";
				result += std::to_string(matrix.data()[i]);
			}
			return result + "]";
		}

		std::string url_decode(const std::string& encoded)
		{
			std::string decoded;
			decoded.reserve(encoded.length());

			for (std::size_t i = 0; i < encoded.length(); ++i)
			{
				if (encoded[i] == '%' && i + 2 < encoded.length())
				{
					// Handle percent-encoded characters
					std::string hex = encoded.substr(i + 1, 2);
					char dec = static_cast<char>(std::stoi(hex, nullptr, 16));
					decoded += dec;
					i += 2;
				}
				else if (encoded[i] == '+')
				{
					// Convert + to space
					decoded += ' ';
				}
				else
				{
					decoded += encoded[i];
				}
			}

			return decoded;
		}

		void read_request()
		{
			auto self = shared_from_this();

			std::cout << "Starting new request read: "
				<< "Socket open: " << socket_.is_open()
				<< " Connected: " << socket_.remote_endpoint().address().to_string()
				<< ":" << socket_.remote_endpoint().port() << std::endl;

			// Clear any previous request data
			request_ = {};
			buffer_.consume(buffer_.size());

			http::async_read(socket_, buffer_, request_,
				[self](boost::system::error_code ec, std::size_t bytes_transferred)
				{
					if (ec == http::error::end_of_stream)
					{
						std::cout << "Socket closed by client" << std::endl;
						return;
					}

					if (ec)
					{
						std::cerr << "Read error: " << ec.message()
							<< " (code: " << ec.value() << ")" << std::endl;
						return;
					}

					std::cout << "\nReceived request -> "
						<< self->request_.method_string() << " "
						<< self->request_.target() << "\n";

					// Log all headers
					std::cout << "Headers:" << std::endl;
					for (auto const& field : self->request_)
					{
						std::cout << "  " << field.name() << ": " << field.value() << "\n";
					}

					if (!self->request_.body().empty())
					{
						std::cout << "Body: " << self->request_.body() << std::endl;
					}

					self->process_request();
				});
		}

		void process_request()
		{
			response_.version(request_.version());
			response_.keep_alive(request_.keep_alive());
			response_.set(http::field::server, "OusterSimulator/1.0");
			response_.set(http::field::content_type, "application/json");
			response_.set(http::field::access_control_allow_origin, "*");

			std::cout << "Processing " << request_.method_string()
				<< " request for " << request_.target()
				<< " (keep-alive: " << request_.keep_alive() << ")"
				<< std::endl;

			std::string target(request_.target());

			if (target == "/api/v1/sensor/metadata")
			{
				response_.result(http::status::ok);
				const auto& info = server_.get_info();

				response_.body() = "{";
				response_.body() += "\"beam_intrinsics\": {";
				response_.body() += "\"beam_altitude_angles\": " + array_to_json(info.beam_altitude_angles) + ",";
				response_.body() += "\"beam_azimuth_angles\": " + array_to_json(info.beam_azimuth_angles) + ",";
				response_.body() += "\"beam_to_lidar_transform\": " + matrix4d_to_json(info.beam_to_lidar_transform) + ",";
				response_.body() += "\"lidar_origin_to_beam_origin_mm\": " + std::to_string(info.lidar_origin_to_beam_origin_mm);
				response_.body() += "},";

				response_.body() += "\"sensor_info\": {";
				response_.body() += "\"hostname\": \"" + info.config.udp_dest.value() + "\",";
				response_.body() += "\"sn\": \"" + info.sn + "\",";
				response_.body() += "\"prod_sn\": \"" + info.sn + "\",";
				response_.body() += "\"fw_rev\": \"" + info.fw_rev + "\",";
				response_.body() += "\"mode\": \"" + ouster::sensor::to_string(info.config.lidar_mode.value()) + "\",";
				response_.body() += "\"prod_line\": \"" + info.prod_line + "\",";
				response_.body() += "\"prod_pn\": \"" + info.prod_pn + "\",";
				response_.body() += "\"build_rev\": \"v" + info.fw_rev + "\",";
				response_.body() += "\"build_date\": \"" + info.build_date + "\",";
				response_.body() += "\"image_rev\": \"" + info.image_rev + "\",";
				response_.body() += "\"status\": \"" + info.status + "\",";
				response_.body() += "\"initialization_id\": " + std::to_string(info.init_id);
				response_.body() += "},";

				response_.body() += "\"imu_intrinsics\": {";
				response_.body() += "\"imu_to_sensor_transform\": " + matrix4d_to_json(info.imu_to_sensor_transform);
				response_.body() += "},";

				response_.body() += "\"lidar_intrinsics\": {";
				response_.body() += "\"lidar_to_sensor_transform\": " + matrix4d_to_json(info.lidar_to_sensor_transform);
				response_.body() += "},";

				response_.body() += "\"lidar_data_format\": {";
				response_.body() += "\"columns_per_frame\": " + std::to_string(info.format.columns_per_frame) + ",";
				response_.body() += "\"pixels_per_column\": " + std::to_string(info.format.pixels_per_column) + ",";
				response_.body() += "\"columns_per_packet\": " + std::to_string(info.format.columns_per_packet) + ",";
				response_.body() += "\"column_window\": [" + std::to_string(info.format.column_window.first) +
					"," + std::to_string(info.format.column_window.second) + "],";
				response_.body() += "\"pixel_shift_by_row\": " + array_to_json(info.format.pixel_shift_by_row) + ",";
				response_.body() += "\"udp_profile_lidar\": \"" + ouster::sensor::to_string(info.format.udp_profile_lidar) + "\",";
				response_.body() += "\"udp_profile_imu\": \"" + ouster::sensor::to_string(info.format.udp_profile_imu) + "\",";
				response_.body() += "\"fps\": " + std::to_string(info.format.fps);
				response_.body() += "},";

				response_.body() += "\"calibration_status\": {";
				response_.body() += "\"reflectivity\": {";
				response_.body() += "\"timestamp\": \"" + info.cal.reflectivity_timestamp.value_or("1970-01-01T00:00:00Z") + "\",";
				response_.body() += "\"valid\": " + std::string(info.cal.reflectivity_status.value_or(false) ? "true" : "false");
				response_.body() += "}},";

				response_.body() += "\"config_params\": {";
				response_.body() += "\"udp_dest\": \"" + *info.config.udp_dest + "\",";
				response_.body() += "\"udp_ip\": \"" + *info.config.udp_dest + "\",";
				response_.body() += "\"udp_port_lidar\": " + std::to_string(*info.config.udp_port_lidar) + ",";
				response_.body() += "\"udp_port_imu\": " + std::to_string(*info.config.udp_port_imu) + ",";
				response_.body() += "\"lidar_mode\": \"" + ouster::sensor::to_string(*info.config.lidar_mode) + "\",";
				response_.body() += "\"timestamp_mode\": \"" + ouster::sensor::to_string(*info.config.timestamp_mode) + "\",";
				response_.body() += "\"azimuth_window\": [" + std::to_string(info.config.azimuth_window->first) + "," + std::to_string(info.config.azimuth_window->second) + "],";
				response_.body() += "\"signal_multiplier\": " + std::to_string(*info.config.signal_multiplier) + ",";
				response_.body() += "\"columns_per_packet\": " + std::to_string(*info.config.columns_per_packet) + ",";
				response_.body() += "\"udp_profile_lidar\": \"" + ouster::sensor::to_string(*info.config.udp_profile_lidar) + "\",";
				response_.body() += "\"udp_profile_imu\": \"" + ouster::sensor::to_string(*info.config.udp_profile_imu) + "\",";
				response_.body() += "\"nmea_in_polarity\": \"" + ouster::sensor::to_string(*info.config.nmea_in_polarity) + "\",";
				response_.body() += "\"nmea_baud_rate\": \"" + ouster::sensor::to_string(*info.config.nmea_baud_rate) + "\",";
				response_.body() += "\"nmea_ignore_valid_char\": " + std::to_string(*info.config.nmea_ignore_valid_char ? 1 : 0) + ",";
				response_.body() += "\"nmea_leap_seconds\": " + std::to_string(*info.config.nmea_leap_seconds) + ",";
				response_.body() += "\"sync_pulse_in_polarity\": \"" + ouster::sensor::to_string(*info.config.sync_pulse_in_polarity) + "\",";
				response_.body() += "\"sync_pulse_out_polarity\": \"" + ouster::sensor::to_string(*info.config.sync_pulse_out_polarity) + "\",";
				response_.body() += "\"sync_pulse_out_angle\": " + std::to_string(*info.config.sync_pulse_out_angle) + ",";
				response_.body() += "\"sync_pulse_out_frequency\": " + std::to_string(*info.config.sync_pulse_out_frequency) + ",";
				response_.body() += "\"sync_pulse_out_pulse_width\": " + std::to_string(*info.config.sync_pulse_out_pulse_width) + ",";
				response_.body() += "\"phase_lock_enable\": " + std::string(*info.config.phase_lock_enable ? "true" : "false") + ",";
				response_.body() += "\"phase_lock_offset\": " + std::to_string(*info.config.phase_lock_offset) + ",";
				response_.body() += "\"gyro_fsr\": \"" + ouster::sensor::to_string(*info.config.gyro_fsr) + "\",";
				response_.body() += "\"accel_fsr\": \"" + ouster::sensor::to_string(*info.config.accel_fsr) + "\",";
				response_.body() += "\"return_order\": \"" + ouster::sensor::to_string(*info.config.return_order) + "\",";
				response_.body() += "\"multipurpose_io_mode\": \"" + ouster::sensor::to_string(*info.config.multipurpose_io_mode) + "\",";
				response_.body() += "\"min_range_threshold_cm\": " + std::to_string(*info.config.min_range_threshold_cm) + ",";
				response_.body() += "\"auto_start_flag\": false,";
				response_.body() += "\"operating_mode\": \"" + ouster::sensor::to_string(*info.config.operating_mode) + "\"";
				response_.body() += "},";  // Close config_params

				response_.body() += "\"ouster-sdk\": {";
				response_.body() += "\"extrinsic\": " + matrix4d_to_json(info.extrinsic);
				response_.body() += "}";

				response_.body() += "}";
			}
			else if (target == "/api/v1/sensor/metadata/sensor_info")
			{
				response_.result(http::status::ok);
				auto& info = server_.get_info();
				info.status = "RUNNING";

				response_.body() = "{";
				response_.body() += "\"hostname\": \"" + info.config.udp_dest.value() + "\",";
				response_.body() += "\"prod_line\": \"" + info.prod_line + "\",";
				response_.body() += "\"prod_pn\": \"" + info.prod_pn + "\",";
				response_.body() += "\"prod_sn\": \"" + info.sn + "\",";
				response_.body() += "\"fw_rev\": \"" + info.fw_rev + "\",";
				response_.body() += "\"image_rev\": \"" + info.image_rev + "\",";
				response_.body() += "\"build_rev\": \"" + info.fw_rev + "\",";
				response_.body() += "\"proto_rev\": \"" + info.fw_rev + "\",";
				response_.body() += "\"build_date\": \"" + info.build_date + "\",";
				response_.body() += "\"status\": \"" + info.status + "\",";
				response_.body() += "\"init_id\": " + std::to_string(info.init_id);
				response_.body() += "}";
			}
			else if (target == "/api/v1/system/firmware")
			{
				response_.result(http::status::ok);
				auto& info = server_.get_info();

				response_.body() = "{";
				response_.body() += "\"fw\": \"" + info.fw_rev + "\",";
				response_.body() += "\"image_rev\": \"" + info.image_rev + "\",";
				response_.body() += "\"build_rev\": \"" + info.fw_rev + "\",";
				response_.body() += "\"status\": \"" + info.status + "\"";
				response_.body() += "}";
			}
			else if (target.starts_with("/api/v1/sensor/cmd/set_config_param"))
			{
				try
				{
					std::cout << "Processing set_config_param request" << std::endl;

					// Extract the JSON from the URL parameter
					std::string args_prefix = "?args=.+";
					size_t args_pos = target.find(args_prefix);

					if (args_pos == std::string::npos)
					{
						throw std::runtime_error("No args parameter found");
					}

					// Get the URL-encoded JSON string
					std::string encoded_json = target.substr(args_pos + args_prefix.length());

					// URL decode the JSON string
					auto decoded_json = url_decode(encoded_json);

					std::cout << "Decoded JSON: " << decoded_json << std::endl;

					// Parse the JSON
					json request_json;
					try
					{
						request_json = json::parse(decoded_json);
					}
					catch (const json::parse_error& e)
					{
						std::cerr << "JSON parse error: " << e.what() << std::endl;
						throw;
					}

					std::cout << "Parsed JSON: " << request_json.dump(2) << std::endl;

					bool changed = false;
					auto& config = server_.get_info().config;

					// Network configuration
					if (request_json.contains("udp_dest"))
					{
						config.udp_dest = request_json["udp_dest"].get<std::string>();
						changed = true;
					}
					if (request_json.contains("udp_port_lidar"))
					{
						config.udp_port_lidar = request_json["udp_port_lidar"].get<uint16_t>();
						changed = true;
					}
					if (request_json.contains("udp_port_imu"))
					{
						config.udp_port_imu = request_json["udp_port_imu"].get<uint16_t>();
						changed = true;
					}

					// Mode settings
					if (request_json.contains("lidar_mode"))
					{
						auto lidar_mode_str = request_json["lidar_mode"].get<std::string>();
						auto mode = ouster::sensor::lidar_mode_of_string(lidar_mode_str);

						if (mode != ouster::sensor::lidar_mode::MODE_UNSPEC)
						{
							config.lidar_mode = mode;
							changed = true;
						}
						else
						{
							throw std::runtime_error("Invalid lidar_mode");
						}
					}
					if (request_json.contains("operating_mode"))
					{
						auto mode =
							ouster::sensor::operating_mode_of_string(request_json["operating_mode"].get<std::string>());
						if (mode)
						{
							config.operating_mode = mode;
							changed = true;
						}
						else
						{
							throw std::runtime_error("Invalid operating_mode");
						}
					}
					if (request_json.contains("timestamp_mode"))
					{
						auto mode =
							ouster::sensor::timestamp_mode_of_string(request_json["timestamp_mode"].get<std::string>());
						if (mode != ouster::sensor::timestamp_mode::TIME_FROM_UNSPEC)
						{
							config.timestamp_mode = mode;
							changed = true;
						}
						else
						{
							throw std::runtime_error("Invalid timestamp_mode");
						}
					}

					// Profile settings
					if (request_json.contains("udp_profile_lidar"))
					{
						auto profile_str = request_json["udp_profile_lidar"].get<std::string>();
						auto profile = ouster::sensor::udp_profile_lidar_of_string(profile_str);

						if (profile)
						{
							config.udp_profile_lidar = profile;
							changed = true;
						}
						else
						{
							throw std::runtime_error("Invalid udp_profile_lidar");
						}
					}
					if (request_json.contains("udp_profile_imu"))
					{
						auto profile = ouster::sensor::udp_profile_imu_of_string(
							request_json["udp_profile_imu"].get<std::string>());
						if (profile)
						{
							config.udp_profile_imu = profile;
							changed = true;
						}
						else
						{
							throw std::runtime_error("Invalid udp_profile_imu");
						}
					}

					// IO settings
					if (request_json.contains("multipurpose_io_mode"))
					{
						auto mode = ouster::sensor::multipurpose_io_mode_of_string(
							request_json["multipurpose_io_mode"].get<std::string>());
						if (mode)
						{
							config.multipurpose_io_mode = mode;
							changed = true;
						}
						else
						{
							throw std::runtime_error("Invalid multipurpose_io_mode");
						}
					}

					// Timing settings
					if (request_json.contains("sync_pulse_in_polarity"))
					{
						auto pol = ouster::sensor::polarity_of_string(
							request_json["sync_pulse_in_polarity"].get<std::string>());
						if (pol)
						{
							config.sync_pulse_in_polarity = pol;
							changed = true;
						}
					}
					if (request_json.contains("sync_pulse_out_polarity"))
					{
						auto pol = ouster::sensor::polarity_of_string(
							request_json["sync_pulse_out_polarity"].get<std::string>());
						if (pol)
						{
							config.sync_pulse_out_polarity = pol;
							changed = true;
						}
					}
					if (request_json.contains("sync_pulse_out_frequency"))
					{
						config.sync_pulse_out_frequency = request_json["sync_pulse_out_frequency"].get<int>();
						changed = true;
					}
					if (request_json.contains("sync_pulse_out_angle"))
					{
						config.sync_pulse_out_angle = request_json["sync_pulse_out_angle"].get<int>();
						changed = true;
					}
					if (request_json.contains("sync_pulse_out_pulse_width"))
					{
						config.sync_pulse_out_pulse_width =
							request_json["sync_pulse_out_pulse_width"].get<int>();
						changed = true;
					}

					// NMEA settings
					if (request_json.contains("nmea_in_polarity"))
					{
						auto pol =
							ouster::sensor::polarity_of_string(request_json["nmea_in_polarity"].get<std::string>());
						if (pol)
						{
							config.nmea_in_polarity = pol;
							changed = true;
						}
					}
					if (request_json.contains("nmea_baud_rate"))
					{
						auto rate =
							ouster::sensor::nmea_baud_rate_of_string(request_json["nmea_baud_rate"].get<std::string>());
						if (rate)
						{
							config.nmea_baud_rate = rate;
							changed = true;
						}
					}
					if (request_json.contains("nmea_ignore_valid_char"))
					{
						// Get as number first, then convert to bool
						config.nmea_ignore_valid_char = request_json["nmea_ignore_valid_char"].get<int>() != 0;
						changed = true;
					}
					if (request_json.contains("nmea_leap_seconds"))
					{
						config.nmea_leap_seconds = request_json["nmea_leap_seconds"].get<int>();
						changed = true;
					}

					// Phase lock settings
					if (request_json.contains("phase_lock_enable"))
					{
						config.phase_lock_enable = request_json["phase_lock_enable"].get<bool>();
						changed = true;
					}
					if (request_json.contains("phase_lock_offset"))
					{
						config.phase_lock_offset = request_json["phase_lock_offset"].get<int>();
						changed = true;
					}

					// Other settings
					if (request_json.contains("columns_per_packet"))
					{
						config.columns_per_packet = request_json["columns_per_packet"].get<int>();
						changed = true;
					}
					if (request_json.contains("azimuth_window"))
					{
						auto window = request_json["azimuth_window"].get<std::vector<int>>();
						if (window.size() == 2)
						{
							config.azimuth_window = std::make_pair(window[0], window[1]);
							changed = true;
						}
					}
					if (request_json.contains("signal_multiplier"))
					{
						double multiplier = request_json["signal_multiplier"].get<double>();
						ouster::sensor::check_signal_multiplier(multiplier);  // Validates the value
						config.signal_multiplier = multiplier;
						changed = true;
					}

					// FSR settings
					if (request_json.contains("gyro_fsr"))
					{
						auto fsr =
							ouster::sensor::full_scale_range_of_string(request_json["gyro_fsr"].get<std::string>());
						if (fsr)
						{
							config.gyro_fsr = fsr;
							changed = true;
						}
					}
					if (request_json.contains("accel_fsr"))
					{
						auto fsr =
							ouster::sensor::full_scale_range_of_string(request_json["accel_fsr"].get<std::string>());
						if (fsr)
						{
							config.accel_fsr = fsr;
							changed = true;
						}
					}

					// Return order settings
					if (request_json.contains("return_order"))
					{
						auto order =
							ouster::sensor::return_order_of_string(request_json["return_order"].get<std::string>());
						if (order)
						{
							config.return_order = order;
							changed = true;
						}
					}

					if (request_json.contains("min_range_threshold_cm"))
					{
						config.min_range_threshold_cm = request_json["min_range_threshold_cm"].get<int>();
						changed = true;
					}

					if (changed)
					{
						on_config_complete_();  // Notify simulator
					}

					response_.result(http::status::ok);
					response_.body() = "\"set_config_param\"";
				}
				catch (const std::exception& e)
				{
					response_.result(http::status::bad_request);
					auto error = std::string(e.what());
					response_.body() = "{\"error\": \"" + error + "\"}";
					std::cerr << "Error setting config param: " << error << std::endl;
				}
			}
			else if (target == "/api/v1/sensor/cmd/get_config_param?args=active")
			{
				response_.result(http::status::ok);
				const auto& info = server_.get_info();
				const auto& config = info.config;

				response_.body() = "{";
				response_.body() += "\"udp_ip\": \"" + *config.udp_dest + "\",";
				response_.body() += "\"udp_dest\": \"" + *config.udp_dest + "\",";
				response_.body() += "\"udp_port_lidar\": " + std::to_string(*config.udp_port_lidar) + ",";
				response_.body() += "\"udp_port_imu\": " + std::to_string(*config.udp_port_imu) + ",";
				response_.body() += "\"timestamp_mode\": \"" + ouster::sensor::to_string(*config.timestamp_mode) + "\",";
				response_.body() += "\"sync_pulse_in_polarity\": \"" + ouster::sensor::to_string(*info.config.sync_pulse_in_polarity) + "\",";
				response_.body() += "\"sync_pulse_out_polarity\": \"" + ouster::sensor::to_string(*info.config.sync_pulse_out_polarity) + "\",";
				response_.body() += "\"sync_pulse_out_angle\": " + std::to_string(*info.config.sync_pulse_out_angle) + ",";
				response_.body() += "\"sync_pulse_out_frequency\": " + std::to_string(*info.config.sync_pulse_out_frequency) + ",";
				response_.body() += "\"sync_pulse_out_pulse_width\": " + std::to_string(*info.config.sync_pulse_out_pulse_width) + ",";
				response_.body() += "\"phase_lock_enable\": " + std::string(*info.config.phase_lock_enable ? "true" : "false") + ",";
				response_.body() += "\"phase_lock_offset\": " + std::to_string(*info.config.phase_lock_offset) + ",";
				response_.body() += "\"lidar_mode\": \"" + ouster::sensor::to_string(*config.lidar_mode) + "\",";
				response_.body() += "\"operating_mode\": \"" + ouster::sensor::to_string(*config.operating_mode) + "\",";
				response_.body() += "\"multipurpose_io_mode\": \"" + ouster::sensor::to_string(*config.multipurpose_io_mode) + "\",";
				response_.body() += "\"nmea_in_polarity\": \"" + ouster::sensor::to_string(*info.config.nmea_in_polarity) + "\",";
				response_.body() += "\"nmea_baud_rate\": \"" + ouster::sensor::to_string(*info.config.nmea_baud_rate) + "\",";
				response_.body() += "\"nmea_ignore_valid_char\": " + std::to_string(*info.config.nmea_ignore_valid_char ? 1 : 0) + ",";
				response_.body() += "\"nmea_leap_seconds\": " + std::to_string(*info.config.nmea_leap_seconds) + ",";
				response_.body() += "\"auto_start_flag\": false,";

				if (config.phase_lock_enable)
					response_.body() += "\"phase_lock_enable\": " + std::string(*config.phase_lock_enable ? "true" : "false") + ",";

				if (config.phase_lock_offset)
					response_.body() += "\"phase_lock_offset\": " + std::to_string(*config.phase_lock_offset) + ",";

				response_.body() += "\"columns_per_packet\": " + std::to_string(*config.columns_per_packet) + ",";
				response_.body() += "\"udp_profile_lidar\": \"" + ouster::sensor::to_string(*config.udp_profile_lidar) + "\",";
				response_.body() += "\"udp_profile_imu\": \"" + ouster::sensor::to_string(*config.udp_profile_imu) + "\",";

				if (config.azimuth_window)
				{
					response_.body() += "\"azimuth_window\": [" +
						std::to_string(config.azimuth_window->first) + "," +
						std::to_string(config.azimuth_window->second) + "],";
				}

				if (config.signal_multiplier)
					response_.body() += "\"signal_multiplier\": " + std::to_string(*config.signal_multiplier) + ",";

				if (config.sync_pulse_out_frequency)
					response_.body() += "\"sync_pulse_out_frequency\": " + std::to_string(*config.sync_pulse_out_frequency) + ",";

				if (config.sync_pulse_out_angle)
					response_.body() += "\"sync_pulse_out_angle\": " + std::to_string(*config.sync_pulse_out_angle) + ",";

				if (config.sync_pulse_out_pulse_width)
					response_.body() += "\"sync_pulse_out_pulse_width\": " + std::to_string(*config.sync_pulse_out_pulse_width);

				// Remove trailing comma if present
				if (response_.body().back() == ',')
					response_.body().pop_back();

				response_.body() += "}";
			}
			else if (target == "/api/v1/sensor/cmd/get_config_param?staged")
			{
				response_.result(http::status::ok);
				const auto& config = server_.get_info().config;

				response_.body() = "{";
				response_.body() += "\"udp_dest\": \"" + *config.udp_dest + "\",";
				response_.body() += "\"udp_port_lidar\": " + std::to_string(*config.udp_port_lidar) + ",";
				response_.body() += "\"udp_port_imu\": " + std::to_string(*config.udp_port_imu) + ",";
				response_.body() += "\"timestamp_mode\": \"" + ouster::sensor::to_string(*config.timestamp_mode) + "\",";
				response_.body() += "\"lidar_mode\": \"" + ouster::sensor::to_string(*config.lidar_mode) + "\",";
				response_.body() += "\"operating_mode\": \"" + ouster::sensor::to_string(*config.operating_mode) + "\",";
				response_.body() += "\"multipurpose_io_mode\": \"" + ouster::sensor::to_string(*config.multipurpose_io_mode) + "\",";
				response_.body() += "\"columns_per_packet\": " + std::to_string(*config.columns_per_packet) + ",";
				response_.body() += "\"udp_profile_lidar\": \"" + ouster::sensor::to_string(*config.udp_profile_lidar) + "\",";
				response_.body() += "\"udp_profile_imu\": \"" + ouster::sensor::to_string(*config.udp_profile_imu) + "\"";
				response_.body() += "}";
			}
			else if (target == "/api/v1/sensor/cmd/set_udp_dest_auto")
			{
				response_.result(http::status::ok);
				response_.body() = "{}";
			}
			else if (target == "/api/v1/sensor/cmd/reinitialize")
			{
				response_.result(http::status::ok);
				response_.body() = "{}";
			}
			else
			{
				response_.result(http::status::not_found);
				response_.body() = R"({"error": "Endpoint not found"})";
			}

			response_.prepare_payload();
			write_response();
		}

		void write_response()
		{
			auto self = shared_from_this();

			std::cout << "\nPreparing to send response:"
				<< "\nStatus: " << response_.result_int()
				<< "\nKeep-alive: " << response_.keep_alive()
				<< "\nBody size: " << response_.body().size()
				<< "\nBody: " << response_.body() << std::endl;

			// Set headers using operator()
			response_.set(http::field::content_length, std::to_string(response_.body().size()));
			response_.set(http::field::connection, response_.keep_alive() ? "keep-alive" : "close");

			http::async_write(socket_, response_,
				[self](boost::system::error_code ec, std::size_t bytes_transferred)
				{
					if (ec)
					{
						std::cerr << "Write error: " << ec.message() << std::endl;
						return;
					}

					std::cout << "Successfully wrote " << bytes_transferred
						<< " bytes" << std::endl;

					if (self->request_.keep_alive())
					{
						std::cout << "Connection is keep-alive, reading next request"
							<< std::endl;
						self->read_request();
					}
					else
					{
						std::cout << "Connection is not keep-alive, closing gracefully"
							<< std::endl;

						boost::system::error_code shutdown_ec;
						self->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send,
							shutdown_ec);
						if (shutdown_ec)
						{
							std::cerr << "Socket shutdown error: "
								<< shutdown_ec.message() << std::endl;
						}
					}
				});
		}
	};
};