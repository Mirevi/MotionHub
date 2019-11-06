#include "Tracker.h"

Tracker::Tracker()
{

	m_properties = new Properties();

	init();

}

void Tracker::init()
{



}

void Tracker::start()
{

	m_properties->isTracking = true;

	m_trackingThread = new std::thread(&Tracker::track, this);
	m_trackingThread->detach();

}

void Tracker::track()
{

	while (m_properties->isTracking)
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

	m_properties->isTracking = false;

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

}

Tracker::Properties* Tracker::getProperties()
{

	return m_properties;

}