#include "TrackerManager.h"

TrackerManager::TrackerManager()
{
	
	m_nextFreeTrackerID = 0;
	m_nextFreeAKCamID = 0;

	Console::log("TrackerManager::TrackerManager(): Created tracker manager.");

}

int TrackerManager::createTracker(TrackerType type)
{

	Console::log("TrackerManager::createTracker(): Creating tracker ...");

	// get the next tracker id based on the tracker count
	int id = m_nextFreeTrackerID;

	m_nextFreeTrackerID++;

	Tracker* tempTracker;

	// create new tracker based on the tracker type
	switch (type)
	{

		// azure kinect
		case azureKinect:


			//lock the tracker pool
			m_isTrackerPoolLocked.store(true);

			//create new AK Tracker with next free Cam ID
			tempTracker = new AKTracker(id, m_nextFreeAKCamID);

			//next AK Tracker has new cam ID
			m_nextFreeAKCamID++;

			// create new azure kinect tracker and insert the tracker in the tracker pool
			m_trackerPool.push_back(tempTracker);

			//unlock the tracker pool
			m_isTrackerPoolLocked.store(false);



			//a tracker has been added, so the tracker pool has changed
			m_hasTrackerPoolChanged = true;

			Console::log("TrackerManager::createTracker(): Created Azure Kinect tracker with cam id = " + std::to_string(m_nextFreeAKCamID - 1) + ".");

			return id;

		case optiTrack:



			//lock the tracker pool
			m_isTrackerPoolLocked.store(true);

			tempTracker = new OTTracker(id);

			// create new azure kinect tracker and insert the tracker in the tracker pool
			m_trackerPool.push_back(tempTracker);

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


void TrackerManager::removeTrackerAt(int positionInList)
{

	Console::log("TrackerManager::removeTracker(): Removing tracker ...");

	//lock the tracker pool
	m_isTrackerPoolLocked.store(true);

	//int i = 0;


	Tracker* temp;

	// get key of tracker with id
	//for (auto itPoolTracker = m_trackerPool.begin(); itPoolTracker != m_trackerPool.end(); itPoolTracker++)
	for(int i = 0; i < m_trackerPool.size(); i++)
	{

		if (i == positionInList)
		{
			temp = m_trackerPool.at(i);
			//temp = (*itPoolTracker);

			if (temp->getCamID() != -1)
			{

				m_nextFreeAKCamID = temp->getCamID();

			}

			// remove tracker with key from tracker pool
			m_trackerPool.erase(m_trackerPool.begin() + i);

			// destroy tracker with key
			temp->destroy();

			//a tracker has been removed, so the tracker pool has changed
			m_hasTrackerPoolChanged = true;

			Console::log("TrackerManager::removeTracker(): Removed tracker with id = " + std::to_string(i) + ".");

			//break;
			m_isTrackerPoolLocked.store(false);
			break;
		}

		//i++;

	}
	//unlock the tracker pool
	return;

}

void TrackerManager::startTracker()
{

	Console::log("TrackerManager::startTracker(): Starting all tracker ...");

	//lock the tracker pool
	m_isTrackerPoolLocked.store(true);

	//loop through all tracker and start everyone of them
	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{

		(*itTracker)->start();
	
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

		(*itTracker)->stop();

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

std::vector<Tracker*>* TrackerManager::getPoolTracker()
{

	return &m_trackerPool;

}

Tracker* TrackerManager::getTrackerRefAt(int trackerPositionInList )
{

	//Console::log("TrackerManager::getTrackerRef(): started getting Tracker. given id is: " + std::to_string(trackerPositionInList));

	int i = 0;

	//loop through tracker pool and return the tracker with given id
	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{

		//Console::log("TrackerManager::getTrackerRef(): current Tracker ID: " + std::to_string((*itTracker)->getProperties()->id) + ", current i: " + std::to_string(i));

		if (i == trackerPositionInList)
		{

			return *itTracker;

		}

		i++;

	}

	//Console::logError("TrackerManager::getTrackerRefAt(): Tracker does not exist!");

	return nullptr;

}