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
	static void log(char type, std::string message);
	static void printHeader();

};