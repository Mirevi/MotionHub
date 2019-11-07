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

	struct Properties
	{

		int id = -1;
		std::string name = "none";
		bool isTracking = false;
		bool isEnabled = false;
		int countDetectedSkeleton = 0;

	};

	/*!
	 * default constructor 
	 */
	Tracker();

	/*!
	 * starts Tracker in new temporary thread
	 */
	virtual void start() = 0;

	/*!
	 *  sets m_tracking to false
	 */
	virtual void stop() = 0;

	/*!
	 * resets the Trackers init data
	 */
	virtual void destroy() = 0;

	virtual std::map<int, Skeleton*>* getSkeletonPool();

	virtual bool isDataAvailable();
	virtual void resetIsDataAvailable();

	virtual bool hasSkeletonPoolChanged();

	virtual Properties* getProperties();

	virtual void disable();

	virtual void enable();

protected:

	Properties* m_properties;

	bool m_isDataAvailable = false;
	bool m_hasSkeletonPoolChanged = false;


	virtual void init() = 0;

	/*!
	 * thread for track() method
	 */
	std::thread* m_trackingThread;

	virtual void track() = 0;

	/*!
	 * main tracking method
	 * captures one frame of body tracking data and saves all data in the skeleton pool
	 */
	virtual void update() = 0;

	/*!
	 * pool containing all skeletons detected by this Tracker
	 */
	std::map<int, Skeleton*> m_skeletonPool;


};