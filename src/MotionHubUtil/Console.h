#pragma once

#include "ConfigDllExportMotionHubUtil.h"

#define _WINSOCKAPI_
#include <windows.h>

#include <iostream>
#include <fstream>

#include <thread>
#include <string>
#include <list>

#include <ctime>
#include <chrono>

#include "defines.h"

#include <atomic>
#include <mutex>
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

	/*!
	 * outputs info message to console
	 * \param message output message
	 */
	static void log(std::string message);
	/*!
	 * outputs warning message to console
	 * \param message output message
	 */
	static void logWarning(std::string message);
	/*!
	 * outputs error message to console
	 * \param message output message
	 */
	static void logError(std::string message);
	/*!
	 * prints Application header with author names 
	 */
	static void printHeader();
	/*!
	 * prints message to logfile 
	 * \param message output message
	 */
	static void writeToLogfile(std::string message);

	/*!
	 * temporary list of messages, which are pulled by the Console Widget in MainWindow
	 * 
	 */
	static std::list<std::string> messagePool;
	
};