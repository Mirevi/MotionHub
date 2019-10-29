#pragma once

#include <iostream>
#include <thread>

#include "MotionHubUtil/Skeleton.h"
#include "MotionHubUtil/Console.h"

/*!
 * \class Tracker
 *
 * \brief Prototype class for implemetation - holds virtual methods
 *
 * \author Kester Evers and Eric Jansen
 */
class Tracker
{

public:

	/*!
	 * default constructor 
	 */
	Tracker();
	/*!
	 * true when Tracker is active
	 */
	bool m_tracking = false;

	/*!
	 * starts Tracker in new temporary thread
	 */
	virtual void start();
	/*!
	 *  sets m_tracking to false
	 */
	virtual void stop();

	/*!
	 * main tracking method
	 * captures one frame of body tracking data and saves all data in the skeleton pool 
	 */
	virtual void track();
	/*!
	 * resets the Trackers init data
	 */
	virtual void shutdown();

	/*!
	 * pool containing all skeletons detected by this Tracker 
	 */
	std::map<int, Skeleton*> poolSkeletons;

private:
	/*!
	 * thread for track() method 
	 */
	std::thread* trackThread;

};