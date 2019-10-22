#include "MotionHub.h"

MotionHub::MotionHub()
{

	m_isTracking = false;

	Console::printHeader();

	m_configReader = new ConfigReader();
	m_configReader->readConfigFile(CONFIG_PATH);

	m_trackerManager = new TrackerManager();
	m_gestureManager = new GestureManager();
	m_networkManager = new NetworkManager();

	m_refPoolTracker = m_trackerManager->getPoolTracker();

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

void MotionHub::track()
{
	//global tracking loop

	while (m_isTracking)
	{
		
		m_trackerManager->update();

		for (auto itTracker = m_refPoolTracker->begin(); itTracker != m_refPoolTracker->end(); itTracker++)
		{
			
			m_gestureManager->updateAllSkeletonPostures(&(itTracker->second->poolSkeletons));
			m_networkManager->sendSkeletonPool(&(itTracker->second->poolSkeletons));

		}
	}
}

void MotionHub::startTracking()
{

	m_isTracking = true;
	track();

}

void MotionHub::stopTracking()
{

	m_isTracking = false;

}