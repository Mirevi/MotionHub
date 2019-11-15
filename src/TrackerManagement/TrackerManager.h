#pragma once

#include "ConfigDllExportTrackingManagement.h"

#include <list>
#include "Tracker.h"

#include <map>
#include <vector>
#include <atomic>

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
	void createTracker(TrackerType type);
	/*!
	 * removes a tracker from the pool
	 * 
	 * \param idToRemove tracker id
	 */
	void removeTracker(int idToRemove);

	void startTracker();
	void stopTracker();

	bool isTracking();

	bool hasTrackerPoolChanged();

	bool isTrackerPoolLocked();

	std::map<std::pair<std::string, int>, Tracker*>* getPoolTracker();

	Tracker* getTrackerRef(int id);

private:

	/*!
	 * pool of all created tracker 
	 */
	std::map<std::pair<std::string, int>, Tracker*> m_trackerPool;

	bool m_isTracking = false;

	bool m_hasTrackerPoolChanged = false;

	std::atomic<bool> m_isTrackerPoolLocked;

};