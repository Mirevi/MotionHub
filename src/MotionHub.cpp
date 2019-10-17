#include "MotionHub.h"

MotionHub::MotionHub()
{

	auto curr_time_sec = std::chrono::system_clock::now();
	std::time_t curr_time = std::chrono::system_clock::to_time_t(curr_time_sec);
	char* time = std::ctime(&curr_time);

	if (time[strlen(time) - 1] == '\n')
	{
		time[strlen(time) - 1] = '\0';
	}

	std::string str(time);
	m_startTime = str;

	Console::printHeader();

	m_configReader = new ConfigReader();
	m_configReader->readConfigFile(CONFIG_PATH);

	m_networkManager = new NetworkManager();
	m_gestureManager = new GestureManager();

	m_trackerManager = new TrackerManager(m_networkManager, m_gestureManager);

}

TrackerManager* MotionHub::getTrackerManager()
{ 

	return m_trackerManager; 

}

NetworkManager* MotionHub::getNetworkmanager()
{

	return m_networkManager;

}

std::string MotionHub::getStartTime()
{

	return m_startTime;

}

GestureManager* MotionHub::getGestureManager()
{

	return m_gestureManager;

}