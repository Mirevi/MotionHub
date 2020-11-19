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



std::string Timer::getCurrTime()
{
	auto curr_time_sec = std::chrono::system_clock::now();
	std::time_t curr_time = std::chrono::system_clock::to_time_t(curr_time_sec);
	char* time = std::ctime(&curr_time);

	if (time[strlen(time) - 1] == '\n')
	{
		time[strlen(time) - 1] = '\0';
	}

	std::string str(time);

	return str;
}
