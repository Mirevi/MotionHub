#include "TrackerManager.h"

TrackerManager::TrackerManager(NetworkManager* networkManager, ConfigManager* configManager)
{
	
	m_nextFreeTrackerID = 0;

	m_networkManager = networkManager;
	m_configManager = configManager;

	Console::log("TrackerManager::TrackerManager(): Created tracker manager");

}

TrackerManager::~TrackerManager()
{
	if (m_isTracking) stopTracker();
	while (m_trackerPool.size() > 0) removeTrackerAt(0);
}

int TrackerManager::createTracker(TrackerType type, std::string filePath)
{
	// get the next tracker id
	int id = m_nextFreeTrackerID;

	//create local Tracker*
	Tracker* tempTracker = nullptr;

	//lock the tracker pool
	m_trackerPoolLock.lock();

	try {
		// Call Tracker Factory
		tempTracker = instantiateTracker(type, id, filePath);
	}
	catch (const Exception& exception) {
		//unlock the tracker pool
		m_trackerPoolLock.unlock();

		// throw Exception for UI Objects 
		throw exception;
	}

	//next tracker ID is increased with 1
	m_nextFreeTrackerID++;

	//create OSC Sender for tracker
	m_networkManager->createOSCSender(id);

	//insert the tracker in the tracker pool
	m_trackerPool.push_back(tempTracker);

	//unlock the tracker pool
	m_trackerPoolLock.unlock();

	//a tracker has been added, so the tracker pool has changed
	m_hasTrackerPoolChanged = true;

	Console::log("TrackerManager::createTracker(): Created tracker with ID = " + toString(id));

	return id;
}


void TrackerManager::removeTrackerAt(int positionInList)
{
	//lock the tracker pool
	m_trackerPoolLock.lock();

	if (positionInList < m_trackerPool.size()) {
		//save the pointer to the tracker
		Tracker* currTracker = m_trackerPool.at(positionInList);

		// remove tracker with key from tracker pool
		m_trackerPool.erase(m_trackerPool.begin() + positionInList);

		int currID = currTracker->getProperties()->id;

		// destroy tracker with key
		delete currTracker;

		//a tracker has been removed, so the tracker pool has changed
		m_hasTrackerPoolChanged = true;

		Console::log("TrackerManager::removeTracker(): Removed tracker with id = " + std::to_string(positionInList) + ".");

		m_networkManager->removeNetworkSender(currID);
	}
	//unlock the tracker pool
	m_trackerPoolLock.unlock();
}

void TrackerManager::startTracker()
{

	Console::log("TrackerManager::startTracker(): Starting all trackers ...");

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

	Console::log("TrackerManager::startTracker(): Started all trackers.");

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

	Console::log("TrackerManager::stopTracker(): Stopped all tracker");

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

Tracker* TrackerManager::getTrackerByType(TrackerType type) {

	m_trackerPoolLock.lock();

	//loop through tracker pool and return the tracker with given type
	for (Tracker* tracker : m_trackerPool) {

		if (tracker->getType() == type) {

			m_trackerPoolLock.unlock();

			return tracker;
		}
	}

	m_trackerPoolLock.unlock();

	return nullptr;
}

std::mutex* TrackerManager::getTrackerPoolLock()
{
	return &m_trackerPoolLock;
}

void TrackerManager::controlTimeline(bool stop)
{

	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{
		if ((*itTracker)->getTrackerType() == "BVH")
		{
			dynamic_cast<BVHPlayer*>(*itTracker)->controlTime(stop);
		}
		else if((*itTracker)->getTrackerType() == "MMH")
		{
			dynamic_cast<mmhPlayer*>(*itTracker)->controlTime(stop);
		}
	}

}

void TrackerManager::timelineValueChange(int newValue)
{
	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{
		if ((*itTracker)->getTrackerType() == "BVH")
		{
			dynamic_cast<BVHPlayer*>(*itTracker)->setCurrentFrame(newValue);
		}
		else if ((*itTracker)->getTrackerType() == "MMH")
		{
			dynamic_cast<mmhPlayer*>(*itTracker)->setCurrentFrame(newValue);
		}
	}
}


//void TrackerManager::setSendSkeletonPtr(void (*sendSkeleton)(std::map<int, Skeleton>* skeletonPool, int trackerID))
//{
//
//	m_sendSkeletonDelegate = sendSkeleton;
//
//}

FrameData TrackerManager::getRecCurrFrameData()
{

	FrameData currFrameData;



	int currFrameIdx = -1;
	float totalTime = -1.0;
	int frameCount = -1;


	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{
		
		if ((*itTracker)->getTrackerType() == "BVH")
		{

			BVHPlayer* currBvhPlayer = dynamic_cast<BVHPlayer*>(*itTracker);

			totalTime = currBvhPlayer->getTotalTime();
			currFrameIdx = currBvhPlayer->getCurrFrameIdx();
			frameCount = currBvhPlayer->getFrameCount();

			break;
		}
		else if ((*itTracker)->getTrackerType() == "MMH")
		{

			mmhPlayer* currMmhPlayer = dynamic_cast<mmhPlayer*>(*itTracker);

			totalTime = currMmhPlayer->getTotalTime();
			currFrameIdx = currMmhPlayer->getCurrFrameIdx();
			frameCount = currMmhPlayer->getFrameCount();

			break;
		}

	}

	//Console::log("TrackerManager::getRecCurrFrameData(): currFrameIdx = " + toString(currFrameIdx));



	currFrameData.currFrameIdx = currFrameIdx;
	currFrameData.frameCount = frameCount;
	currFrameData.totalTime = totalTime;

	return currFrameData;


}

void TrackerManager::writeSkeletonsToRecorder()
{
	//record all active tracker

	//loop over all tracker
	for (auto itTracker = m_trackerPool.begin(); itTracker != m_trackerPool.end(); itTracker++)
	{

		//check if Tracker is enabled
		if ((*itTracker)->isEnabled())
		{
			
			//add skeletons to Recording Frame, use cache because thread issues
			Recorder::instance().addSkeletonsToFrame((*itTracker)->getSkeletonPoolCache());



		}


	}

	Recorder::instance().nextFrame();

}

Tracker* TrackerManager::instantiateTracker(TrackerType type, int id, std::string filePath) {

	Tracker* tempTracker = nullptr;

	// create new tracker based on the tracker type
	switch (type)
	{

	case azureKinect:
	{
		Console::log("TrackerManager::createTracker(): Creating Azure Kinect tracker");

		try {
			//create new AK Tracker with next free Cam ID
			tempTracker = new AKTracker(id, m_networkManager, m_configManager);
		}
		catch (const Exception& exception) {
			delete tempTracker;

			throw exception;
		}

		//sendSkeletonDelegate() funcPtr pass through
		//tempTracker->setSendSkeletonDelegate(m_sendSkeletonDelegate);

		break;
	}

	case optiTrack:
	{
		Console::log("TrackerManager::createTracker(): Creating OptiTrack tracker");

		//create new Tracker with current ID
		tempTracker = new OTTracker(id, m_networkManager, m_configManager);

		break;
	}

	case bvh:
	{
		Console::log("TrackerManager::createTracker(): Creating BVH-Player");

		//create new BVH-Player with current ID
		tempTracker = new BVHPlayer(id, m_networkManager, m_configManager, filePath);

		break;
	}

	case CapturyLive:
	{
		//create new Tracker with current ID
		tempTracker = new CLTracker(id, m_networkManager, m_configManager);

		break;
	}

	case mmh:
	{
		Console::log("TrackerManager::createTracker(): Creating mmh-Player");

		//create new BVH-Player with current ID
		tempTracker = new mmhPlayer(id, m_networkManager, m_configManager, filePath);

		break;
	}

	case xSens:
	{
		Console::log("TrackerManager::createTracker(): Creating xSens tracker");

		//create new BVH-Player with current ID
		tempTracker = new XSTracker(id, m_networkManager, m_configManager);

		break;
	}

	case openVR:
	{
		Console::log("TrackerManager::createTracker(): Creating OpenVR tracker");

		try {
			tempTracker = new OVRTracker(id, m_networkManager, m_configManager);
		}
		catch (const Exception& exception) {
			delete tempTracker;

			throw exception;
		}

		break;
	}

	case group:
	{
		tempTracker = new TrackerGroup(id);

		break;
	}

	default:
	{
		Console::log("TrackerManager::createTracker(): Can not create tracker. Unknown tracker type");

		throw Exception("Unknown tracker type");
	}
	}

	// Set Tracker Type
	if (tempTracker != nullptr) {
		tempTracker->setType(type);
	}

	return tempTracker;
}
