#pragma once

#include <windows.h>
#include <iostream>

#include <string>

#include <ctime>
#include <chrono>

//static class for giving putting out messages
class Console
{

public:
	static void log(std::string message);
	static void warning(std::string message);
	static void error(std::string message);
	static void printHeader();

};