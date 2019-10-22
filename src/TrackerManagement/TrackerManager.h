#pragma once

#include "ConfigDllExportTrackingManagement.h"

#include <list>
#include "Tracker.h"

#include <map>
#include <vector>

#include "AKTracker.h"

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
	enum TypeTracker 
	{
		AKT,	// Azure Kinect Tracker
		XST		// XSense Tracker
	};;

	/*!
	 * creates a new tracker and adds it to the pool
	 * 
	 * \param type type of the tracker (Azure Kinect, XSense, ...)
	 */
	void createTracker(TypeTracker type);

	/*!
	* calls all track() methods of the active tracker in the pool
	*/
	void update();

	std::map<std::pair<TypeTracker, int>, Tracker*>* getPoolTracker();

private:

	/*!
	 * pool of all created tracker 
	 */
	std::map<std::pair<TypeTracker, int>, Tracker*> m_poolTracker;

};