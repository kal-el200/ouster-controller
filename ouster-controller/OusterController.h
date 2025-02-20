#pragma once

#include <string>
#include <memory>
#include <functional>
#include "callback.h"

class OusterControllerImpl;
namespace Ouster { class OusterMsg; }

class OusterController
{
public:
    static OusterController& getInstance();
    using OusterMsgCallback = std::function<void(const Ouster::OusterMsg&)>;

    void registerOusterMsgCallback(OusterMsgCallback func);

    bool run(std::string config_path);
    ~OusterController();

    OusterController(const OusterController&) = delete;
    OusterController& operator=(const OusterController&) = delete;

private:
    OusterController();
    std::unique_ptr<OusterControllerImpl> pimpl_;
};