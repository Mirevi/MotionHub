#include "DataHandlerManager.h"

//redeclaration
Tracker* DataHandlerManager::m_tracker;
sFrameOfMocapData* DataHandlerManager::m_data;
std::atomic<bool> DataHandlerManager::m_isDataAvailable;


DataHandlerManager::DataHandlerManager(Tracker* tracker)
{

	m_tracker = tracker;
	m_isDataAvailable = false;

}

void DataHandlerManager::DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	//check if Tracker is active or if it hasn't fetched data yet
	if (!m_tracker->isTracking() || m_isDataAvailable.load())
	{

		return;

	}

	//store data in this object
	m_data = data;

	//new data is now available
	m_isDataAvailable.store(true);

}

sFrameOfMocapData* DataHandlerManager::getData()
{

	return m_data;

}

bool DataHandlerManager::isDataAvailable()
{

	//when data is available, reset
	if (m_isDataAvailable.load())
	{

		m_isDataAvailable.store(false);

		return true;

	}

	return false;
}


