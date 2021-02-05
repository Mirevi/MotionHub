#pragma once


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
	 * \param tracker DataHandler needs to check for isTracking
	 */
	DataHandlerManager(Tracker* tracker);
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
	/*!
	 * getter for m_isDataAvailable
	 * 
	 * \return m_isDataAvailable
	 */
	bool isDataAvailable();


private:

	/*!
	 * properties container of OTTracker
	 * 
	 */
	static Tracker* m_tracker;
	/*!
	 * contains frame data containing current skeletons
	 * 
	 */
	static sFrameOfMocapData* m_data;
	/*!
	 * atomic bool for checking availability
	 * true when new data hasn't been fetched yet
	 */
	static std::atomic<bool> m_isDataAvailable;

};
