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
		case azureKinect:

			m_isTrackerPoolLocked.store(true);

			// get next azure kinect camera id
			for (auto itPoolTracker = m_trackerPool.begin(); itPoolTracker != m_trackerPool.end(); itPoolTracker++)
			{

				if (itPoolTracker->first.first == "azureKinect")
				{

					nextCamIdAk = itPoolTracker->first.second + 1;

				}
			}

			// create new azure kinect tracker and insert the tracker in the tracker pool
			m_trackerPool.insert({ { "azureKinect", id }, new AKTracker(id, nextCamIdAk) });

			m_isTrackerPoolLocked.store(false);

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

	m_isTrackerPoolLocked.store(true);

	// get key of tracker with id
	for (auto itPoolTracker = m_trackerPool.begin(); itPoolTracker != m_trackerPool.end(); itPoolTracker++)
	{

		if (itPoolTracker->first.second == idToRemove)
		{

			// destroy tracker with key
			m_trackerPool.at(itPoolTracker->first)->destroy();
			// remove tracker with key from tracker pool
			m_trackerPool.erase(itPoolTracker->first);

			m_hasTrackerPoolChanged = true;

			Console::log("TrackerManager::removeTracker(): Removed tracker with id = " + std::to_string(idToRemove) + ".");

			break;

		}
	}

	m_isTrackerPoolLocked.store(false);

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

	m_isTrackerPoolLocked.store(true);

	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{
		itTracker->second->start();
	}

	m_isTrackerPoolLocked.store(false);

	m_isTracking = true;

	Console::log("TrackerManager::startTracker(): Started all tracker.");
}

// stop all tracker in the tracker pool
void TrackerManager::stopTracker()
{

	Console::log("TrackerManager::stopTracker(): Stopping all tracker ...");

	m_isTracking = false;

	m_isTrackerPoolLocked.store(true);

	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{
		itTracker->second->stop();
	}

	m_isTrackerPoolLocked.store(false);

	Console::log("TrackerManager::stopTracker(): Stopped all tracker.");
}

// return refference pointer to the tracker poll
std::map<std::pair<std::string, int>, Tracker*>* TrackerManager::getPoolTracker()
{

	return &m_trackerPool;

}

bool TrackerManager::isTracking()
{

	return m_isTracking;

}

bool TrackerManager::isTrackerPoolLocked()
{
	return m_isTrackerPoolLocked.load();
}

Tracker* TrackerManager::getTrackerRef(int id)
{

	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{

		if (itTracker->first.second == id)
			return itTracker->second;

	}

	return nullptr;

}