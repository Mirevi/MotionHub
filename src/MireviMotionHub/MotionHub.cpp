#include "MotionHub.h"

MotionHub::MotionHub(int argc, char** argv)
{

	Console::log("MotionHub::MotionHub(): Starting Mirevi MotionHub ...");

	// save arguments
	m_argc = argc;
	m_argv = argv;

	// print header with information in console
	Console::printHeader();

	// create new config reader
	m_configReader = new ConfigReader();
	// load config file
	m_configReader->readConfigFile(CONFIG_PATH);
	
	// create manager
	m_inputManager = new InputManager();
	m_trackerManager = new TrackerManager(m_inputManager);
	m_gestureManager = new GestureManager();
	m_networkManager = new NetworkManager();

	// start main loop
	startUpdateThread();

	// main thread continues in m_uiManager QApplication::exec() method
	m_uiManager = new UIManager(m_argc, m_argv, m_inputManager);



}

// start thread for main loop
void MotionHub::startUpdateThread()
{

	// check if update thread exists
	if(m_updateThread == nullptr)
	{

		// create and start new thread if none exists
		m_updateThread = new std::thread(&MotionHub::update, this);
		// detach thread from method scope runtime
		m_updateThread->detach();

		Console::log("MotionHub::startUpdateThread(): Started update thread.");

	}
	else
		Console::logError("MotionHub::startUpdateThread(): Can not start update thread. Thread is already running!");

}

// motion hub main loop
void MotionHub::update()
{

	// main loop
	while (true)
	{

		// process ui input
		processInput();

		// tracking loop
		if (m_isTracking)
		{

			// iterates threw all tracker located in the tracker manager tracker pool 
			for (auto itTracker = m_trackerManager->getPoolTracker()->begin(); itTracker != m_trackerManager->getPoolTracker()->end(); itTracker++)
			{

				// check if new skeleton data is available
				if (itTracker->second->isDataAvailable())
				{

					// send skeleton pool reference to gesture manager in order to update all postures
					m_gestureManager->updateAllSkeletonPostures(itTracker->second->getSkeletonPool());
					// send skeleton pool 
					m_networkManager->sendSkeletonPool(itTracker->second->getSkeletonPool());

					// reset bool and start new tracking cycle
					itTracker->second->resetIsDataAvailable();

				}
			}
		}
	}
}

// process input based on input manager button handling
void MotionHub::processInput()
{

	// toggle tracking
	// check if start / stop tracking button is pressed
	if (m_inputManager->isButtonPressed(0))
	{

		// check if motion hub is tracking
		if (!m_isTracking)
			startTracking(); // start tracking if false
		else
			stopTracking(); // stop tracking hub is true

	}

	// check if remove tracker button is pressed
	if (m_inputManager->isButtonPressed(1))
	{

		// save current tracking state
		bool wasTracking = m_isTracking;

		// check if motion hub is tracking
		if (m_isTracking)
			stopTracking(); // stop tracking if true

		// get selected tracker id in ui list
		// remove selected tracker from tracker pool
		m_trackerManager->removeTracker(m_inputManager->getSelectedTrackerIdInList());

		// check if motion hub was tracking
		if (wasTracking)
			startTracking(); // start tracking if true

	}

	// check if add tracker button in dialog is pressed
	if (m_inputManager->isButtonPressed(2))
	{

		// get selected tracker id
		switch (m_inputManager->getSelectedTrackerIdInDropdown())
		{

			case 0:
				m_trackerManager->createTracker(TrackerManager::AKT); // create new azure kinect tracker and add tracker to the tracking manager tracker pool
				break;

			default:
				Console::logError("MotionHub::checkInput(): Can not create tracker. Tracker type unkown!");
				break;

		}
	}
}

// starts tracking
void MotionHub::startTracking()
{

	Console::log("MotionHub::startTracking(): Starting tracking ...");

	// start the tracking thread for each tracker in the tracker manager tracker pool
	m_trackerManager->startTracker();
	// set tracking to true in order to execute the tracking loop in main loop / update() method
	m_isTracking = true;

	Console::log("MotionHub::startTracking(): Started tracking loop.");

}

// stops tracking
void MotionHub::stopTracking()
{

	Console::log("MotionHub::stopTracking(): Stopping tracking ...");

	// stop the tracking thread for each tracker in the tracker manager tracker pool
	m_trackerManager->stopTracker();
	// set tracking to false in order to stop the tracking loop in main loop / update() method
	m_isTracking = false;

	Console::log("MotionHub::stopTracking(): Stopped tracking loop.");

}