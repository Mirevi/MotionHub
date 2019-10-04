#pragma once

#include "ConfigReader.h"
#include "TrackerManager.h"
#include "NetworkManager.h"
#include "Console.h"

class MotionHub
{

public:
	MotionHub();
	ConfigReader m_configReader;
	
	NetworkManager m_networkManager;

	inline TrackerManager* getTrackerManager() { return m_trackerManager; }
	//inline void setTrackerManager(TrackerManager* trackingManager) { m_trackerManager = trackingManager; }

private:
	TrackerManager* m_trackerManager;
};