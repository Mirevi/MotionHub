#pragma once

#include "ConfigDllExportTrackingManagement.h"

#include <map>
#include <vector>
#include <atomic>
#include <mutex>

#include "Tracker.h"
#include "AKTracker.h"
#include "OTTracker.h"
#include "CLTracker.h"
#include "BVHPlayer.h"
#include "mmhPlayer.h"
#include "XSTracker.h"
#include "OVRTracker.h"
#include "TrackerGroup.h"
#include "Recorder.h"



/*!
*a container struct for frame meta data
*/
struct __declspec(dllexport) FrameData
{
public:
	int currFrameIdx = 0;
	float totalTime = 0.0;
	int frameCount = 0;
};



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
	TrackerManager(NetworkManager* networkManager, ConfigManager* configManager);

	~TrackerManager();

	/*!
	 * enum for all tracker types 
	 */
	enum TrackerType 
	{

		azureKinect = 0,	// Azure Kinect Tracker
		optiTrack = 1,		// OptiTrack	Tracker
		bvh = 2,			//BVH-Player
		mmh = 3,
		CapturyLive = 4,	// CapturyLive  Tracker
		xSens = 5,
		openVR = 6,	
		group = 7			//Is not display and counted in the createTrackerWindow.iu. Pls add a new tracker before group

	};



	/*!
	 * creates a new tracker and adds it to the pool
	 * 
	 * \param type type of the tracker (Azure Kinect, XSense, ...)
	 */
	int createTracker(TrackerType type, std::string filePath = "");
	/*!
	 * removes a tracker from the pool
	 * 
	 * \param positionInList tracker id
	 */
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

	/*!
	 * getter for the tracker pool lock
	 * 
	 * \return m_trackerPoolLock
	 */
	std::mutex* getTrackerPoolLock();

	void controlTimeline(bool stop);

	void timelineValueChange(float newValue);

	FrameData getRecCurrFrameData();

	void writeSkeletonsToRecorder();

	/*!
	 * sets the pointer to sendSkeletonDelegate() in main.cpp
	 *
	 * \param skeletonPool the skeletons detected by the tracker
	 * \param trackerID the trackers ID
	 */
	//void setSendSkeletonPtr(void (*)(std::map<int, Skeleton>* skeletonPool, int trackerID));

	NetworkManager* m_networkManager;


private:

	/*!
	 * Instatiates a new tracker pointer
	 *
	 * \throws Exception
	 * 
	 * \param type type of the tracker (Azure Kinect, XSense, ...)
	 * \param id the trackers ID
	 */
	Tracker* instantiateTracker(TrackerType type, int id, std::string filePath);

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

	/*!
	 * the next created tracker gets this tracker ID
	 * 
	 */
	int m_nextFreeTrackerID;

	/*!
	 * mutex lock for the tracker pool
	 * 
	 */
	std::mutex m_trackerPoolLock;

	ConfigManager* m_configManager;
};