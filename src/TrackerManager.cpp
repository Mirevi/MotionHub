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

	switch (type)
	{

		case AKT:
			poolTracker.insert({ "NULL", new AKTracker() });
			Console::log("Created tracker: AKTracker.");
			break;

		case XST:
			//poolTracker.insert({ "NULL", new XSTracker() });
			Console::log("Created tracker: XSTracker.");
			break;

		default:
			break;

	}
}