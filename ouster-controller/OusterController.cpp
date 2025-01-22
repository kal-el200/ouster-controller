#include "pch.h"
#include "OusterController.h"
#include <random>
#include "SensorProxy.hpp"

#include <ouster/lidar_scan.h>
#include <ouster/sensor_scan_source.h>
#include <ouster/types.h>
#include <ouster_configuration.hpp>

using namespace ouster;

const int N_FRAMES = -1;

OusterController& OusterController::getInstance()
{
    static OusterController instance;
    return instance;
}

bool OusterController::run(std::string config_path)
{
    try
    {
        auto& configuration = OusterConfiguration::getInstance();
        configuration.load(config_path);

        ouster::sensor::init_logger(configuration.get<std::string>("client.log_level"));
        //std::cerr << "Ouster Client / SDK version " << ouster::SDK_VERSION << std::endl;

        std::vector<sensor::Sensor> sensors = get_sdk_sensors(configuration);

        // Create sensor client and get information
        ouster::sensor::SensorClient client(sensors);
        std::cerr << "Connection to sensors succeeded" << std::endl;

        auto& infos = client.get_sensor_info();

        ouster::viz::PointViz viz("Ouster Point Clouds");
        std::vector<std::unique_ptr<SensorProxy>> sensor_proxies;
        for (const auto& info : infos)
        {
            sensor_proxies.emplace_back(std::make_unique<SensorProxy>(info, viz, N_FRAMES));
        }

        // Start scan capture thread
        std::thread capture_thread([&]() {
            std::cout << "Capturing scans... " << std::endl;

            // As long as any sensor is running, keep capturing
            while (std::any_of(sensor_proxies.begin(), sensor_proxies.end(),
                [](auto& sp) { return sp->is_running(); }))
            {
                ouster::sensor::LidarPacket lidar_packet;
                ouster::sensor::ImuPacket imu_packet;

                auto event = client.get_packet(lidar_packet, imu_packet, 1.0);

                if (event.type == ouster::sensor::ClientEvent::EventType::LidarPacket)
                {
                    int sensor_idx = event.source;
                    sensor_proxies[sensor_idx]->add_packet(lidar_packet);
                }
            }
            });

        run_visualization(viz);

        if (capture_thread.joinable())
            capture_thread.join();

        std::cout << "Program completed successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<ouster::sensor::Sensor> OusterController::get_sdk_sensors(OusterConfiguration& configuration)
{
    std::vector<sensor::Sensor> sensors;

    const std::string sensor_hostname = configuration.get<std::string>("sensor.hostname");
    std::cerr << "Connecting to \"" << sensor_hostname << "\"...\n";

    sensor::sensor_config config;
    read_sensor_config(config, configuration);

    std::cerr << "Creating sensor connection with config:\n"
        << "UDP Destination: " << config.udp_dest.value() << "\n"
        << "Lidar Port: " << config.udp_port_lidar.value() << "\n"
        << "IMU Port: " << config.udp_port_imu.value() << "\n"
        << "Mode: " << configuration.get<std::string>("sensor.lidar_mode") << "\n";

    sensor::Sensor sensor(sensor_hostname, config);
    std::cerr << "Sensor connection created\n";

    sensors.push_back(sensor);
    return sensors;
}

void OusterController::read_sensor_config(ouster::sensor::sensor_config& config, OusterConfiguration& configuration)
{
    config.udp_dest = configuration.get<std::string>("sensor.udp_dest");
    config.udp_port_lidar = configuration.get<int>("sensor.udp_port_lidar");
    config.udp_port_imu = configuration.get<int>("sensor.udp_port_imu");

    // Ensure UDP socket is properly bound
    config.operating_mode = configuration.get<std::string>("sensor.operating_mode") == "NORMAL"
        ? sensor::OPERATING_NORMAL
        : sensor::OPERATING_STANDBY;
    config.lidar_mode =
        sensor::lidar_mode_of_string(configuration.get<std::string>("sensor.lidar_mode"));

    config.timestamp_mode = sensor::timestamp_mode_of_string(
        configuration.get<std::string>("sensor.timestamp_mode"));

    config.multipurpose_io_mode = sensor::multipurpose_io_mode_of_string(
        configuration.get<std::string>("sensor.multipurpose_io_mode"));

    auto azimuth_window_vec = configuration.get<std::vector<int>>("sensor.azimuth_window");
    config.azimuth_window = { azimuth_window_vec[0], azimuth_window_vec[1] };

    config.signal_multiplier = configuration.get<double>("sensor.signal_multiplier");
    config.columns_per_packet = configuration.get<int>("sensor.columns_per_packet");

    config.udp_profile_lidar = sensor::udp_profile_lidar_of_string(
        configuration.get<std::string>("sensor.udp_profile_lidar"));

    config.udp_profile_imu = sensor::udp_profile_imu_of_string(
        configuration.get<std::string>("sensor.udp_profile_imu"));

    config.nmea_in_polarity = sensor::polarity_of_string(
        configuration.get<std::string>("sensor.nmea_in_polarity"));

    config.nmea_baud_rate = sensor::nmea_baud_rate_of_string(
        configuration.get<std::string>("sensor.nmea_baud_rate"));

    config.nmea_ignore_valid_char = configuration.get<bool>("sensor.nmea_ignore_valid_char");
    config.nmea_leap_seconds = configuration.get<int>("sensor.nmea_leap_seconds");

    config.sync_pulse_in_polarity = sensor::polarity_of_string(
        configuration.get<std::string>("sensor.sync_pulse_in_polarity"));

    config.sync_pulse_out_polarity = sensor::polarity_of_string(
        configuration.get<std::string>("sensor.sync_pulse_out_polarity"));

    config.sync_pulse_out_frequency = configuration.get<int>("sensor.sync_pulse_out_frequency");
    config.sync_pulse_out_angle = configuration.get<int>("sensor.sync_pulse_out_angle");
    config.sync_pulse_out_pulse_width = configuration.get<int>("sensor.sync_pulse_out_pulse_width");
    config.phase_lock_enable = configuration.get<bool>("sensor.phase_lock_enable");
    config.phase_lock_offset = configuration.get<int>("sensor.phase_lock_offset");
}

void OusterController::run_visualization(ouster::viz::PointViz& viz)
{
    viz.camera().set_yaw(-180.0f);   // Look at +X direction
    viz.camera().set_pitch(15.0f);  // Look slightly downward

    //viz.camera().birds_eye_view();
    viz.camera().dolly(15.0);        // Move back to see full cloud
    viz.camera().set_fov(90.0f);     // Wide FOV

    ouster::viz::add_default_controls(viz);

    // Run visualization in main thread
    std::cout << "Starting visualization for sensors" << std::endl;
    viz.run();
}