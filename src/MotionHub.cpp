#include "MotionHub.h"
#include "defines.h"

MotionHub::MotionHub()
{

	Console::printHeader();

	m_configReader = ConfigReader();
	m_configReader.readConfigFile(CONFIG_PATH);
	m_trackerManager = TrackerManager();
	m_networkManager = NetworkManager();

}