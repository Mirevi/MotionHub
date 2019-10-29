#include "MotionHub.h"

MotionHub::MotionHub(int argc, char** argv)
{

	m_argc = argc;
	m_argv = argv;

	m_isTracking = false;

	Console::printHeader();

	m_configReader = new ConfigReader();
	m_configReader->readConfigFile(CONFIG_PATH);
	
	m_inputManager = new InputManager();

	m_trackerManager = new TrackerManager();
	m_gestureManager = new GestureManager();
	m_networkManager = new NetworkManager();

	m_threadInputLoop = new std::thread(&MotionHub::inputLoop, this);
	m_threadInputLoop->detach();

	m_uiManager = new UIManager(m_argc, m_argv, m_inputManager);

}

TrackerManager* MotionHub::getTrackerManager()
{ 

	return m_trackerManager; 

}

NetworkManager* MotionHub::getNetworkmanager()
{

	return m_networkManager;

}

GestureManager* MotionHub::getGestureManager()
{

	return m_gestureManager;

}


// motion hub loop
void MotionHub::update()
{

	while (m_isTracking)
	{	

		// tracker loop
		for (auto itTracker = m_trackerManager->getPoolTracker()->begin(); itTracker != m_trackerManager->getPoolTracker()->end(); itTracker++)
		{
			
			if (!itTracker->second->m_tracking)
			{

				m_gestureManager->updateAllSkeletonPostures(&(itTracker->second->poolSkeletons));
				m_networkManager->sendSkeletonPool(&(itTracker->second->poolSkeletons));
				itTracker->second->start();

			}
		}
	}
}

void MotionHub::start()
{

	if (!m_isTracking)
	{
		
		m_isTracking = true;

		m_threadInputLoop = new std::thread(&MotionHub::update, this);
		m_threadInputLoop->detach();

		Console::log("MotionHub::start(): Started tracking loop.");

	}
	else
		Console::logError("MotionHub::start(): Can not start tracking loop. Thread is already running!");

}

void MotionHub::stop()
{

	m_isTracking = false;

	Console::log("MotionHub::start(): Stopped tracking loop.");

}

UIManager* MotionHub::getUiManager()
{

	return m_uiManager;

}

InputManager* MotionHub::getInputManager()
{

	return m_inputManager;

}

bool MotionHub::isTracking()
{

	return m_isTracking;

}

void MotionHub::inputLoop()
{

	Console::log("MotionHub::inputLoop(): Started input loop.");

	// input loop
	while (true)
	{

		if (m_inputManager->isButtonPressed(0))
		{

			if (!m_isTracking)
				start();
			else
				stop();

		}

		if (m_inputManager->isButtonPressed(1))
		{

			bool wasTracking = m_isTracking;

			// stop tracking loop if running
			if (m_isTracking)
			{

				stop();

			}

			m_trackerManager->removeTracker(m_inputManager->getSelectedTrackerIdInList());

			// start tracking loop if it was running
			if (wasTracking)
			{

				start();

			}
		}

		if (m_inputManager->isButtonPressed(2))
		{
			switch (m_inputManager->getSelectedTrackerIdInDropdown())
			{
				case 0:
					m_trackerManager->createTracker(TrackerManager::AKT);
					break;

				default:
					Console::logError("MotionHub::inputLoop(): Tracker type not yet implemented.");
					break;

			}
		}
	}
}