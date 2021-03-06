#include "Console.h"

//_CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

//redeclaration
std::vector<Console::Message> Console::m_messagePool;

//outputs a string in console as info text
void Console::log(std::string _message)
{
	//static member that works without a object
	static std::atomic<bool> g_consoleIsLocked(false);

	//chek if console is locked
	while (g_consoleIsLocked.load())
	{

		//wait 5 milliseconds before trying again
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}

	//lock console
	g_consoleIsLocked.store(true);

	HANDLE  handle_console;
	handle_console = GetStdHandle(STD_OUTPUT_HANDLE);

	auto curr_time_sec = std::chrono::system_clock::now();
	std::time_t curr_time = std::chrono::system_clock::to_time_t(curr_time_sec);
	char* time = std::ctime(&curr_time);

	if (time[strlen(time) - 1] == '\n')
	{
		time[strlen(time) - 1] = '\0';
	}

	std::string timeString(time);
	std::string textString(timeString + " [INFO]: " + _message);

	Console::writeToLogfile(textString);

	Message currMessage;

	currMessage.time	= timeString;
	currMessage.type	= "[INFO]";
	currMessage.message = _message;

	m_messagePool.push_back(currMessage);

	SetConsoleTextAttribute(handle_console, 8);
	std::cout << time;
	SetConsoleTextAttribute(handle_console, 2);
	std::cout << " [INFO]: ";
	SetConsoleTextAttribute(handle_console, 15);
	std::cout << _message << std::endl;

	SetConsoleTextAttribute(handle_console, 15);

	g_consoleIsLocked.store(false);

}

//outputs a string in console as warning
void Console::logWarning(std::string _message)
{

	//static member that works without a object
	static std::atomic<bool> g_consoleIsLocked(false);

	//chek if console is locked
	while (g_consoleIsLocked.load())
	{

		//wait 5 milliseconds before trying again
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}

	//lock console
	g_consoleIsLocked.store(true);

	HANDLE  handle_console;
	handle_console = GetStdHandle(STD_OUTPUT_HANDLE);

	auto curr_time_sec = std::chrono::system_clock::now();
	std::time_t curr_time = std::chrono::system_clock::to_time_t(curr_time_sec);
	char* time = std::ctime(&curr_time);

	if (time[strlen(time) - 1] == '\n')
	{
		time[strlen(time) - 1] = '\0';
	}

	std::string str(time);

	Console::writeToLogfile(str + " [WARNING]: " + _message);


	Message currMessage;

	currMessage.time = str;
	currMessage.type = "[WARNING]";
	currMessage.message = _message;

	m_messagePool.push_back(currMessage);

	SetConsoleTextAttribute(handle_console, 8);
	std::cout << time;
	SetConsoleTextAttribute(handle_console, 6);
	std::cout << " [WARNING]: ";
	SetConsoleTextAttribute(handle_console, 15);
	std::cout << _message << std::endl;

	SetConsoleTextAttribute(handle_console, 15);

	g_consoleIsLocked.store(false);

}

//outputs a string in console as error
void Console::logError(std::string _message)
{

	//static member that works without a object
	static std::atomic<bool> g_consoleIsLocked(false);

	//chek if console is locked
	while (g_consoleIsLocked.load())
	{

		//wait 5 milliseconds before trying again
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}

	//lock console
	g_consoleIsLocked.store(true);

	HANDLE  handle_console;
	handle_console = GetStdHandle(STD_OUTPUT_HANDLE);

	auto curr_time_sec = std::chrono::system_clock::now();
	std::time_t curr_time = std::chrono::system_clock::to_time_t(curr_time_sec);
	char* time = std::ctime(&curr_time);

	if (time[strlen(time) - 1] == '\n')
	{
		time[strlen(time) - 1] = '\0';
	}

	std::string str(time);

	Console::writeToLogfile(str + " [ERROR]: " + _message);

	Message currMessage;

	currMessage.time = str;
	currMessage.type = "[ERROR]";
	currMessage.message = _message;

	m_messagePool.push_back(currMessage);

	SetConsoleTextAttribute(handle_console, 8);
	std::cout << time;
	SetConsoleTextAttribute(handle_console, 4);
	std::cout << " [ERROR]: ";
	SetConsoleTextAttribute(handle_console, 15);
	std::cout << _message << std::endl;

	SetConsoleTextAttribute(handle_console, 15);

	g_consoleIsLocked.store(false);

}


// outputs the header in console, showing names of the developers and institute
void Console::printHeader()
{

	HANDLE handle_console;
	handle_console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(handle_console, 14);

	std::cout << R"(
        :::   :::    :::::::: ::::::::::: ::::::::::: ::::::::  ::::    ::: :::    ::: :::    ::: :::::::::
      :+:+: :+:+:  :+:    :+:    :+:         :+:    :+:    :+: :+:+:   :+: :+:    :+: :+:    :+: :+:    :+:
    +:+ +:+:+ +:+ +:+    +:+    +:+         +:+    +:+    +:+ :+:+:+  +:+ +:+    +:+ +:+    +:+ +:+    +:+
   +#+  +:+  +#+ +#+    +:+    +#+         +#+    +#+    +:+ +#+ +:+ +#+ +#++:++#++ +#+    +:+ +#++:++#+
  +#+       +#+ +#+    +#+    +#+         +#+    +#+    +#+ +#+  +#+#+# +#+    +#+ +#+    +#+ +#+    +#+
 #+#       #+# #+#    #+#    #+#         #+#    #+#    #+# #+#   #+#+# #+#    #+# #+#    #+# #+#    #+#
###       ###  ########     ###     ########### ########  ###    #### ###    ###  ########  #########
	)" << std::endl;

	SetConsoleTextAttribute(handle_console, 15);

	std::cout << "Mirevi MotionHub" << std::endl;;
	std::cout << "Combines different body tracking solutions and sends position and rotation data of skeleton joints via ip.\n" << std::endl;;
	std::cout << "MotionHub is created at MIREVI within the scope of the project HIVE.\nCurrently MotionHub is developed by Kester Evers, Eric Jansen, Manuel Zohlen and supervised by Philipp Ladwig.\n" << std::endl;;

}

void Console::writeToLogfile(std::string message)
{

	std::ofstream stream;

	stream.open(LOGFILE, std::ofstream::app);

	stream << message << std::endl;
}

std::vector<Console::Message> Console::getMessages()
{
	
	std::vector<Console::Message> tempPool = m_messagePool;

	m_messagePool.clear();

	return tempPool;

}