#include "MotionHub.h"

MotionHub::MotionHub(int argc, char** argv)
{

	// save arguments
	m_argc = argc;

	

	m_argv = argv;



	Console::log("MotionHub::MotionHub(): Starting Mirevi MotionHub ...");

	// create new config reader
	m_configReader = new ConfigReader();
	// load config file
	m_configReader->readConfigFile(CONFIG_PATH);
	
	// create manager
	m_gestureManager = new GestureManager();
	m_networkManager = new NetworkManager();
	m_trackerManager = new TrackerManager(m_networkManager);
	m_uiManager = new UIManager(m_argc, m_argv, m_trackerManager);


	// start update loop
	update();

}

// motion hub main loop
void MotionHub::update()
{


	int framesElapsed = 0;

	// main loop
	while (m_uiManager->getMainWindow()->isVisible())
	{

		// process ui input
		m_uiManager->processInput();
		m_uiManager->getMainWindow()->updateConsole();

		// send skeleton pools to other managers
		if (m_trackerManager->isTracking())
		{

			std::vector<Tracker*> trackerPoolTempCopy = m_trackerManager->getPoolTracker();

			// iterates threw all tracker located in the tracker manager tracker pool 
			for (auto itTracker = trackerPoolTempCopy.begin(); itTracker != trackerPoolTempCopy.end(); itTracker++)
			{

				// check if new skeleton data is available
				if ((*itTracker)->getProperties()->isEnabled && (*itTracker)->isDataAvailable())
				{

					//cache skeleton data of current tracker, so gesture- and network manager use current data
					(*itTracker)->cacheSkeletonData();

					// send skeleton pool reference to gesture manager in order to update all postures
					//m_gestureManager->updateAllSkeletonPostures(&((*itTracker)->getSkeletonPoolCache())									  );
					// send skeleton pool 
					//m_networkManager->sendSkeletonPool(			&((*itTracker)->getSkeletonPoolCache()), (*itTracker)->getProperties()->id);



					// update ui if skeleton was added or removed from pool
					if ((*itTracker)->hasSkeletonPoolChanged())
					{
						//update UI
						m_uiManager->getMainWindow()->update();	

						m_uiManager->getMainWindow()->getOglRenderer()->updateSkeletonMeshCount();

					}

					m_uiManager->getMainWindow()->getOglRenderer()->updateSkeletonMeshTransform();

					// reset bool and start new tracking cycle
					(*itTracker)->resetIsDataAvailable();

				}

			}
		}	//check if tracker is added or removed
		else if(m_trackerManager->hasTrackerPoolChanged())
		{

			//update UI
			m_uiManager->getMainWindow()->update();

			m_uiManager->getMainWindow()->getOglRenderer()->updateSkeletonMeshPoolSize();

		}

	}

}