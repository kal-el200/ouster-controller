#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <concepts>

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

template<typename F>
concept NoArgCallback = requires(F f)
{
	{ f() } -> std::same_as<void>;  // Must be callable with no arguments and return void
	requires std::movable<F>;       // Must be movable (can be moved from one object to another)
};

// Define a valid callback function
template<typename F, typename DataType>
concept ValidDataCallback = requires(F f, const DataType & data)
{
	{ f(data) } -> std::same_as<void>;  // Callback must take const DataType& and return void
};

// Callback implementation without data
template<typename Func>
	requires NoArgCallback<Func>
class Callback : public CallbackBase
{
public:
	explicit Callback(Func&& func)
		: func_(std::forward<Func>(func))
	{
		if (!func_)
		{
			throw std::runtime_error("Callback function is null during construction");
		}
	}

	void execute() override
	{
		if (!func_)
		{
			throw std::runtime_error("Callback function is null during execution");
		}
		func_();
	}

private:
	std::function<void()> func_;  // Store as std::function for consistent behavior
};

// Callback implementation with data
template<typename DataType, typename Func>
	requires ValidDataCallback<Func, DataType>
class DataCallback : public DataCallbackBase<DataType>
{
public:
	explicit DataCallback(Func&& func)
		: func_(std::forward<Func>(func))
	{
		if (!func_)
		{
			throw std::runtime_error("Callback function is null during construction");
		}
	}

	void execute(const DataType& data) override
	{
		if (!func_)
		{
			throw std::runtime_error("Callback function is null during execution");
		}
		func_(data);
	}

private:
	std::function<void(const DataType&)> func_;
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
