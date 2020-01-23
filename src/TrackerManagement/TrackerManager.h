#pragma once

#include "ConfigDllExportTrackingManagement.h"

#include <map>
#include <vector>
#include <atomic>

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
		optiTrack

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
	 * getter for atomic m_isTrackerPoolLocked
	 * 
	 * \return 
	 */
	bool isTrackerPoolLocked();

	void setTrackerPoolLocked(bool state);


	/*!
	 * getter for m_trackerPool
	 * 
	 * \return pointer to map of tracker
	 */
	std::vector<Tracker*>* getPoolTracker();

	/*!
	 * getter for a specific tracker
	 * 
	 * \param id the id of the tracker which should be returned
	 * \return pointer to the tracker with given id
	 */
	Tracker* getTrackerRefAt(int id);

private:

	/*!
	 * pool of all created tracker 
	 */
	//std::map<std::pair<std::string, int>, Tracker*> m_trackerPool;
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

	/*!
	 * true when some thread is reading from the tracker pool
	 * 
	 */
	std::atomic<bool> m_isTrackerPoolLocked;

	int m_nextFreeTrackerID;

	int m_nextFreeAKCamID;

};