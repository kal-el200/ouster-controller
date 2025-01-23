#pragma once

#include <cstdint>
#include <iostream>
#include <iomanip>
#include <ouster/types.h>

class FrameValidator
{
private:
    struct FrameStats
    {
        uint32_t first_frame_id{ 0 };
        uint32_t packet_count{ 0 };
        uint32_t last_measurement_id{ 0 };
        bool initialized{ false };
    };

    FrameStats current_frame_;
    uint32_t last_frame_id_{ 0 };
    bool first_frame_{ true };

public:
    void reset()
    {
        current_frame_ = FrameStats{};
        first_frame_ = true;
    }

    bool validate_packet(const ouster::sensor::packet_format& pf, const uint8_t* data)
    {
        uint32_t frame_id = pf.frame_id(data);
        uint16_t measurement_id = pf.col_measurement_id(pf.nth_col(0, data));

        // Debug raw bytes of frame ID
        const uint8_t* frame_id_bytes = data + 16; // Adjust offset based on packet format
        std::cout << "Frame ID raw bytes: ";
        for (int i = 0; i < 4; i++)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                << (int)frame_id_bytes[i] << " ";
        }
        std::cout << std::dec << " -> Frame ID: " << frame_id << std::endl;

        if (!current_frame_.initialized)
        {
            current_frame_.initialized = true;
            current_frame_.first_frame_id = frame_id;
            current_frame_.last_measurement_id = measurement_id;
            current_frame_.packet_count = 1;
            return true;
        }

        if (frame_id != current_frame_.first_frame_id)
        {
            // New frame detected
            if (!first_frame_)
            {
                std::cout << "Frame complete - Stats:"
                    << "\n  Frame ID: " << current_frame_.first_frame_id
                    << "\n  Packets: " << current_frame_.packet_count
                    << "\n  Frame ID delta: " << (frame_id - last_frame_id_)
                    << std::endl;
            }

            last_frame_id_ = current_frame_.first_frame_id;
            current_frame_ = FrameStats{};
            current_frame_.initialized = true;
            current_frame_.first_frame_id = frame_id;
            current_frame_.packet_count = 1;
            current_frame_.last_measurement_id = measurement_id;
            first_frame_ = false;
            return true;
        }

        // Validate measurement ID sequence
        if (measurement_id <= current_frame_.last_measurement_id &&
            current_frame_.packet_count > 1)
        {
            std::cout << "WARNING: Non-sequential measurement IDs: previous="
                << current_frame_.last_measurement_id
                << " current=" << measurement_id << std::endl;
        }

        current_frame_.last_measurement_id = measurement_id;
        current_frame_.packet_count++;
        return true;
    }
};