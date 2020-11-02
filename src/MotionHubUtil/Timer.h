#pragma once

#include "ConfigDllExportMotionHubUtil.h"


#include <chrono>
#include <string>
#include <ctime>


class MotionHubUtil_DLL_import_export Timer
{

	static std::chrono::time_point<std::chrono::steady_clock> startTime;



public:

	/*!
	* resets the Timer
	*/
	static void reset();

	/*!
	* gets the duration from the last time reset() was called
	*/
	static float getDuration();

	/*!
	* gets the current Date and Time
	*/
	static std::string getCurrTime();

};