#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <thread>

#include "PcapPlayer.h"


PcapPlayer::PcapPlayer(const std::string& pcap_file, bool loop)
	: pcap_filename_(pcap_file),
	  loop_(loop)
{
	try
	{
		std::cout << "Opening PCAP file: '" << pcap_file << "'" << std::endl;
		// Create a clean copy using explicit char-by-char copy
		std::string pcap_filename = pcap_filename_;
		std::vector<char> buffer(pcap_filename.begin(), pcap_filename.end());
		buffer.push_back('\0');  // Ensure null termination

		// Create PcapReader with the clean buffer
		pcap_reader_ = std::make_unique<ouster::sensor_utils::PcapReader>(std::string(buffer.data()));

		is_initialized_ = true;
		std::cout << "Successfully opened PCAP file: " << pcap_file << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to open PCAP file: " << e.what() << std::endl;
		is_initialized_ = false;
	}
}

/// <summary>
/// Read next packet from pcap file. Timing is handled externally!
/// </summary>
bool PcapPlayer::read_next_packet(std::vector<uint8_t>& packet_buf)
{
	if (!is_initialized_) return false;

	// Read next packet
	if (!pcap_reader_->next_packet())
	{
		if (loop_)
		{
			pcap_reader_->reset();
			return read_next_packet(packet_buf);
		}

		return false; // end of file
	}

	// Get packet info
	const uint8_t* data = pcap_reader_->current_data();
	size_t packet_size = pcap_reader_->current_length();

	// Copy packet data
	packet_buf.resize(packet_size);
	std::memcpy(packet_buf.data(), data, packet_size);

	if (!validate_packet(packet_buf))
	{
		return read_next_packet(packet_buf);  // Skip invalid packet
	}

	return true;
}

bool PcapPlayer::validate_packet(const std::vector<uint8_t>& packet_buf)
{
	return packet_buf.size() >= 64; // Minimum reasonable size for a lidar packet
}
