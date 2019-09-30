#pragma once

#include "ConfigReader.h"
#include "TrackerManager.h"
#include "NetworkManager.h"

class MotionHub
{

public:
	MotionHub();

private:
	ConfigReader* m_configReader;
	TrackerManager* m_trackerManager;
	NetworkManager* m_networkManager;

};