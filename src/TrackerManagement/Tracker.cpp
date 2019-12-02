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

	m_trackingThread = new std::thread(&Tracker::update, this);
	m_trackingThread->detach();

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

	//if (m_isDataAvailable)
	//	Console::log("Tracker::isDataAvailable(): [" + m_properties->name + "] Tracking cycles = " + std::to_string(m_trackingCycles) + ".");

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

void Tracker::setSkeletonPoolChanged(bool state)
{

	m_hasSkeletonPoolChanged = state;

}

std::map<int, Skeleton*>* Tracker::getSkeletonPool()
{

	return &m_skeletonPool;

}

Tracker::Properties* Tracker::getProperties()
{

	return m_properties;

}

void Tracker::disable()
{

	m_properties->isEnabled = false;

	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	clean();

	Console::log("Tracker::disable(): Disabled tracker with id = " + std::to_string(m_properties->id));

}

void Tracker::enable()
{

	m_properties->isEnabled = true;

	Console::log("Tracker::enable(): Enabled tracker with id = " + std::to_string(m_properties->id));

}

void Tracker::clean()
{

	m_properties->countDetectedSkeleton = 0;
	m_skeletonPool.clear();

}

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