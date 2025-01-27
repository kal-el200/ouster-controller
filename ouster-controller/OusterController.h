#pragma once

#include <string>
#include <memory>

class OusterController
{
public:
    static OusterController& getInstance();
    bool run(std::string config_path);
    ~OusterController();

    OusterController(const OusterController&) = delete;
    OusterController& operator=(const OusterController&) = delete;

private:
    OusterController();
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};