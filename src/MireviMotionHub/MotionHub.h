#pragma once

#include "MotionHubUtil/ConfigManager.h"
#include "MotionHubUtil/Timer.h"

#include "TrackerManagement/TrackerManager.h"
#include "GestureManagement/GestureManager.h"
#include "RenderManagement/UIManager.h"


#include "NetworkManagement/OSCListener.h"

#include <chrono>
#include <string>

#include <shlobj.h>

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

	~MotionHub();

private:
	/*!
	 * argument c 
	 */
	int m_argc;
	/*!
	 * argment v 
	 */
	char** m_argv;

	/*!
	 *  pointer to the config reader
	 */
	ConfigManager* m_configManager;
	/*!
	 *  pointer to the tracker manager
	 */
	TrackerManager* m_trackerManager;
	/*!
	 * pointer to the gesture manager 
	 */
	GestureManager* m_gestureManager;
	/*!
	 * pointer to the network manager 
	 */
	NetworkManager* m_networkManager;
	/*!
	 * pointer to  the UI manager 
	 */
	UIManager* m_uiManager;


	/*!
	 * contains the main loop, loops as long as the UI Window exists 
	 */
	void update();

	void updateTimeline();

	void updateRecorderThread();

	void startListening();

	std::thread* m_recordingThread;
	std::thread* m_listeningThread;

	//OSCListener m_oscListener;

	void loadRecordPath();



};