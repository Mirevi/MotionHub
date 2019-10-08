#include "MotionHub.h"

MotionHub::MotionHub()
{

	Console::printHeader();

	m_configReader = new ConfigReader();
	m_configReader->readConfigFile(CONFIG_PATH);

	m_networkManager = new NetworkManager();

	m_trackerManager = new TrackerManager(m_networkManager);

}

TrackerManager* MotionHub::getTrackerManager()
{ 

	return m_trackerManager; 

}