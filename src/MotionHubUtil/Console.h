#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#include <windows.h>
#include <iostream>
#include <fstream>

#include <string>

#include <ctime>
#include <chrono>

#include "defines.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>


/*!
 * \class Console
 *
 * \brief Utility Class for Console output
 *
 * \author Eric Jansen
 */
class MotionHubUtil_DLL_import_export Console
{

public:
	static void log(std::string message);
	static void logWarning(std::string message);
	static void logError(std::string message);
	static void printHeader();
	static void writeToLogfile(std::string message);
	
	
};