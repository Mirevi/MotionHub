#pragma once

#include <list>
#include "Tracker.h"
#include "NetworkManager.h"
#include "GestureManager.h"

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
class TrackerManager
{

public:

	/*!
	 * default constructor (empty)
	 */
	TrackerManager();
	/*!
	 * constructor with Network manager passing 
	 * \param networkmanager pointer to the Network manager
	 */
	TrackerManager(NetworkManager* networkManager, GestureManager* gestureManager);

	/*!
	 * starts the tracking loop
	 */
	void start();

	/*!
	* stops the tracking loop
	*/
	void stop();

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

private:

	/*!
	 * true while traking loop is running 
	 */
	bool m_tracking = false;

	/*!
	 * pool of all created tracker 
	 */
	std::map<std::pair<TypeTracker, int>, Tracker*> m_poolTracker;

	/*!
	 * pointer to the Network Manager for passing the skeleton data for transmission
	 */
	NetworkManager* m_networkManager;
	GestureManager* m_gestureManager;

	/*!
	 * calls all track() methods of the active tracker in the pool
	 */
	void update();

};