#pragma once

#include "defines.h"

#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/ConfigReader.h"

#include "TrackerManagement/TrackerManager.h"
#include "NetworkManagement/NetworkManager.h"
#include "GestureManagement/GestureManager.h"
#include "UIManagement/UIManager.h"
#include "UIManagement/InputManager.h"

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
	MotionHub(int argc, char** argv);
	TrackerManager* getTrackerManager();
	NetworkManager* getNetworkmanager();
	GestureManager* getGestureManager();
	UIManager* getUiManager();
	//InputManager* getInputManager();
	bool isTracking();

	void start();
	void stop();

private:
	ConfigReader* m_configReader;
	TrackerManager* m_trackerManager;
	GestureManager* m_gestureManager;
	NetworkManager* m_networkManager;
	UIManager* m_uiManager;
	//InputManager* m_inputManager;

	bool m_isTracking;
	void update();

};