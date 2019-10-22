#pragma once

#include "defines.h"

#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/ConfigReader.h"

#include "TrackerManagement/TrackerManager.h"
#include "NetworkManagement/NetworkManager.h"
#include "GestureManagement/GestureManager.h"

#include <string>

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

	void startTracking();
	void stopTracking();

private:
	TrackerManager* m_trackerManager;
	ConfigReader* m_configReader;
	NetworkManager* m_networkManager;
	GestureManager* m_gestureManager;

	std::map<std::pair<TrackerManager::TypeTracker, int>, Tracker*>* m_refPoolTracker;

	bool m_isTracking;

	void track();

};