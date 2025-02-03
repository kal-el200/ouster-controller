#pragma once

#include <ouster/types.h>
#include <ouster/lidar_scan.h>
#include <ouster/point_viz.h>
#include "Callback.h"

#include <vector>
#include <atomic>

namespace Ouster { class OusterMsg; }
using namespace Ouster;

class SensorProxy
{
private:
    const int N_FRAMES;  // Number of frames to capture
    const size_t W;      // cols per frame
    const size_t H;      // pixels per col
    const size_t COLS_PER_PACKET;    // cols per packet
    const size_t PACKETS_PER_FRAME;   // packets per frame
    const size_t CLOUD_SIZE;          // all points in frame

    const ouster::sensor::packet_format pf_;
    std::vector<ouster::sensor::LidarPacket> current_frame_;
    uint32_t current_frame_id_ = 0;
    int frame_counter_ = 0;
    ouster::XYZLut xyz_lut_;
    ouster::ScanBatcher batcher_;
    ouster::viz::PointViz& viz_;
    std::shared_ptr<ouster::viz::Cloud> cloud_;
    std::atomic<bool> running_;  // Initialize in constructor
    const ouster::sensor::sensor_info& info_;

    std::vector<float> viz_points_;
    float max_range_{ 100.0f };
    ouster::LidarScan scan_;  // Reuse scan object
    const std::vector<std::unique_ptr<DataCallbackBase<OusterMsg>>>& callbacks_;
    int notify_client_frequency_;
    std::unique_ptr<OusterMsg> CreateOusterMessage();
    void process_current_frame();
    void init_viz();
    void update_visualization();
    static void LogSensorInformation(const ouster::sensor::sensor_info& info);

public:
    SensorProxy(
        const ouster::sensor::sensor_info& info,
        ouster::viz::PointViz& viz,
        const std::vector<std::unique_ptr<DataCallbackBase<OusterMsg>>>& callbacks,
        const int notify_client_frequency,
        const int n_frames = -1) :
        N_FRAMES(n_frames),
        W(info.format.columns_per_frame),
        H(info.format.pixels_per_column),
        COLS_PER_PACKET(info.format.columns_per_packet),
        PACKETS_PER_FRAME(W / COLS_PER_PACKET),
        CLOUD_SIZE(W* H),
        pf_(info),
        xyz_lut_(ouster::make_xyz_lut(info, false)),
        batcher_(info),
        viz_(viz),
        cloud_(std::make_shared<ouster::viz::Cloud>(CLOUD_SIZE)),
        running_(true),  // Initialize atomic bool
        info_(info),
        scan_(W, H, info_.format.udp_profile_lidar),
        viz_points_(W* H * 3),
        notify_client_frequency_(notify_client_frequency),
        callbacks_(callbacks)
    {
        LogSensorInformation(info);
        init_viz();
    }

    // Disable copy construction and assignment
    SensorProxy(const SensorProxy&) = delete;
    SensorProxy& operator=(const SensorProxy&) = delete;

    // Add move constructor and assignment if needed
    SensorProxy(SensorProxy&&) = default;
    SensorProxy& operator=(SensorProxy&&) = default;

    void add_packet(const ouster::sensor::LidarPacket& packet);
    bool is_running() const 
    {
        auto a = running_.load(std::memory_order_acquire);
        return a;
    }

    void stop() { running_.store(false, std::memory_order_release); }
};
