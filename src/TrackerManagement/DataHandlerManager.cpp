#include "DataHandlerManager.h"


Tracker::Properties* DataHandlerManager::m_properties;

sFrameOfMocapData* DataHandlerManager::m_data;


DataHandlerManager::DataHandlerManager(Tracker::Properties* properties)
{

	m_properties = properties;



}

// receives data from the server
void DataHandlerManager::DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	

	if (!m_properties->isTracking)
	{

		return;
	}


	if (m_data != data)
	{

		m_data = data;

		Console::log("DataHandlerManager::DataHandler(): m_data = data");


	}



	NatNetClient* pClient = (NatNetClient*)pUserData;


	bool bIsRecording = ((m_data->params & 0x01) != 0);
	bool bTrackedModelsChanged = ((m_data->params & 0x02) != 0);
	if (bIsRecording)
		printf("RECORDING\n");
	if (bTrackedModelsChanged)
		printf("Models Changed.\n");
	// timecode - for systems with an eSync and SMPTE timecode generator - decode to values
	int hour, minute, second, frame, subframe;
	bool bValid = pClient->DecodeTimecode(m_data->Timecode, m_data->TimecodeSubframe,
		&hour, &minute, &second, &frame, &subframe);
	// decode to friendly string
	char szTimecode[128] = "";
	pClient->TimecodeStringify(m_data->Timecode, m_data->TimecodeSubframe, szTimecode, 128);

}


sFrameOfMocapData* DataHandlerManager::getData()
{

	Console::log("DataHandlerManager::getData(): get data");

	return m_data;

}