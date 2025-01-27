// OusterController.cpp
#include "pch.h"
#include "OusterController.h"
#include "ddsCommunicator.h"
#include "SensorProxy.h"
#include "ouster_dds_model.hpp"
#include "ouster_dds_plugin.hpp"

using namespace communicator::DDS;

const int N_FRAMES = -1;

class OusterController::Impl
{
public:
    std::unique_ptr<DDSCommunicator> dds_communicator_;
    std::vector<ouster::sensor::Sensor> sensors_;

    Impl() : dds_communicator_(std::make_unique<DDSCommunicator>()) 
    {
        DDSCommunicatorInitParams prms
        {
            .DomainId = 0,
            .QoSDefaultLibraryName = "Ouster_dds_Library",
            .QoSDefaultProfileName = "Ouster_dds_Profile"
        };

        dds_communicator_->Init(prms);
        create_dds_writers();
    }

    static OusterController& getInstance()
    {
        static OusterController instance;
        return instance;
    }

    bool run(std::string config_path)
    {
        try
        {
            auto& configuration = OusterConfiguration::getInstance();
            configuration.load(config_path);

            ouster::sensor::init_logger(configuration.get<std::string>("client.log_level"));

            sensors_ = get_sdk_sensors(configuration);
            ouster::sensor::SensorClient client(sensors_);
            auto& infos = client.get_sensor_info();

            ouster::viz::PointViz viz("Ouster Point Clouds");
            std::vector<std::unique_ptr<SensorProxy>> sensor_proxies;

            for (const auto& info : infos)
            {
                sensor_proxies.emplace_back(std::make_unique<SensorProxy>(info, viz, N_FRAMES));
            }

            std::thread capture_thread([&]() {
                while (std::any_of(sensor_proxies.begin(), sensor_proxies.end(),
                    [](auto& sp) { return sp->is_running(); }))
                {
                    ouster::sensor::LidarPacket lidar_packet;
                    ouster::sensor::ImuPacket imu_packet;
                    auto event = client.get_packet(lidar_packet, imu_packet, 1.0);
                    if (event.type == ouster::sensor::ClientEvent::EventType::LidarPacket)
                    {
                        sensor_proxies[event.source]->add_packet(lidar_packet);
                    }
                }
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

private:
    void create_dds_writers() const
    {
        try
        {
            // Create writer with explicit type parameter for Ouster Message
            auto writer = dds_communicator_->CreateWriter<Ouster::OusterMsg>("Ouster::OusterMsg");

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
};

OusterController::OusterController() : pimpl_(std::make_unique<Impl>()) {}
OusterController::~OusterController() = default;

OusterController& OusterController::getInstance()
{
    return Impl::getInstance();
}

bool OusterController::run(std::string config_path)
{
    return pimpl_->run(config_path);
}