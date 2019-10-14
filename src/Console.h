#pragma once

#include <windows.h>
#include <iostream>
#include <fstream>

#include <string>

#include <ctime>
#include <chrono>

#include "defines.h"

//static class for giving putting out messages
class Console
{

public:
	static void log(std::string message);
	static void logWarning(std::string message);
	static void logError(std::string message);
	static void printHeader();
	static void writeToLogfile(std::string message);

};