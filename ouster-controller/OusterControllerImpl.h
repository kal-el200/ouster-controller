#pragma once

#include "pch.h"
#include "Callback.h"
#include <ddsCommunicator.h>
#include "cpp_util.hpp"

using namespace communicator::DDS;
using namespace cpp_util;

template<typename Func>
    requires ValidDataCallback<Func, OusterDynMessage>
class OusterMsgCallback : public DataCallback<OusterDynMessage, Func>
{
public:
    explicit OusterMsgCallback(Func&& func)
        : DataCallback<OusterDynMessage, Func>(std::forward<Func>(func))
    {}
};

// Helper function
template<typename Func>
auto makeOusterMsgCallback(Func&& func)
{
    return std::make_unique<OusterMsgCallback<Func>>(std::forward<Func>(func));
}

class OusterControllerImpl
{
private:
    const int N_FRAMES = -1;

    std::vector<std::unique_ptr<DataCallbackBase<OusterDynMessage>>> ousterMsgCallbacks_;
    std::unique_ptr<DDSCommunicator> dds_communicator_;
    std::vector<ouster::sensor::Sensor> sensors_;

    std::vector<ouster::sensor::Sensor> get_sdk_sensors(OusterConfiguration& configuration)
    {
        std::vector<ouster::sensor::Sensor> sensors;
        const std::string sensor_hostname = configuration.get<std::string>("sensor.hostname");
        std::cerr << "Connecting to \"" << sensor_hostname << "\"...\n";

        ouster::sensor::sensor_config config;
        read_sensor_config(config, configuration);

        std::cerr << "Creating sensor connection with config:\n"
            << "UDP Destination: " << config.udp_dest.value() << "\n"
            << "Lidar Port: " << config.udp_port_lidar.value() << "\n"
            << "IMU Port: " << config.udp_port_imu.value() << "\n"
            << "Mode: " << configuration.get<std::string>("sensor.lidar_mode") << "\n";

        ouster::sensor::Sensor sensor(sensor_hostname, config);
        std::cerr << "Sensor connection created\n";

        sensors.push_back(sensor);
        return sensors;
    }

    void read_sensor_config(ouster::sensor::sensor_config& config, OusterConfiguration& configuration)
    {
        config.udp_dest = configuration.get<std::string>("sensor.udp_dest");
        config.udp_port_lidar = configuration.get<int>("sensor.udp_port_lidar");
        config.udp_port_imu = configuration.get<int>("sensor.udp_port_imu");

        config.operating_mode = configuration.get<std::string>("sensor.operating_mode") == "NORMAL"
            ? ouster::sensor::OPERATING_NORMAL : ouster::sensor::OPERATING_STANDBY;

        config.lidar_mode = ouster::sensor::lidar_mode_of_string(
            configuration.get<std::string>("sensor.lidar_mode"));
        config.timestamp_mode = ouster::sensor::timestamp_mode_of_string(
            configuration.get<std::string>("sensor.timestamp_mode"));
        config.multipurpose_io_mode = ouster::sensor::multipurpose_io_mode_of_string(
            configuration.get<std::string>("sensor.multipurpose_io_mode"));

        auto azimuth_window = configuration.get<std::vector<int>>("sensor.azimuth_window");
        config.azimuth_window = { azimuth_window[0], azimuth_window[1] };

        config.signal_multiplier = configuration.get<double>("sensor.signal_multiplier");
        config.columns_per_packet = configuration.get<int>("sensor.columns_per_packet");

        config.udp_profile_lidar = ouster::sensor::udp_profile_lidar_of_string(
            configuration.get<std::string>("sensor.udp_profile_lidar"));
        config.udp_profile_imu = ouster::sensor::udp_profile_imu_of_string(
            configuration.get<std::string>("sensor.udp_profile_imu"));

        config.nmea_in_polarity = ouster::sensor::polarity_of_string(
            configuration.get<std::string>("sensor.nmea_in_polarity"));
        config.nmea_baud_rate = ouster::sensor::nmea_baud_rate_of_string(
            configuration.get<std::string>("sensor.nmea_baud_rate"));

        config.nmea_ignore_valid_char = configuration.get<bool>("sensor.nmea_ignore_valid_char");
        config.nmea_leap_seconds = configuration.get<int>("sensor.nmea_leap_seconds");

        config.sync_pulse_in_polarity = ouster::sensor::polarity_of_string(
            configuration.get<std::string>("sensor.sync_pulse_in_polarity"));
        config.sync_pulse_out_polarity = ouster::sensor::polarity_of_string(
            configuration.get<std::string>("sensor.sync_pulse_out_polarity"));

        config.sync_pulse_out_frequency = configuration.get<int>("sensor.sync_pulse_out_frequency");
        config.sync_pulse_out_angle = configuration.get<int>("sensor.sync_pulse_out_angle");
        config.sync_pulse_out_pulse_width = configuration.get<int>("sensor.sync_pulse_out_pulse_width");
        config.phase_lock_enable = configuration.get<bool>("sensor.phase_lock_enable");
        config.phase_lock_offset = configuration.get<int>("sensor.phase_lock_offset");
    }

    void run_visualization(ouster::viz::PointViz& viz)
    {
        viz.camera().set_yaw(-180.0f);
        viz.camera().set_pitch(15.0f);
        viz.camera().dolly(15.0);
        viz.camera().set_fov(90.0f);

        ouster::viz::add_default_controls(viz);

        std::cout << "Starting visualization for sensors" << std::endl;
        viz.run();
    }

    void create_dds_writers() const
    {
        try
        {
            // Create writer with explicit type parameter for Ouster Message
            auto writer = dds_communicator_->CreateWriter<Ouster::OusterMsg>("VehicleQosLib::OusterMsg");

            if (writer == dds::core::null)
            {
                std::cerr << "Failed to create DDS writer for OusterMsg" << std::endl;
                return;
            }

            std::cout << "Successfully created DDS writer for OusterMsg" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error creating DDS writer: " << e.what() << std::endl;
        }
    }

    void publish_ouster_msg(const Ouster::OusterMsg& msg) const
    {
        try
        {
            if (!dds_communicator_->Write("Ouster::OusterMsg", msg))
            {
                std::cerr << "Failed to publish OusterMsg" << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error publishing ouster message: " << e.what() << std::endl;
        }
    }

public:
    OusterControllerImpl() : dds_communicator_(std::make_unique<DDSCommunicator>())
    {
        DDSCommunicatorInitParams prms
        {
            .DomainId = 0,
            .CommunicatorName = "OusterCommunicator",
            .QoSXmlUri = getExeDirectory() + "\\USER_QOS_PROFILES.xml",
            .QoSDefaultLibraryName = "VehicleQosLib",
            .QoSDefaultProfileName = "OusterMsg",
        };

        dds_communicator_->Init(prms);
        create_dds_writers();
    }

    template<typename Func>
    void registerOusterMsgCallback(Func&& func)
    {
        ousterMsgCallbacks_.push_back(makeOusterMsgCallback(func));
    }

    bool run(std::string config_path)
    {
        try
        {
            if (!dds_communicator_)
            {
                std::cerr << "DDS communicator not initialized" << std::endl;
                return false;
            }

            std::cout << "Loading configuration..." << std::endl;
            auto& configuration = OusterConfiguration::getInstance();
            configuration.load(config_path);

            std::cout << "Initializing logger..." << std::endl;
            ouster::sensor::init_logger(configuration.get<std::string>("client.log_level"));

            std::cout << "Getting SDK sensors..." << std::endl;
            sensors_ = get_sdk_sensors(configuration);
            std::cout << "Sensors array size: " << sensors_.size() << std::endl;
            
            if (sensors_.empty())
            {
                std::cerr << "No sensors were created" << std::endl;
                return false;
            }

            std::cout << "Creating sensor client..." << std::endl;
            ouster::sensor::SensorClient client(sensors_);

            std::cout << "Getting sensor info..." << std::endl;
            auto& infos = client.get_sensor_info();

            std::cout << "Creating point viz..." << std::endl;
            ouster::viz::PointViz viz("Ouster Point Clouds");

            std::cout << "Creating sensor proxies..." << std::endl;
            std::vector<std::unique_ptr<SensorProxy>> sensor_proxies;

            for (const auto& info : infos)
            {
                std::cout << "Creating sensor proxy " << info.sn << std::endl;
                sensor_proxies.emplace_back(
                    std::make_unique<SensorProxy>(info, viz, ousterMsgCallbacks_, N_FRAMES));
            }

            std::thread capture_thread([&]() {
                run_capture_thread(sensor_proxies, client);
            });

            run_visualization(viz);
            if (capture_thread.joinable()) capture_thread.join();
            return true;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    void run_capture_thread(std::vector<std::unique_ptr<SensorProxy>>& sensor_proxies,
        ouster::sensor::SensorClient& client)
    {
        try
        {
            while (std::any_of(sensor_proxies.begin(), sensor_proxies.end(),
                [](const auto& sp) { return sp && sp->is_running(); }))
            {
                ouster::sensor::LidarPacket lidar_packet;
                ouster::sensor::ImuPacket imu_packet;
                auto event = client.get_packet(lidar_packet, imu_packet, 1.0);
                if (event.type == ouster::sensor::ClientEvent::EventType::LidarPacket)
                {
                    if (event.source < sensor_proxies.size() && sensor_proxies[event.source])
                    {
                        sensor_proxies[event.source]->add_packet(lidar_packet);
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Capture thread error: " << e.what() << std::endl;
            // Stop all proxies on error
            for (auto& proxy : sensor_proxies)
            {
                if (proxy) proxy->stop();
            }
        }
    }
};
