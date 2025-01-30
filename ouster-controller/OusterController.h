#pragma once

#include <string>
#include <memory>
#include <functional>
#include "ouster_data_model.h"
#include "callback.h"


class OusterControllerImpl;
class OusterController
{
public:
    static OusterController& getInstance();
    using OusterMsgCallback = std::function<void(OusterDynMessage)>;

    void registerOusterMsgCallback(std::function<void(const OusterDynMessage&)> func);

    bool run(std::string config_path);
    ~OusterController();

    OusterController(const OusterController&) = delete;
    OusterController& operator=(const OusterController&) = delete;

private:
    OusterController();
    std::unique_ptr<OusterControllerImpl> pimpl_;
};