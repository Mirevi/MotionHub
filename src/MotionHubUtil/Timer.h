#pragma once

#include "ConfigDllExportMotionHubUtil.h"


#include <chrono>
#include <string>
#include <ctime>


class MotionHubUtil_DLL_import_export Timer
{

	static std::chrono::time_point<std::chrono::steady_clock> startTime;



public:

	static void reset();

	static float getDuration();

	static std::string getCurrTime();

};