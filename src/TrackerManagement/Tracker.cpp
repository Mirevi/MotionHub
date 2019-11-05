#include "Tracker.h"

Tracker::Tracker()
{

	init();

}

void Tracker::init()
{



}

void Tracker::start()
{

	m_isTracking = true;

	m_trackingThread = new std::thread(&Tracker::track, this);
	m_trackingThread->detach();

}

void Tracker::track()
{

	while (m_isTracking)
	{

		if (!m_isDataAvailable)
			update();

	}
}

void Tracker::update()
{

	m_isDataAvailable = true;

}

void Tracker::stop()
{

	m_isTracking = false;
	//m_trackingThread->join();

}

void Tracker::destroy()
{

	delete this;

}

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

std::map<int, Skeleton*>* Tracker::getSkeletonPool()
{

	return &m_skeletonPool;

	//if (m_isDataAvailable)
	//	return &m_skeletonPool;
	//else
	//{

	//	Console::logError("Tracker::getSkeletonPool(): No new data available!");
	//	return nullptr;

	//}
}