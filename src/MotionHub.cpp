#include "MotionHub.h"
#include "defines.h"

MotionHub::MotionHub()
{

	m_configReader = new ConfigReader();
	m_configReader->readConfigFile(CONFIG_PATH);

	TrackerManager* m_trackerManager;
	NetworkManager* m_networkManager;

}