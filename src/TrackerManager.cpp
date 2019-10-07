#include "TrackerManager.h"

TrackerManager::TrackerManager()
{
	


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

		for (auto iteratorTracker = poolTracker.begin(); iteratorTracker != poolTracker.end(); iteratorTracker++)
		{

			if(iteratorTracker->second->m_tracking)
				iteratorTracker->second->track();
			else
				iteratorTracker->second->start();

		}
	}

	// stop all tracker
	for (auto iteratorTracker = poolTracker.begin(); iteratorTracker != poolTracker.end(); iteratorTracker++)
	{

		iteratorTracker->second->stop();

	}
}

void TrackerManager::createTracker(TypeTracker type)
{

	// error handling in method -> work with pointers

	int idCam = 0;

	switch (type)
	{

		case AKT:

			for (auto itPoolTracker = poolTracker.begin(); itPoolTracker != poolTracker.end(); itPoolTracker++)
			{

				if (itPoolTracker->first.first == AKT)
				{

					idCam = itPoolTracker->first.second + 1;

				}
			}

			poolTracker.insert({ { AKT, idCam }, new AKTracker(K4A_DEVICE_CONFIG_INIT_DISABLE_ALL, idCam) });

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