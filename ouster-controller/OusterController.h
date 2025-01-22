#pragma once

#include <string>
#include <vector>

namespace ouster
{
    namespace sensor { class Sensor; struct sensor_config; }
    namespace viz { class PointViz; }
}
class OusterConfiguration;

class OusterController
{
public:
    static OusterController& getInstance();

    OusterController(const OusterController&) = delete;
    OusterController& operator=(const OusterController&) = delete;

    bool run(std::string config_path);

private:
    OusterController() = default;

    std::vector<ouster::sensor::Sensor> get_sdk_sensors(OusterConfiguration& configuration);
    void read_sensor_config(ouster::sensor::sensor_config& config, OusterConfiguration& configuration);
    void run_visualization(ouster::viz::PointViz& viz);
};