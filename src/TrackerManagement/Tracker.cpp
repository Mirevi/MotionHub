#include "Tracker.h"

Tracker::Tracker()
{

	m_properties = new Properties();

	m_isCacheLocked.store(false);

	init();

}

#pragma region controls/util

void Tracker::start()
{

	//isTracking is true, so update loop will continue tracking
	m_properties->isTracking = true;

	//start new thread for update loop
	m_trackingThread = new std::thread(&Tracker::update, this);
	m_trackingThread->detach();

}

void Tracker::stop()
{

	//sets isTracking to false, so update loop retruns after current frame
	m_properties->isTracking = false;

}

void Tracker::destroy()
{

	delete this;

}

void Tracker::disable()
{

	//sets isTracking to false, so main loop doesn't check for new data
	m_properties->isEnabled = false;

	//pause this thread, so it doesn't skip cleaning 
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	//delete all skeletons from the pool
	clean();

	Console::log("Tracker::disable(): Disabled tracker with id = " + std::to_string(m_properties->id));

}

void Tracker::enable()
{

	//sets isTracking to true, so main loop checks for new data
	m_properties->isEnabled = true;

	Console::log("Tracker::enable(): Enabled tracker with id = " + std::to_string(m_properties->id));

}

void Tracker::clean()
{
	//reset number of skeletons
	m_properties->countDetectedSkeleton = 0;

	//delete all skeletons in pool
	for (auto itSkeleton = m_skeletonPool.begin(); itSkeleton != m_skeletonPool.end(); itSkeleton++)
	{

		delete itSkeleton->second;

	}

	//clear skeleton pool
	m_skeletonPool.clear();

}

void Tracker::cacheSkeletonData()
{

	m_isCacheLocked.store(true);

	m_skeletonPoolCache.clear();

	for (auto itSkeleton = m_skeletonPool.begin(); itSkeleton != m_skeletonPool.end(); itSkeleton++)
	{

		m_skeletonPoolCache.insert({ itSkeleton->first, *(itSkeleton->second) });

	}

	m_isCacheLocked.store(false);

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

Tracker::Properties* Tracker::getProperties()
{

	return m_properties;

}

std::map<int, Skeleton*>* Tracker::getSkeletonPool()
{

	return &m_skeletonPool;

}

std::map<int, Skeleton>* Tracker::getSkeletonPoolCache()
{

	while (m_isCacheLocked.load())
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	}

	return &m_skeletonPoolCache;

}


int Tracker::getCamID()
{

	return m_idCam;

}

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

Tracker* Tracker::getThis()
{

	return this;

}


#pragma endregion

#pragma region protected_methods

void Tracker::init()
{



}

void Tracker::update()
{

	while (m_properties->isTracking)
	{

		if (!m_isDataAvailable)
			track();

	}
}

void Tracker::track()
{

	m_isDataAvailable = true;

}

#pragma endregion



