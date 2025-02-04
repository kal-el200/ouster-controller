#include "pch.h"

#include "SensorProxy.h"

#include <atomic>
#include <omp.h>
#include <ouster_dds_model.hpp>

using namespace Ouster;

void SensorProxy::process_current_frame()
{
	update_visualization();
	current_frame_.clear();
	frame_counter_++;
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

std::unique_ptr<OusterMsg> SensorProxy::CreateOusterMessage(
	const ouster::LidarScan& scan,
	const std::vector<ouster::sensor::LidarPacket>& packets)
{
	auto msg = std::make_unique<OusterMsg>();

	const auto& it = SENSOR_MODEL_TYPES.find(info_.prod_line);
	if (it == SENSOR_MODEL_TYPES.end())
	{
		std::cout << "No sensor model type was found for product line " << info_.prod_line << std::endl;
		return nullptr;
	}

	msg->sensor_model_type(it->second);

	bool is_dual_returns = (
		info_.format.udp_profile_lidar == ouster::sensor::UDPProfileLidar::PROFILE_RNG19_RFL8_SIG16_NIR16_DUAL ||
		info_.format.udp_profile_lidar == ouster::sensor::UDPProfileLidar::PROFILE_FUSA_RNG15_RFL8_NIR8_DUAL);

	msg->return_mode(is_dual_returns ? 1 : 0);

	auto& ouster_packet = msg->ouster_packet();
	size_t num_firings = std::min(size_t{ 600 }, size_t{ scan.w });
	ouster_packet.resize(num_firings);

	const auto& range_field = scan.field<uint32_t>(ouster::sensor::ChanField::RANGE);
	const auto& reflectivity_field = scan.field<uint8_t>(ouster::sensor::ChanField::REFLECTIVITY);
	const auto& signal_field = scan.field<uint16_t>(ouster::sensor::ChanField::SIGNAL);
	const auto& near_ir_field = scan.field<uint16_t>(ouster::sensor::ChanField::NEAR_IR);

	for (size_t i = 0; i < num_firings; i++)
	{
		auto& firing = ouster_packet[i];

		// Calculate which packet and column within packet this firing corresponds to
		size_t packet_idx = i / COLS_PER_PACKET;
		size_t col_within_packet = i % COLS_PER_PACKET;

		if (packet_idx >= packets.size())
		{
			std::cerr << "Packet index out of bounds: " << packet_idx << " >= " << packets.size() << std::endl;
			continue;
		}

		const uint8_t* col_buf = pf_.nth_col(col_within_packet, packets[packet_idx].buf.data());
		if (!col_buf)
		{
			std::cerr << "Invalid column buffer for packet " << packet_idx << " column " << col_within_packet << std::endl;
			continue;
		}

		uint16_t measurement_id = pf_.col_measurement_id(col_buf);
		uint16_t azimuth = (measurement_id * 36000) / W;
		firing.rotational_direction_azimuth(azimuth);

		firing.azimuth_firing_time(std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count());

		auto& ray_data = firing.ouster_ray_data();
		ray_data.resize(scan.h);

		for (size_t j = 0; j < scan.h; j++)
		{
			auto& ray = ray_data[j];
			ray.distance_first(range_field(j, i));
			ray.reflectivity_first(reflectivity_field(j, i));
			ray.nir_value(near_ir_field(j, i));
		}
	}

	msg->msg_index(msg_index_++);
	msg->beam_altitude_angle_type(beam_altitude_angle_type_);

	return msg;
}

void SensorProxy::add_packet(const ouster::sensor::LidarPacket& packet)
{
	static std::vector<ouster::sensor::LidarPacket> message_packets;

	try
	{
		uint32_t packet_frame_id = pf_.frame_id(packet.buf.data());

		// Frame management
		if (current_frame_.empty())
		{
			current_frame_id_ = packet_frame_id;
		}
		else if (packet_frame_id != current_frame_id_)
		{
			process_current_frame();
			current_frame_id_ = packet_frame_id;
			message_packets.clear();
		}

		// Add to current frame (for visualization)
		current_frame_.push_back(packet);

		// Message packet management
		message_packets.push_back(packet);

		if (message_packets.size() >= NUM_PACKETS_PER_MESSAGE)
		{
			// Calculate actual number of columns based on packets
			size_t cols_in_message = message_packets.size() * COLS_PER_PACKET;

			// Create temporary sensor info with modified format
			auto temp_info = info_;  // Copy the sensor info
			temp_info.format.columns_per_frame = cols_in_message;  // Set to actual columns we have

			// Create temporary scan object sized for just these packets
			ouster::LidarScan temp_scan(cols_in_message, H, info_.format.udp_profile_lidar);
			ouster::ScanBatcher temp_batcher(temp_info);

			for (const auto& p : message_packets)
			{
				temp_batcher(p, temp_scan);
			}

			auto msg = CreateOusterMessage(temp_scan, message_packets);
			if (msg != nullptr)
			{
				for (auto& callback : callbacks_)
				{
					callback->execute(*msg);
				}
			}

			message_packets.clear();
		}

		if (frame_counter_ == N_FRAMES)
		{
			stop();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error processing packet: " << e.what() << std::endl;
		stop();
	}
}