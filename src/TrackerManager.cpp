#include "TrackerManager.h"

TrackerManager::TrackerManager()
{
	


}

TrackerManager::TrackerManager(NetworkManager* networkManager, GestureManager* gestureManager)
{

	m_networkManager = networkManager;
	m_gestureManager = gestureManager;

}

void TrackerManager::start()
{

	m_tracking = true;
	update();

}

void TrackerManager::stop()
{

	m_tracking = false;

}

void TrackerManager::update()
{

	// tracking loop
	while (m_tracking)
	{

		for (auto itTracker = m_poolTracker.begin(); itTracker != m_poolTracker.end(); itTracker++)
		{

			if (itTracker->second->m_tracking)
			{

				itTracker->second->track();
				m_gestureManager->updateAllSkeletonPostures(&(itTracker->second->poolSkeletons));
				m_networkManager->sendSkeletonPool(&(itTracker->second->poolSkeletons));

			}							
			else
				itTracker->second->start();			

		}
	}

	/*
	// stop all tracker
	for (auto iteratorTracker = poolTracker.begin(); iteratorTracker != poolTracker.end(); iteratorTracker++)
	{

		iteratorTracker->second->stop();

	}
	*/
}

void TrackerManager::createTracker(TypeTracker type)
{

	// error handling in method -> work with pointers

	int idCam = 0;

	switch (type)
	{

		case AKT:

			for (auto itPoolTracker = m_poolTracker.begin(); itPoolTracker != m_poolTracker.end(); itPoolTracker++)
			{

				if (itPoolTracker->first.first == AKT)
				{

					idCam = itPoolTracker->first.second + 1;

				}
			}

			m_poolTracker.insert({ { AKT, idCam }, new AKTracker(K4A_DEVICE_CONFIG_INIT_DISABLE_ALL, idCam) });

			Console::log("TrackerManager::createTracker(): Created AKTracker with cam id = " + std::to_string(idCam) + ".");

			break;

		case XST:

			//poolTracker.insert({ "NULL", new XSTracker() });

			Console::log("Created tracker: XSTracker.");

			break;

		default:
			break;

	}
}