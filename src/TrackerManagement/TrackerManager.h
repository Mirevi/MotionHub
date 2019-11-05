#pragma once

#include "ConfigDllExportTrackingManagement.h"

#include <list>
#include "Tracker.h"

#include <map>
#include <vector>

#include "AKTracker.h"
#include "MotionHubUtil/InputManager.h"

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
	TrackerManager(/*InputManager* inputManager*/);

	/*!
	 * enum for all tracker types 
	 */
	enum TrackerType 
	{
		AKT,	// Azure Kinect Tracker
		XST		// XSense Tracker
	};;

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

	std::map<std::pair<TrackerType, int>, Tracker*>* getPoolTracker();

private:

	/*!
	 * pool of all created tracker 
	 */
	std::map<std::pair<TrackerType, int>, Tracker*> m_trackerPool;

	//InputManager* m_refInputManager;

	bool m_isTracking = false;

	bool m_hasTrackerPoolChanged = false;

};