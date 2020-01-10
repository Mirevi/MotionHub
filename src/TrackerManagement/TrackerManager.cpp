#include "TrackerManager.h"

TrackerManager::TrackerManager()
{
	
	Console::log("TrackerManager::TrackerManager(): Created tracker manager.");

}

int TrackerManager::createTracker(TrackerType type)
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

			//lock the tracker pool
			m_isTrackerPoolLocked.store(true);

			// get next azure kinect camera id
			for (auto itPoolTracker = m_trackerPool.begin(); itPoolTracker != m_trackerPool.end(); itPoolTracker++)
			{

				if (itPoolTracker->first.first == "azureKinect")
				{

					//if there is already an AKTracker, increase the current cam id
					nextCamIdAk = itPoolTracker->first.second + 1;

				}

			}

			// create new azure kinect tracker and insert the tracker in the tracker pool
			m_trackerPool.insert({ { "azureKinect", id }, new AKTracker(id, nextCamIdAk) });

			//unlock the tracker pool
			m_isTrackerPoolLocked.store(false);

			//a tracker has been added, so the tracker pool has changed
			m_hasTrackerPoolChanged = true;

			Console::log("TrackerManager::createTracker(): Created Azure Kinect tracker with cam id = " + std::to_string(nextCamIdAk) + ".");

			return id;

		case optiTrack:

			//lock the tracker pool
			m_isTrackerPoolLocked.store(true);

			// create new azure kinect tracker and insert the tracker in the tracker pool
			m_trackerPool.insert({ { "optiTrack", id }, new OTTracker(id) });

			//unlock the tracker pool
			m_isTrackerPoolLocked.store(false);

			//a tracker has been added, so the tracker pool has changed
			m_hasTrackerPoolChanged = true;

			Console::log("TrackerManager::createTracker(): Created OptiTrack tracker.");

			return id;

		default:
			Console::log("TrackerManager::createTracker(): Can not create tracker. Unknown tracker type!");

			return -1;

	}

}

void TrackerManager::removeTracker(int idToRemove)
{

	Console::log("TrackerManager::removeTracker(): Removing tracker ...");

	//lock the tracker pool
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

			//a tracker has been removed, so the tracker pool has changed
			m_hasTrackerPoolChanged = true;

			Console::log("TrackerManager::removeTracker(): Removed tracker with id = " + std::to_string(idToRemove) + ".");

			break;

		}
	}

	//unlock the tracker pool
	m_isTrackerPoolLocked.store(false);

}

void TrackerManager::startTracker()
{

	Console::log("TrackerManager::startTracker(): Starting all tracker ...");

	//lock the tracker pool
	m_isTrackerPoolLocked.store(true);

	//loop through all tracker and start everyone of them
	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{

		itTracker->second->start();
	
	}

	//unlock the tracker pool
	m_isTrackerPoolLocked.store(false);

	//we are now in playMode
	m_isTracking = true;

	Console::log("TrackerManager::startTracker(): Started all tracker.");

}

void TrackerManager::stopTracker()
{

	Console::log("TrackerManager::stopTracker(): Stopping all tracker ...");

	//the playMode has ended
	m_isTracking = false;

	//lock the tracker pool
	m_isTrackerPoolLocked.store(true);

	//loop through all tracker and disable everyone of them
	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{

		itTracker->second->stop();

	}

	//unlock the tracker pool
	m_isTrackerPoolLocked.store(false);

	Console::log("TrackerManager::stopTracker(): Stopped all tracker.");

}

bool TrackerManager::hasTrackerPoolChanged()
{

	//m_hasTrackerPoolChanged is true, reset it to flase
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

bool TrackerManager::isTracking()
{

	return m_isTracking;

}

bool TrackerManager::isTrackerPoolLocked()
{

	return m_isTrackerPoolLocked.load();

}

std::map<std::pair<std::string, int>, Tracker*>* TrackerManager::getPoolTracker()
{

	return &m_trackerPool;

}

Tracker* TrackerManager::getTrackerRef(int id)
{

	//loop through tracker pool and return the tracker with given id
	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{

		if (itTracker->first.second == id)
		{

			return itTracker->second;

		}

	}

	return nullptr;

}