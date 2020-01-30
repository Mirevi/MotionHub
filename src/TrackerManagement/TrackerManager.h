#pragma once

#include "ConfigDllExportTrackingManagement.h"

#include <map>
#include <vector>
#include <atomic>
#include <mutex>

#include "Tracker.h"
#include "AKTracker.h"
#include "OTTracker.h"

/*!
 * \class TrackerManager
 *
 * \brief Manager for all types of tracker
 *
 * \note while Tracking, it collects all data from the tracker pool
 * 
 * \author Kester Evers and Eric Jansen
 */
class TrackingManagement_DLL_import_export TrackerManager
{

public:

	/*!
	 * default constructor (empty)
	 */
	TrackerManager();

	/*!
	 * enum for all tracker types 
	 */
	enum TrackerType 
	{

		azureKinect,	// Azure Kinect Tracker
		optiTrack		//OptiTrack		Tracker

	};

	/*!
	 * creates a new tracker and adds it to the pool
	 * 
	 * \param type type of the tracker (Azure Kinect, XSense, ...)
	 */
	int createTracker(TrackerType type);
	/*!
	 * removes a tracker from the pool
	 * 
	 * \param idToRemove tracker id
	 */
	void removeTracker(int idToRemove);

	void removeTrackerAt(int positionInList);

	/*!
	 * starts all tracker in the tracker pool
	 * 
	 */
	void startTracker();

	/*!
	 * stops all tracker in the tracker pool
	 *
	 */
	void stopTracker();

	/*!
	 * getter for m_isTracking
	 * 
	 * \return 
	 */
	bool isTracking();

	/*!
	 * getter for m_hasTrackerPoolChanged
	 * also resets m_hasTrackerPoolChanged when it was true
	 * 
	 * \return 
	 */
	bool hasTrackerPoolChanged();


	/*!
	 * getter for m_trackerPool
	 * 
	 * \return pointer to map of tracker
	 */
	std::vector<Tracker*> getPoolTracker();

	/*!
	 * getter for a specific tracker
	 * 
	 * \param id the id of the tracker which should be returned
	 * \return pointer to the tracker with given id
	 */
	Tracker* getTrackerRefAt(int id);


	std::mutex* getTrackerPoolLock();

private:

	/*!
	 * pool of all created tracker 
	 */
	std::vector<Tracker*> m_trackerPool;

	/*!
	 * true when tracker is tracking, when MMH is in playMode
	 * 
	 */
	bool m_isTracking = false;

	/*!
	 * true when tracker pool has gained or lost a tracker
	 * 
	 */
	bool m_hasTrackerPoolChanged = false;


	int m_nextFreeTrackerID;

	int m_nextFreeAKCamID;

	std::mutex m_trackerPoolLock;

};