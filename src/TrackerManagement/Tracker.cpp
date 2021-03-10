#include "Tracker.h"

Tracker::Tracker()
{

	m_properties = new Properties();


	init();

}

#pragma region controls/util

void Tracker::start()
{
	//isTracking is true, so update loop will continue tracking
	m_isTracking = true;

	//start new thread for update loop
	m_trackingThread = new std::thread(&Tracker::update, this);
}

void Tracker::stop()
{
	//sets isTracking to false, so update loop retruns after current frame
	m_isTracking = false;

	//wait for tracking thread to terminate, then dispose of thread object
	if (m_trackingThread->joinable()) m_trackingThread->join();
	delete m_trackingThread;
}

void Tracker::destroy()
{

	delete this;

}

void Tracker::disable()
{
	//sets isEnabled to false, so the trackingThread doesn't check for new data
	m_isEnabled = false;

	Console::log("Tracker::disable(): Disabled tracker with id = " + std::to_string(m_properties->id));
}

void Tracker::enable()
{
	//sets isEnabled to true, so the trackingThread checks for new data
	m_isEnabled = true;

	Console::log("Tracker::enable(): Enabled tracker with id = " + std::to_string(m_properties->id));
}

void Tracker::clean()
{
	//reset number of skeletons
	m_countDetectedSkeleton = 0;

	//clear skeleton pool
	m_skeletonPool.clear();

}

bool Tracker::isTracking()
{
	return m_isTracking;
}

bool Tracker::isEnabled()
{
	return m_isEnabled;
}

void Tracker::cacheSkeletonData()
{

	//lock the skeleton pool and copy it to the cache
	m_skeletonPoolLock.lock();

	m_skeletonPoolCache = m_skeletonPool;

	m_skeletonPoolLock.unlock();

}

std::string Tracker::getTrackerType()
{

	return "";

}

#pragma endregion

#pragma region getter/setter

bool Tracker::isDataAvailable()
{

	return m_isDataAvailable;

}

void Tracker::resetIsDataAvailable()
{

	m_isDataAvailable = false;

}

bool Tracker::hasSkeletonPoolChanged()
{

	//resets when it's true
	if (m_hasSkeletonPoolChanged)
	{

		m_hasSkeletonPoolChanged = false;
		return true;

	}
	else
	{

		return false;

	}
}

void Tracker::setSkeletonPoolChanged(bool state)
{

	m_hasSkeletonPoolChanged = state;

}

int Tracker::getNumDetectedSkeletons()
{
	return m_countDetectedSkeleton;
}

Tracker::Properties* Tracker::getProperties()
{

	return m_properties;

}

std::map<int, Skeleton> Tracker::getSkeletonPoolCache()
{

	//lock skeleton pool for the case, that cacheSkeletonData() is called while this method reads from the cache
	m_skeletonPoolLock.lock();

	//copy cache to local copy, so we can unlock the skeleton pool befor return
	std::map<int, Skeleton> skeletonPoolCacheCopy = m_skeletonPoolCache;

	m_skeletonPoolLock.unlock();



	return skeletonPoolCacheCopy;

}

int Tracker::getCamID()
{

	return m_idCam;

}
 
//void Tracker::setSendSkeletonDelegate(void (*sendSkeletonDelegate)(std::map<int, Skeleton>* skeletonPool, int trackerID))
//{
//
//	m_sendSkeletonDelegate = sendSkeletonDelegate;
//
//}




#pragma endregion

#pragma region tracker_Offset_handling

void Tracker::updateMatrix()
{

	//create new Matrix and set it to be identity
	m_offsetMatrix = transformMatrix(m_properties->positionOffset, m_properties->rotationOffset, m_properties->scaleOffset);

}

void Tracker::setPositionOffset(Vector3f position)
{

	m_properties->positionOffset = position;

	updateMatrix();

}

void Tracker::setRotationOffset(Vector3f rotation)
{

	m_properties->rotationOffset = rotation;

	updateMatrix();

}

void Tracker::setScaleOffset(Vector3f scale)
{

	m_properties->scaleOffset = scale;

	updateMatrix();

}


#pragma endregion

#pragma region protected_methods

void Tracker::init()
{

	Console::log("Tracker::init()");


}

void Tracker::update()
{
	// track while tracking is true
	while (m_isTracking)
	{
		//Console::log("Tracker::update()");
		// if no new data is procressed
		if (!m_isDataAvailable && m_isEnabled) {
			// get new data
			track();

			// send Skeleton Pool to NetworkManager
			if (m_networkManager != nullptr) m_networkManager->sendSkeletonPool(&m_skeletonPool, m_properties->id);
		}
	}
	//clean skeleton pool after tracking
	clean();
}

void Tracker::track()
{
	Console::log("Tracker::track()");

	m_isDataAvailable = true;
}

#pragma endregion
