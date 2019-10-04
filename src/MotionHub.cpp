#include "MotionHub.h"
#include "defines.h"

MotionHub::MotionHub()
{
	m_trackerManager = new TrackerManager();
	Console::printHeader();

	m_configReader = ConfigReader();
	m_configReader.readConfigFile(CONFIG_PATH);
	m_trackerManager = new TrackerManager();
	m_networkManager = NetworkManager();

}