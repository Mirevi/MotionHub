#pragma once

#include "Tracker.h"
#include "OpenVRTracking.h"
#include "MotionHubUtil/HierarchicSkeleton.h"
#include "MotionHubUtil/IKSolverHip.h"
#include "MotionHubUtil/IKSolverSpine.h"
#include "MotionHubUtil/IKSolverLimb.h"
#include "MotionHubUtil/IKSolverArm.h"

/*!
 * \class OVRTracker
 *
 * \brief Manages TODO: Kommentare
 *
 * \author David Nowottnik
 */
class OVRTracker : public Tracker {

public:

	/*!
	 * default constructor
	 */
	OVRTracker();

	/*!
	 * constructor wit initialization
	 *
	 * \param id Tracker ID
	 */
	OVRTracker(int id, NetworkManager* networkManager, ConfigManager* configManager);

	/*!
	 * default destructor
	 */
	~OVRTracker();

	/*!
	 * calls the start() method of the base class which sets m_tracking to true
	 */
	void start() override;

	/*!
	* executes the stop() method of the base class which sets m_tracking to false
	*/
	void stop() override;

	std::string getTrackerType() override;

private:

	/*!
	 * empty override method for Tracker::init()
	 */
	void init();

	/*!
	 * updade method used for tracker thread
	 */
	void update() override;

	/*!
	 * main tracking method
	 * captures one frame of body tracking data and saves all data in the skeleton pool
	 */
	void track() override;

	/*!
	 * gets skeleton data and inserts that data into the skeleton pool
	 */
	void extractSkeleton();

	/*!
	 * converts OpenVR tracking to default skeleton type
	 *
	 * \param skeleton input OptiTrack skeleto
	 * \param id the skeletons ID
	 * \return converted default skeleton
	 */
	Skeleton* parseSkeleton(int id, Skeleton* oldSkeletonData);

	/*!
	 * initializes IK chains
	 */
	void initIKSolvers();

private:

	OpenVRTracking trackingSystem;

	HierarchicSkeleton* hierarchicSkeleton;

	IKSolverHip* ikSolverHip;
	IKSolverSpine* ikSolverSpine;
	IKSolverLimb* ikSolverLeftLeg;
	IKSolverLimb* ikSolverRightLeg;
	IKSolverArm* ikSolverLeftArm;
	IKSolverArm* ikSolverRightArm;
};