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

	trackingThread = new std::thread(&Tracker::track, this);
	trackingThread->detach();

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

}

void Tracker::destroy()
{

	delete this;

}

bool Tracker::isDataAvailable()
{

	return m_isDataAvailable;

}

std::map<int, Skeleton*>* Tracker::getSkeletonPool()
{

	if (m_isDataAvailable)
	{

		m_isDataAvailable = false;
		return &m_skeletonPool;

	}
	else
	{

		Console::logError("Tracker::getSkeletonPool(): No new data available!");
		return nullptr;

	}
}