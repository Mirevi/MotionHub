#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

#include "MotionHubUtil/Skeleton.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/MMHmath.h"
#include "NetworkManagement/NetworkManager.h"
#include "Recorder.h"


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

		/*!
		 * offset of the trackers position
		 * 
		 */
		Vector3f positionOffset;

		/*!
		 * offset of the trackers rotation
		 *
		 */
		Vector3f rotationOffset;

		/*!
		 * offset of the trackers scale
		 *
		 */
		Vector3f scaleOffset;

	};

	/*!
	 * default constructor 
	 */
	Tracker();


	bool valid = true;

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

	/*!
	 * setter for m_hasSkeletonPoolChanged
	 * 
	 * \param state
	 */
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

	/*!
	 * getter for the trackers skeleton pool
	 * \return the trackers skeleton pool
	 */
	virtual std::map<int, Skeleton> getSkeletonPoolCache();

	/*!
	 * recalculates the update matrix
	 * 
	 */
	virtual void updateMatrix();

	/*!
	 * sets the position offset in the properties
	 * 
	 * \param position position offset
	 */
	virtual void setPositionOffset(Vector3f position);

	/*!
	 * sets the rotation offset in the properties
	 *
	 * \param position rotation offset
	 */
	virtual void setRotationOffset(Vector3f rotation);

	/*!
	 * sets the scale offset in the properties
	 *
	 * \param position scale offset
	 */
	virtual void setScaleOffset(Vector3f scale);

	/*!
	 * getter for the camera ID
	 * 
	 * \return m_idCam
	 */
	virtual int getCamID();

	/*!
	 * copys the skeleton pool to it's cache
	 * 
	 */
	virtual void cacheSkeletonData();

	/*!
	 * pointer to sendSkeletonDelegate() in main.cpp
	 *
	 */
	//virtual void setSendSkeletonDelegate(void (*sendSkeletonDelegate)(std::map<int, Skeleton>* skeletonPool, int trackerID));


	virtual std::string getTrackerType();

	virtual std::vector<Vector3f> resetOffsets();

	virtual int getCurrentFramePercent();


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
	std::map<int, Skeleton> m_skeletonPool;

	std::map<int, Skeleton> m_skeletonPoolCache;

	/*!
	 * id of the Azure Kinect Camera
	 * k4a SDK assigns the ids internally and automatically
	 * if only one camera is connected, this id should be 0
	 */
	int m_idCam = 0;


	/*!
	 * base method for tracker initialisation 
	 */
	virtual void init() = 0;

	/*!
	 * updade method used for tracker thread 
	 */
	virtual void update();

	/*!
	 * main tracking method
	 * captures one frame of body tracking data and saves all data in the skeleton pool
	 */
	virtual void track() = 0;


	/*!
	 * tracks the refresh cycles of a tracker
	 * 
	 */
	int m_trackingCycles = 0;

	/*!
	 * matrix calculated with the offset Vectors
	 * 
	 */
	Matrix4f m_offsetMatrix;

	/*!
	 * lock for save acces to skeleton pool
	 * 
	 */
	std::mutex m_skeletonPoolLock;


	NetworkManager* m_networkManager;

	ConfigManager* m_configManager;

};