#include "TrackerManager.h"

TrackerManager::TrackerManager(NetworkManager* networkManager, ConfigManager* configManager)
{
	
	m_nextFreeTrackerID = 0;
	m_nextFreeAKCamID = 0;

	m_networkManager = networkManager;
	m_configManager = configManager;

	Console::log("TrackerManager::TrackerManager(): Created tracker manager.");

}

int TrackerManager::createTracker(TrackerType type, std::string filePath)
{

	// get the next tracker id
	int id = m_nextFreeTrackerID;

	//next tracker ID is increased with 1
	m_nextFreeTrackerID++;

	//create local Tracker*
	Tracker* tempTracker = nullptr;


	//lock the tracker pool
	m_trackerPoolLock.lock();


	// create new tracker based on the tracker type
	switch (type)
	{

		// azure kinect
		case azureKinect:
		{

			Console::log("TrackerManager::createTracker(): Creating AKtracker with cam ID = " + toString(m_nextFreeAKCamID) + " ...");

			//create new AK Tracker with next free Cam ID
			tempTracker = new AKTracker(id, m_nextFreeAKCamID, m_networkManager, m_configManager);


			if (!tempTracker->valid)
			{

				delete tempTracker;

				//unlock the tracker pool
				m_trackerPoolLock.unlock();

				return -1;

			}

			//sendSkeletonDelegate() funcPtr pass through
			//tempTracker->setSendSkeletonDelegate(m_sendSkeletonDelegate);

			//next AK Tracker has new cam ID
			m_nextFreeAKCamID++;

			break;

		}

		case optiTrack:
		{

			//create new Tracker with current ID
			tempTracker = new OTTracker(id, m_networkManager, m_configManager);

			break;

		}

		case bvh:
		{

			Console::log("TrackerManager::createTracker(): Creating BVH-Player ...");

			//create new BVH-Player with current ID
			tempTracker = new BVHPlayer(id, m_networkManager, m_configManager, filePath);

			break;

		}

		case group:
		{

			tempTracker = new TrackerGroup(id);

			break;

		}

		default:
		{

			Console::log("TrackerManager::createTracker(): Can not create tracker. Unknown tracker type!");

			//unlock the tracker pool
			m_trackerPoolLock.unlock();

			return -1;

		}
	}

	m_networkManager->createOSCSender(id);


	//insert the tracker in the tracker pool
	m_trackerPool.push_back(tempTracker);

	//unlock the tracker pool
	m_trackerPoolLock.unlock();

	//a tracker has been added, so the tracker pool has changed
	m_hasTrackerPoolChanged = true;

	Console::log("TrackerManager::createTracker(): Created tracker with ID = " + toString(id) + ".");

	return id;

}


void TrackerManager::removeTrackerAt(int positionInList)
{
	//lock the tracker pool
	m_trackerPoolLock.lock();

	//current Tracker to delete
	Tracker* currTracker;

	int currID;

	// find tracker with positionInList
	for(int i = 0; i < m_trackerPool.size(); i++)
	{

		if (i == positionInList)
		{
			//save the pointer to the tracker
			currTracker = m_trackerPool.at(i);

			//when tracker is AKtracker
			if (currTracker->getCamID() != -1)
			{
				//get the cam ID to reuse it
				m_nextFreeAKCamID = currTracker->getCamID();

			}

			// remove tracker with key from tracker pool
			m_trackerPool.erase(m_trackerPool.begin() + i);

			currID = currTracker->getProperties()->id;

			// destroy tracker with key
			currTracker->destroy();

			//a tracker has been removed, so the tracker pool has changed
			m_hasTrackerPoolChanged = true;

			Console::log("TrackerManager::removeTracker(): Removed tracker with id = " + std::to_string(i) + ".");

			break;
		}

	}

	//unlock the tracker pool
	m_trackerPoolLock.unlock();

	m_networkManager->removeNetworkSender(currID);

	return;

}

void TrackerManager::startTracker()
{

	Console::log("TrackerManager::startTracker(): Starting all tracker ...");

	//lock the tracker pool
	m_trackerPoolLock.lock();


	//loop through all tracker and start everyone of them
	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{

		(*itTracker)->start();
	
	}

	//unlock the tracker pool
	m_trackerPoolLock.unlock();

	//we are now in playMode
	m_isTracking = true;

	Console::log("TrackerManager::startTracker(): Started all tracker.");

}

void TrackerManager::stopTracker()
{

	//the playMode has ended
	m_isTracking = false;

	//lock the tracker pool
	m_trackerPoolLock.lock();

	//loop through all tracker and disable everyone of them
	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{

		(*itTracker)->stop();

	}

	//unlock the tracker pool
	m_trackerPoolLock.unlock();

	Console::log("TrackerManager::stopTracker(): Stopped all tracker.");

}

bool TrackerManager::hasTrackerPoolChanged()
{

	//m_hasTrackerPoolChanged is true, reset it to false
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

std::vector<Tracker*> TrackerManager::getPoolTracker()
{
	//lock the tracker pool to protect it from other methods to write on it during copying
	m_trackerPoolLock.lock();

	//make a local copy, so we can unlock the pool before return
	std::vector<Tracker*> trackerPoolCopyTemp = m_trackerPool;

	m_trackerPoolLock.unlock();

	return trackerPoolCopyTemp;

}

Tracker* TrackerManager::getTrackerRefAt(int trackerPositionInList )
{

	int i = 0;

	m_trackerPoolLock.lock();

	//loop through tracker pool and return the tracker with given id
	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{

		if (i == trackerPositionInList)
		{

			m_trackerPoolLock.unlock();

			return *itTracker;

		}

		i++;

	}

	m_trackerPoolLock.unlock();

	return nullptr;

}

std::mutex* TrackerManager::getTrackerPoolLock()
{
	return &m_trackerPoolLock;
}

//void TrackerManager::setSendSkeletonPtr(void (*sendSkeleton)(std::map<int, Skeleton>* skeletonPool, int trackerID))
//{
//
//	m_sendSkeletonDelegate = sendSkeleton;
//
//}
