#include "TrackerManager.h"

TrackerManager::TrackerManager()
{
	
	Console::log("TrackerManager::TrackerManager(): Created tracker manager.");

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

			m_hasTrackerPoolChanged = true;

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

	// get key of tracker with id
	for (auto itPoolTracker = m_trackerPool.begin(); itPoolTracker != m_trackerPool.end(); itPoolTracker++)
	{

		if (itPoolTracker->first.second == idToRemove)
		{

			m_hasTrackerPoolChanged = true;

			// destroy tracker with key
			m_trackerPool.at(itPoolTracker->first)->destroy();
			// remove tracker with key from tracker pool
			m_trackerPool.erase(itPoolTracker->first);

			Console::log("TrackerManager::removeTracker(): Removed tracker with id = " + std::to_string(idToRemove) + ".");

			break;

		}
	}
}

bool TrackerManager::hasTrackerPoolChanged()
{

	if (m_hasTrackerPoolChanged)
	{

		m_hasTrackerPoolChanged = false;
		return true;

	}
	else
	{

		return false;

	}
}

// start all tracker in the tracker pool
void TrackerManager::startTracker()
{

	Console::log("TrackerManager::startTracker(): Starting all tracker ...");

	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{
		itTracker->second->start();
	}

	m_isTracking = true;

	Console::log("TrackerManager::startTracker(): Started all tracker.");
}

// stop all tracker in the tracker pool
void TrackerManager::stopTracker()
{

	Console::log("TrackerManager::stopTracker(): Stopping all tracker ...");

	m_isTracking = false;

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

bool TrackerManager::isTracking()
{

	return m_isTracking;

}