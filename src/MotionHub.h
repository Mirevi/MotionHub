#pragma once

#include "ConfigReader.h"
#include "TrackerManager.h"
#include "NetworkManager.h"
#include "Console.h"
#include "defines.h"


/*!
 * Hub for combining different body tracking solutions and sending position and rotation data of skeleton joints via ip.
 */
class MotionHub
{

public:
	MotionHub();
	TrackerManager* getTrackerManager();
	NetworkManager* getNetworkmanager();

private:

	TrackerManager* m_trackerManager;
	ConfigReader* m_configReader;
	NetworkManager* m_networkManager;

};