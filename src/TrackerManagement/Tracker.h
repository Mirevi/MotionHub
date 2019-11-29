#pragma once

#include <iostream>
#include <thread>

//#include <Core>
//#include <Geometry>
//#include <Dense>

#include "MotionHubUtil/Skeleton.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/MMHmath.h"


/*!
 * \class Tracker
 *
 * \brief Prototype class for implemetation - holds virtual methods
 *
 * \author Kester Evers and Eric Jansen
 */
class  Tracker
{

public:

	/*!
	 * struct for containing tracker properties data  
	 */
	struct Properties
	{
		/*!
		 * tracker ID 
		 */
		int id = -1;
		/*!
		 * tracker name 
		 */
		std::string name = "none";
		/*!
		 * the trackers tracking state
		 */
		bool isTracking = false;
		/*!
		 * the trackers enabling state 
		 */
		bool isEnabled = false;
		/*!
		 * the number of skeletons detected by this tracker
		 */
		int countDetectedSkeleton = 0;

		Vector3f positionOffset;
		Vector3f rotationOffset;
		Vector3f scaleOffset;

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
	/*!
	 * disable tracker, so it doesn't track skeleton data during tracking loop
	 */
	virtual void disable();
	/*!
	 *  enables tracker, so it tracks skeleton data during tracking loop
	 */
	virtual void enable();
	/*!
	 * deletes all skeletons which are not in the scene anymore 
	 */
	virtual void clean();
	/*!
	 * checks if new skeleton date is available 
	 * \return true when new skeleton data is available
	 */
	virtual bool isDataAvailable();
	/*!
	 * resets the isDataAvailable to false
	 * call when you got the current skeleton data 
	 */
	virtual void resetIsDataAvailable();
	/*!
	 *  checks if skeleton count has changed
	 * \return true when number of skeletons is different from last frame
	 */
	virtual bool hasSkeletonPoolChanged();

	virtual void setSkeletonPoolChanged(bool state);

	/*!
	 * getter for the trackers properties struct 
	 * \return the trackers Property struct
	 */
	virtual Properties* getProperties();
	/*!
	 * getter for the trackers skeleton pool 
	 * \return the trackers skeleton pool
	 */
	virtual std::map<int, Skeleton*>* getSkeletonPool();



protected:

	/*!
	 * the trackers property struct 
	 */
	Properties* m_properties;
	/*!
	 * is true after one completed tracking cycle
	 */
	bool m_isDataAvailable = false;
	/*!
	 * is true if skeleton was added or removed from pool
	 */
	bool m_hasSkeletonPoolChanged = false;
	/*!
	 * thread for track() method
	 */
	std::thread* m_trackingThread;
	/*!
	 * pool containing all skeletons detected by this Tracker
	 */
	std::map<int, Skeleton*> m_skeletonPool;


	/*!
	 * base method for tracker initialisation 
	 */
	virtual void init() = 0;
	/*!
	 * updade method used for tracker thread 
	 */
	virtual void update() = 0;
	/*!
	 * main tracking method
	 * captures one frame of body tracking data and saves all data in the skeleton pool
	 */
	virtual void track() = 0;

	int m_trackingCycles = 0;

	Matrix4f m_offsetMatrix;

};