#pragma once

//#include "NatNetClient.h"

#include "Tracker.h"
#include "MotionHubUtil/Skeleton.h"
#include "OTTracker.h"

#include <iostream>
#include <list>
#include <atomic>

/*!
 * \class DataHandlerManager
 *
 * \brief helper class for DataHandler method callback
 *
 * \author Kester Evers and Eric Jansen
 */
class DataHandlerManager
{


public:

	/*!
	 * constructor with properties for checking members
	 * 
	 * \param properties DataHandler needs to check for isTracking
	 */
	DataHandlerManager(Tracker::Properties* properties);

	/*!
	 * called by NatNet Client via callback
	 * receives Data from the server
	 * 
	 * \param data frame data containing current skeletons
	 * \param pUserData -
	 */
	void static DataHandler(sFrameOfMocapData* data, void* pUserData);

	/*!
	 * getter for frame data
	 * 
	 * \return private member m_data
	 */
	sFrameOfMocapData* getData();
	
	bool isDataAvailable();


private:

	/*!
	 * properties container of OTTracker
	 * 
	 */
	static Tracker::Properties* m_properties;

	/*!
	 * contains frame data containing current skeletons
	 * 
	 */
	static sFrameOfMocapData* m_data;
	static std::atomic<bool> m_isDataAvailable;


};