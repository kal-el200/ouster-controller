#include "pch.h"

#include "SensorProxy.h"

#include <atomic>
#include <omp.h>


std::unique_ptr<OusterDynMessage> SensorProxy::CreateOusterMessage()
{
	auto msg = std::make_unique<OusterDynMessage>();

	// Populate message structure
	msg->m_Presence_Vector_1byte = 1;
	auto& multilayer_data = msg->m_ouster_multilayer_data;

	multilayer_data.m_msg_index = frame_counter_;

	// Set sensor model based on info_
	if (info_.prod_line == "OS-1-64")
		multilayer_data.m_sensor_model = SensorModelType::SENSOR_MODEL_TYPE_OS1_64;
	else if (info_.prod_line == "OS-1-128")
		multilayer_data.m_sensor_model = SensorModelType::SENSOR_MODEL_TYPE_OS1_128;

	// Set return mode based on UDP profile
	multilayer_data.m_return_mode = (info_.format.udp_profile_lidar == ouster::sensor::UDPProfileLidar::PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL)
		? ReturnMode::RETURN_MODE_DUAL
		: ReturnMode::RETURN_MODE_SINGLE;

	// Populate packet data
	auto& packet_dyn = multilayer_data.m_ouster_packet_array_dyn;
	packet_dyn.m_num_of_elements = static_cast<unsigned short>(current_frame_.size());

	// Process each packet
	for (size_t packet_idx = 0; packet_idx < current_frame_.size() && packet_idx < 500; packet_idx++)
	{
		auto& packet_data = packet_dyn.m_ouster_packet_struct_array[packet_idx];
		const auto& packet = current_frame_[packet_idx];

		packet_data.m_is_exists = 1;
		auto& one_azimuth = packet_data.m_ouster_one_azimuth;

		// Get first column data from packet
		const uint8_t* col_buf = pf_.nth_col(0, packet.buf.data());

		// Set azimuth measurement ID
		one_azimuth.m_rotational_direction_azimuth = pf_.col_measurement_id(col_buf);

		// Set timestamp
		uint64_t ts = pf_.col_timestamp(col_buf);
		auto time = std::chrono::system_clock::now();
		auto tm = std::chrono::system_clock::to_time_t(time);
		auto local_tm = *localtime(&tm);

		one_azimuth.m_azimuth_firing_time.miliseconds = static_cast<unsigned short>((ts / 1000000) % 1000);
		one_azimuth.m_azimuth_firing_time.Seconds = static_cast<unsigned char>(local_tm.tm_sec);
		one_azimuth.m_azimuth_firing_time.Minutes = static_cast<unsigned char>(local_tm.tm_min);
		one_azimuth.m_azimuth_firing_time.Hour = static_cast<unsigned char>(local_tm.tm_hour);
		one_azimuth.m_azimuth_firing_time.Day = static_cast<unsigned char>(local_tm.tm_mday);

		// Temporary arrays to store channel data
		std::vector<uint32_t> range_data(info_.format.pixels_per_column);
		std::vector<uint8_t> reflectivity_data(info_.format.pixels_per_column);
		std::vector<uint16_t> signal_data(info_.format.pixels_per_column);

		// Get channel data using col_field
		pf_.col_field(col_buf, ouster::sensor::ChanField::RANGE, range_data.data(), 1);
		pf_.col_field(col_buf, ouster::sensor::ChanField::REFLECTIVITY, reflectivity_data.data(), 1);
		pf_.col_field(col_buf, ouster::sensor::ChanField::SIGNAL, signal_data.data(), 1);

		std::vector<uint32_t> range2_data;
		std::vector<uint8_t> reflectivity2_data;

		if (multilayer_data.m_return_mode == ReturnMode::RETURN_MODE_DUAL)
		{
			range2_data.resize(info_.format.pixels_per_column);
			reflectivity2_data.resize(info_.format.pixels_per_column);
			pf_.col_field(col_buf, ouster::sensor::ChanField::RANGE2, range2_data.data(), 1);
			pf_.col_field(col_buf, ouster::sensor::ChanField::REFLECTIVITY2, reflectivity2_data.data(), 1);
		}

		// Process each channel/pixel in the column
		for (size_t ch = 0; ch < info_.format.pixels_per_column && ch < 128; ch++)
		{
			auto& ray_data = one_azimuth.m_ouster_ray_data[ch];

			// First returns
			ray_data.m_distance_first = static_cast<unsigned short>(range_data[ch]);
			ray_data.m_reflectivity_first = reflectivity_data[ch];
			ray_data.m_nir_value = signal_data[ch];

			// Second returns if in dual mode
			if (multilayer_data.m_return_mode == ReturnMode::RETURN_MODE_DUAL)
			{
				ray_data.m_distance_second = static_cast<unsigned short>(range2_data[ch]);
				ray_data.m_reflectivity_second = reflectivity2_data[ch];
			}
			else
			{
				ray_data.m_distance_second = 0;
				ray_data.m_reflectivity_second = 0;
			}
		}
	}

	// Set beam altitude angle type
	const auto& altitude_angles = info_.beam_altitude_angles;
	if (!altitude_angles.empty())
	{
		if (std::all_of(altitude_angles.begin(), altitude_angles.end(),
			[](double angle) { return angle >= 0; }))
		{
			msg->m_beam_altitude_angle_type = 2; // above horizon
		}
		else if (std::all_of(altitude_angles.begin(), altitude_angles.end(),
			[](double angle) { return angle <= 0; }))
		{
			msg->m_beam_altitude_angle_type = 3; // below horizon
		}
		else
		{
			msg->m_beam_altitude_angle_type = 1; // gradient
		}
	}
	else
	{
		msg->m_beam_altitude_angle_type = 0; // uniform
	}

	return msg;
}

void SensorProxy::process_current_frame()
{
	auto msg = CreateOusterMessage();

	// Execute callbacks
	for (auto& callback : callbacks_)
	{
		callback->execute(*msg);
	}

	if (!current_frame_.empty())
	{
		update_visualization();
		current_frame_.clear();
		frame_counter_++;
	}
}

void SensorProxy::init_viz()
{
	viz_.add(cloud_);
	cloud_->set_point_size(2.0);

	// Use Viridis color palette for better range visualization
	cloud_->set_palette(ouster::viz::viridis_palette[0], ouster::viz::viridis_n);
}

void SensorProxy::update_visualization()
{
	try
	{
		bool frame_complete = false;
		for (const auto& packet : current_frame_)
		{
			frame_complete = batcher_(packet, scan_);
		}

		if (frame_complete)
		{
			ouster::LidarScan::Points points;
			points = ouster::cartesian(scan_, xyz_lut_);

			// Get both range and reflectivity fields
			const auto& range_field = scan_.field<uint32_t>(ouster::sensor::ChanField::RANGE);
			const auto& reflectivity_field = scan_.field<uint8_t>(ouster::sensor::ChanField::REFLECTIVITY);

#pragma omp parallel sections 
			{
#pragma omp section
				{
					// Process points
					for (int i = 0; i < points.rows(); i++)
					{
						viz_points_[i] = float(points(i, 0));
						viz_points_[i + points.rows()] = float(points(i, 1));
						viz_points_[i + points.rows() * 2] = float(points(i, 2));
					}
				}

#pragma omp section
				{
					// Find max range
					float local_max = 0;
					for (int i = 0; i < range_field.rows(); i++)
					{
						for (int j = 0; j < range_field.cols(); j++)
						{
							float range = range_field(i, j) * 0.001f;  // Convert to meters
							local_max = std::max(local_max, range);
						}
					}
					max_range_ = local_max;
				}
			}

			// Update member variables for RGBA colors
			std::vector<float> viz_colors_rgba(points.rows() * 4);

			// Color computation combining range and reflectivity
#pragma omp parallel for
			for (int i = 0; i < points.rows(); i++)
			{
				auto row = i / scan_.w;
				auto col = i % scan_.w;

				float range = range_field(row, col) * 0.001f;  // Convert to meters
				float range_norm = range / max_range_;  // Normalize range to [0,1]

				// Normalize reflectivity (already 0-255)
				float reflectivity_norm = reflectivity_field(row, col) / 255.0f;

				// Combine range and reflectivity for coloring
				// Using range for intensity (luminance) and reflectivity for color
				float h = reflectivity_norm;  // Hue from reflectivity
				float s = 0.8f;               // Fixed saturation
				float v = 1.0f - range_norm;  // Value (brightness) from range

				// Convert HSV to RGB
				float c = v * s;
				float x = c * (1 - std::abs(std::fmod(h * 6, 2.0f) - 1));
				float m = v - c;

				float r, g, b;
				if (h < 1.0f / 6.0f) { r = c; g = x; b = 0; }
				else if (h < 2.0f / 6.0f) { r = x; g = c; b = 0; }
				else if (h < 3.0f / 6.0f) { r = 0; g = c; b = x; }
				else if (h < 4.0f / 6.0f) { r = 0; g = x; b = c; }
				else if (h < 5.0f / 6.0f) { r = x; g = 0; b = c; }
				else { r = c; g = 0; b = x; }

				// Store RGBA values
				viz_colors_rgba[i * 4 + 0] = r + m;      // Red
				viz_colors_rgba[i * 4 + 1] = g + m;      // Green
				viz_colors_rgba[i * 4 + 2] = b + m;      // Blue
				viz_colors_rgba[i * 4 + 3] = 1.0f;       // Alpha
			}

			// Update the point cloud visualization
			cloud_->set_xyz(viz_points_.data());
			cloud_->set_key_rgba(viz_colors_rgba.data());

			static const ouster::viz::mat4d flip_transform = []() {
				auto transform = ouster::viz::identity4d;
				transform[10] = -1.0;
				return transform;
				}();
			cloud_->set_pose(flip_transform);

			viz_.update();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error in visualization update: " << e.what() << std::endl;
	}
}

void SensorProxy::LogSensorInformation(const ouster::sensor::sensor_info& info)
{
	size_t w = info.format.columns_per_frame;
	size_t h = info.format.pixels_per_column;
	ouster::sensor::ColumnWindow column_window = info.format.column_window;

	std::cout << "\nConfiguration for Sensor " << info.sn << ":" << std::endl;
	std::cout << "  UDP Profile:       " << ouster::sensor::to_string(info.format.udp_profile_lidar) << std::endl;
	std::cout << "  Product line:      " << info.prod_line << std::endl;
	std::cout << "  Firmware Version:  " << info.image_rev << std::endl;
	std::cout << "  Serial number:     " << info.sn << std::endl;
	std::cout << "  Scan dimensions:   " << w << " x " << h << std::endl;
	std::cout << "  Column window:     [" << column_window.first << ", " << column_window.second << "]" << std::endl;
	std::cout << "  UDP Configuration:" << std::endl;
	std::cout << "    Destination:     " << info.config.udp_dest.value() << std::endl;
	std::cout << "    Lidar Port:      " << info.config.udp_port_lidar.value() << std::endl;
	std::cout << "    IMU Port:        " << info.config.udp_port_imu.value() << std::endl;
	std::cout << std::endl;

	if (!info.beam_altitude_angles.empty())
	{
		std::cout << "  Number of beams: " << info.beam_altitude_angles.size() << std::endl;
		std::cout << "  First 5 altitude angles:" << std::endl;
		for (size_t i = 0; i < std::min(size_t(5), info.beam_altitude_angles.size()); i++)
		{
			std::cout << "    " << info.beam_altitude_angles[i] << std::endl;
		}
	}

	if (!info.beam_azimuth_angles.empty())
	{
		std::cout << "  First 10 azimuth angles:" << std::endl;
		for (size_t i = 0; i < std::min(size_t(10), info.beam_azimuth_angles.size()); i++)
		{
			std::cout << "    " << info.beam_azimuth_angles[i] << std::endl;
		}
	}
}

void SensorProxy::add_packet(const ouster::sensor::LidarPacket& packet)
{
	try
	{
		uint32_t packet_frame_id = pf_.frame_id(packet.buf.data());

		if (current_frame_.empty())
		{
			current_frame_id_ = packet_frame_id;
		}
		else if (packet_frame_id != current_frame_id_)
		{
			process_current_frame();
			current_frame_id_ = packet_frame_id;
		}

		current_frame_.push_back(packet);

		if (frame_counter_ == N_FRAMES)
		{
			stop(); // Use the new stop() method
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error processing packet: " << e.what() << std::endl;
		stop(); // Stop on error
	}
}
