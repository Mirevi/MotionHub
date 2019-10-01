#include "Console.h"

//_CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

//outputs a string in console
void Console::log(char type, std::string message)
{
	std::string output;

	HANDLE  handle_console;
	handle_console = GetStdHandle(STD_OUTPUT_HANDLE);

	auto curr_time_sec = std::chrono::system_clock::now();
	std::time_t curr_time = std::chrono::system_clock::to_time_t(curr_time_sec);
	char* time = std::ctime(&curr_time);

	if (time[strlen(time) - 1] == '\n')
	{
		time[strlen(time) - 1] = '\0';
	}

	switch (type)
	{

	case 'i':
		SetConsoleTextAttribute(handle_console, 8);
		std::cout << time;
		SetConsoleTextAttribute(handle_console, 2);
		std::cout << " [INFO]: ";
		SetConsoleTextAttribute(handle_console, 15);
		std::cout << message << std::endl;
		break;

	case 'w':
		SetConsoleTextAttribute(handle_console, 8);
		std::cout << time;
		SetConsoleTextAttribute(handle_console, 6);
		std::cout << " [WARNING]: ";
		SetConsoleTextAttribute(handle_console, 15);
		std::cout << message << std::endl;
		break;

	case 'e':
		SetConsoleTextAttribute(handle_console, 8);
		std::cout << time;
		SetConsoleTextAttribute(handle_console, 4);
		std::cout << " [ERROR]: ";
		SetConsoleTextAttribute(handle_console, 15);
		std::cout << message << std::endl;
		break;

	case 'f':
		SetConsoleTextAttribute(handle_console, 14);
		std::cout << message << std::endl;
		break;

	default:
		std::cout << message << std::endl;
		break;

	}

	SetConsoleTextAttribute(handle_console, 15);
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

	log('h', "Mirevi MotionHub");
	log('h', "Processes skeleton data and sends osc skeleton udp packets over ip.");
	std::cout << std::endl;
	log('h', "Created by Kester Evers and Eric Jansen at MIREVI (mirevi.de).");
	std::cout << std::endl;

}