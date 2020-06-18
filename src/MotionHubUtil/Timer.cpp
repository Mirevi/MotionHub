#include "Timer.h"


std::chrono::time_point<std::chrono::steady_clock> Timer::startTime;

void Timer::reset()
{

	startTime = std::chrono::high_resolution_clock::now();

}



float Timer::getDuration()
{

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = endTime - startTime;

	return duration.count();

}