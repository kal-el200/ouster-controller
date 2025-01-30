#pragma once

#include <memory>
#include <vector>
#include <iostream>

// Base class for callbacks without data
class CallbackBase
{
public:
    virtual ~CallbackBase() = default;
    virtual void execute() = 0;
};

// Base class for callbacks with data
template<typename DataType>
class DataCallbackBase
{
public:
    virtual ~DataCallbackBase() = default;
    virtual void execute(const DataType& data) = 0;
};

// Callback implementation without data
template<typename Func>
class Callback : public CallbackBase
{
public:
    Callback(Func&& func) : func_(std::forward<Func>(func)) {}

    void execute() override
    {
        func_();
    }

private:
    Func func_;
};

// Callback implementation with data
template<typename DataType, typename Func>
class DataCallback : public DataCallbackBase<DataType>
{
public:
    DataCallback(Func&& func) : func_(std::forward<Func>(func)) {}

    void execute(const DataType& data) override
    {
        // func_(data);
    }

private:
    Func func_;
};


// Helper function for callbacks without data
template<typename Func>
auto makeCallback(Func&& func)
{
    return std::make_unique<Callback<Func>>(std::forward<Func>(func));
}

// Helper function for callbacks with data
template<typename DataType, typename Func>
auto makeDataCallback(Func&& func)
{
    return std::make_unique<DataCallback<DataType, Func>>(std::forward<Func>(func));
}
