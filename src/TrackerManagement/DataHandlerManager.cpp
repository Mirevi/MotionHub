#include "DataHandlerManager.h"

//second declaration
Tracker::Properties* DataHandlerManager::m_properties;
sFrameOfMocapData* DataHandlerManager::m_data;
std::atomic<bool> DataHandlerManager::m_isDataAvailable;


DataHandlerManager::DataHandlerManager(Tracker::Properties* properties)
{

	m_properties = properties;
	m_isDataAvailable = false;

}

// receives data from the server
void DataHandlerManager::DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	//check if Tracker is active or if it hasn't fetched data yet
	if (!m_properties->isTracking || m_isDataAvailable.load())
	{

		return;
	}

	//check if data wasn't already assigned
	if (m_data != data)
	{

		m_data = data;

	}

	//assign new client data
	NatNetClient* pClient = (NatNetClient*)pUserData;
	   

	// timecode - for systems with an eSync and SMPTE timecode generator - decode to values
	int hour, minute, second, frame, subframe;
	bool bValid = pClient->DecodeTimecode(m_data->Timecode, m_data->TimecodeSubframe, &hour, &minute, &second, &frame, &subframe);
	// decode to friendly string
	char szTimecode[128] = "";
	pClient->TimecodeStringify(m_data->Timecode, m_data->TimecodeSubframe, szTimecode, 128);

	//new data is now available
	m_isDataAvailable.store(true);

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


sFrameOfMocapData* DataHandlerManager::getData()
{

	return m_data;

}