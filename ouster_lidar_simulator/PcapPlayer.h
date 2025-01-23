#pragma once

#include <memory>
#include <ouster/types.h>
#include <ouster/pcap.h>
#include <ouster/lidar_scan.h>

class PcapPlayer
{
private:
    std::unique_ptr<ouster::sensor_utils::PcapReader> pcap_reader_;
    std::string pcap_filename_;
    bool is_initialized_{ false };
    bool loop_{ false };

public:
    PcapPlayer(const std::string& pcap_file, bool loop);
    bool is_initialized() const { return is_initialized_; }
    bool read_next_packet(std::vector<uint8_t>& packet_buf);

    bool validate_packet(const std::vector<uint8_t>& packet_buf);
};