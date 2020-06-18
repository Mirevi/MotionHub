#pragma once

#include "ConfigDllExportMotionHubUtil.h"


#include <chrono>


class MotionHubUtil_DLL_import_export Timer
{

	static std::chrono::time_point<std::chrono::steady_clock> startTime;



public:

	static void reset();

	static float getDuration();

};