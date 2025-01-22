#pragma once

#include <chrono>
#include <string>
#include <iostream>

class ScopedTimer
{
private:
    std::chrono::high_resolution_clock::time_point start_;
    std::string name_;

public:
    ScopedTimer(const std::string& name) :
        start_(std::chrono::high_resolution_clock::now()),
        name_(name)
    {
    }

    ~ScopedTimer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
        std::cout << name_ << " took " << duration.count() << "ms" << std::endl;
    }
};

#define PROFILE_SCOPE(name) ScopedTimer timer##__LINE__(name)