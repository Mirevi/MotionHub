#include "TrackerManager.h"

TrackerManager::TrackerManager()
{
	


}

// create new tracker based on the tracker type
void TrackerManager::createTracker(TrackerType type)
{

	Console::log("TrackerManager::createTracker(): Creating tracker ...");

	// get the next tracker id based on the tracker count
	int id = m_trackerPool.size();
	// var for azure kinect tracker id calculation
	int nextCamIdAk = 0;

	// create new tracker based on the tracker type
	switch (type)
	{

		// azure kinect
		case AKT:

			// get next azure kinect camera id
			for (auto itPoolTracker = m_trackerPool.begin(); itPoolTracker != m_trackerPool.end(); itPoolTracker++)
			{

				if (itPoolTracker->first.first == AKT)
				{

					nextCamIdAk = itPoolTracker->first.second + 1;

				}
			}			

			// create new azure kinect tracker and insert the tracker in the tracker pool
			m_trackerPool.insert({ { AKT, id }, new AKTracker(nextCamIdAk) });

			Console::log("TrackerManager::createTracker(): Created Azure Kinect tracker with cam id = " + std::to_string(nextCamIdAk) + ".");

			break;

		default:
			Console::log("TrackerManager::createTracker(): Can not create tracker. Unknown tracker type!");
			break;

	}
}

// remove tracker with id from the tracker pool
void TrackerManager::removeTracker(int idToRemove)
{

	Console::log("MotionHub::checkInput(): Removing tracker ...");

	// key for tracker identification
	std::pair<TrackerType, int> key;

	// get key of tracker with id
	for (auto itPoolTracker = m_trackerPool.begin(); itPoolTracker != m_trackerPool.end(); itPoolTracker++)
	{

		if (itPoolTracker->first.second == idToRemove)
		{

			key = itPoolTracker->first;

		}
	}

	// destroy tracker with key
	m_trackerPool.at(key)->destroy();
	// remove tracker with key from tracker pool
	m_trackerPool.erase(key);

	Console::log("TrackerManager::removeTracker(): Removed tracker with id = " + std::to_string(key.second) + ".");

}

// start all tracker in the tracker pool
void TrackerManager::startTracker()
{

	Console::log("TrackerManager::startTracker(): Starting all tracker ...");

	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{
		itTracker->second->start();
	}

	Console::log("TrackerManager::startTracker(): Started all tracker.");
}

// stop all tracker in the tracker pool
void TrackerManager::stopTracker()
{

	Console::log("TrackerManager::stopTracker(): Stopping all tracker ...");

	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{
		itTracker->second->stop();
	}

	Console::log("TrackerManager::stopTracker(): Stopped all tracker.");
}

// return refference pointer to the tracker poll
std::map<std::pair<TrackerManager::TrackerType, int>, Tracker*>* TrackerManager::getPoolTracker()
{

	return &m_trackerPool;

}