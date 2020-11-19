#pragma once

#include "ConfigDllExportTrackingManagement.h"

#include <map>
#include <vector>
#include <atomic>
#include <mutex>

#include "Tracker.h"
#include "AKTracker.h"
#include "OTTracker.h"
#include "BVHPlayer.h"
#include "mmhPlayer.h"
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

	/*!
	 * enum for all tracker types 
	 */
	enum TrackerType 
	{

		azureKinect,	// Azure Kinect Tracker
		optiTrack,		// OptiTrack	Tracker
		bvh,			//BVH-Player
		mmh,
		group

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

	void timelineValueChange(int newValue);

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
	 * the next created AK tracker gets this cam ID
	 *
	 */
	int m_nextFreeAKCamID;

	/*!
	 * mutex lock for the tracker pool
	 * 
	 */
	std::mutex m_trackerPoolLock;

	ConfigManager* m_configManager;





};