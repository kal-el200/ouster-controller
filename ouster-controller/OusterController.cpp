// OusterController.cpp
#include "pch.h"
#include "OusterController.h"
#include "ddsCommunicator.h"
#include "SensorProxy.h"
#include "ouster_dds_model.hpp"
#include "ouster_dds_plugin.hpp"
#include "OusterControllerImpl.h"


OusterController::OusterController() : pimpl_(std::make_unique<OusterControllerImpl>()) {}
OusterController::~OusterController() = default;

OusterController& OusterController::getInstance()
{
    static OusterController instance;
    return instance;
}

void OusterController::registerOusterMsgCallback(std::function<void(const OusterDynMessage&)> func)
{
    if (!func)
    {
        throw std::runtime_error("Cannot register null callback");
    }
    pimpl_->registerOusterMsgCallback(std::move(func));
}

bool OusterController::run(std::string config_path)
{
    return pimpl_->run(config_path);
}