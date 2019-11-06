#include "MotionHub.h"

MotionHub::MotionHub(int argc, char** argv)
{

	// save arguments
	m_argc = argc;
	m_argv = argv;

	// print header with information in console
	Console::printHeader();

	Console::log("MotionHub::MotionHub(): Starting Mirevi MotionHub ...");

	// create new config reader
	m_configReader = new ConfigReader();
	// load config file
	m_configReader->readConfigFile(CONFIG_PATH);
	
	// create manager
	m_trackerManager = new TrackerManager();
	m_gestureManager = new GestureManager();
	m_networkManager = new NetworkManager();

	// start main loop
	startUpdateThread();

	// main thread continues in m_uiManager QApplication::exec() method
	m_uiManager = new UIManager(m_argc, m_argv, m_trackerManager);

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

		// tracking loop
		if (m_trackerManager->isTracking())
		{

			// iterates threw all tracker located in the tracker manager tracker pool 
			for (auto itTracker = m_trackerManager->getPoolTracker()->begin(); itTracker != m_trackerManager->getPoolTracker()->end(); itTracker++)
			{

				// check if new skeleton data is available
				if (itTracker->second->getProperties()->isEnabled && itTracker->second->isDataAvailable())
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