#pragma once

#include "ConfigReader.h"
#include "TrackerManager.h"
#include "NetworkManager.h"
#include "Console.h"
#include "defines.h"

class MotionHub
{

public:
	MotionHub();
	TrackerManager* getTrackerManager();

private:
	TrackerManager* m_trackerManager;
	ConfigReader* m_configReader;
	NetworkManager* m_networkManager;

};