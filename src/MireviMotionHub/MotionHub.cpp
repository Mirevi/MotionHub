#include "MotionHub.h"

MotionHub::MotionHub(int argc, char** argv)
{

	m_isTracking = false;

	Console::printHeader();

	m_configReader = new ConfigReader();
	m_configReader->readConfigFile(CONFIG_PATH);
	
	//m_inputManager = new InputManager();

	m_uiManager = new UIManager(argc, argv/*, m_inputManager*/);

	m_trackerManager = new TrackerManager();
	m_gestureManager = new GestureManager();
	m_networkManager = new NetworkManager();

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

	m_isTracking = true;

	update();

}

void MotionHub::stop()
{

	m_isTracking = false;

}

UIManager* MotionHub::getUiManager()
{

	return m_uiManager;

}

//InputManager* MotionHub::getInputManager()
//{
//
//	return m_inputManager;
//
//}

bool MotionHub::isTracking()
{

	return m_isTracking;

}