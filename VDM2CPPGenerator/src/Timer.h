#pragma once
#include "Log.h"
class Timer
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;
	std::chrono::time_point<std::chrono::high_resolution_clock> endTimePoint;
	double ms;
public:
	Timer()
	{
		ms = 0.0;
		startTimePoint = std::chrono::high_resolution_clock::now();
	}

	void Start()
	{
		startTimePoint = std::chrono::high_resolution_clock::now();
	}

	void Stop()
	{
		endTimePoint = std::chrono::high_resolution_clock::now();
		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimePoint).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

		auto duration = end - start;
		ms = duration * 0.001;
	}

	void Log()
	{
		LOG_INFO("Apllication: Execution time: {0}ms", ms);
	}
};