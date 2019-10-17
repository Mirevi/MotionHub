#pragma once

#include "ConfigReader.h"
#include "TrackerManager.h"
#include "NetworkManager.h"
#include "GestureManager.h"

#include "Console.h"

#include <string>

#include "defines.h"

/*!
 * \class MotionHub
 *
 * \brief Hub for combining different body tracking solutions and sending position and rotation data of skeleton joints via ip.
 *
 * \author Kester Evers and Eric Jansen
 */
class MotionHub
{

public:
	MotionHub();
	TrackerManager* getTrackerManager();
	NetworkManager* getNetworkmanager();
	GestureManager* getGestureManager();
	std::string getStartTime();

private:
	std::string m_startTime;
	TrackerManager* m_trackerManager;
	ConfigReader* m_configReader;
	NetworkManager* m_networkManager;
	GestureManager* m_gestureManager;

};