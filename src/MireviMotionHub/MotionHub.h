#pragma once





#include "defines.h"

#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/ConfigReader.h"

#include "TrackerManagement/TrackerManager.h"
#include "NetworkManagement/NetworkManager.h"
#include "GestureManagement/GestureManager.h"
#include "UIManagement/UIManager.h"
#include "MotionHubUtil/InputManager.h"

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
	/*!
	 * 
	 * default constructor
	 * inititalizes all member objects and starts threads
	 * 
	 * \param argc
	 * \param argv
	 */
	MotionHub(int argc, char** argv);

private:
	int m_argc;
	char** m_argv;

	ConfigReader* m_configReader;
	TrackerManager* m_trackerManager;
	GestureManager* m_gestureManager;
	NetworkManager* m_networkManager;
	UIManager* m_uiManager;
	InputManager* m_inputManager;

	//bool m_isTracking = false;

	void startUpdateThread();

	void update();
	std::thread* m_updateThread = nullptr;

	void processInput();

	//void startTracking();
	//void stopTracking();

};