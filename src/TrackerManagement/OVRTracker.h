#pragma once

#include "Tracker.h"
#include "OpenVRTracking.h"
#include "OpenVRConfig.h"
#include "MotionHubUtil/HierarchicSkeleton.h"
#include "MotionHubUtil/IKSolverHip.h"
#include "MotionHubUtil/IKSolverSpine.h"
#include "MotionHubUtil/IKSolverLeg.h"
#include "MotionHubUtil/IKSolverArm.h"
#include "MotionHubUtil/ButtonHelper.h"
#include "OpenVRButtonSubject.h"

#include "MotionHubUtil/OneEuroFilter.h"

/*!
 * \class OVRTracker
 *
 * \brief Manages Open VR Tracking and uses IK Solvers to realize Body Tracking
 */
class OVRTracker : public Tracker, public Observer {

public:

	/*!	
	 * default constructor
	 */
	OVRTracker();

	/*!
	 * constructor with initialization
	 *
	 * \param id Tracker ID
	 * \param networkManager reference to NetworkManager
	 * \param configManager reference to ConfigManager
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

	void refresh();

	/*!
	* executes the stop() method of the base class which sets m_tracking to false
	*/
	void stop() override;

	/*!
	* returns a unique name to clearly identify the type of tracker
	*/
	std::string getTrackerType() override;

	// TODO: Debug entfernen
	std::vector<IKSolver::DebugLine> GetDebugLineLists() {
		std::vector<IKSolver::DebugLine> lineList;

		/*
		lineList.insert(lineList.end(), ikSolverHip->debugLineList.begin(), ikSolverHip->debugLineList.end());
		lineList.insert(lineList.end(), ikSolverSpine->debugLineList.begin(), ikSolverSpine->debugLineList.end());
		lineList.insert(lineList.end(), ikSolverLeftLeg->debugLineList.begin(), ikSolverLeftLeg->debugLineList.end());
		lineList.insert(lineList.end(), ikSolverRightLeg->debugLineList.begin(), ikSolverRightLeg->debugLineList.end());
		lineList.insert(lineList.end(), ikSolverLeftArm->debugLineList.begin(), ikSolverLeftArm->debugLineList.end());
		lineList.insert(lineList.end(), ikSolverRightArm->debugLineList.begin(), ikSolverRightArm->debugLineList.end());
		*/

		return lineList;
	}

private:

	/*!
	 * override method for Tracker::init()
	 */
	void init();

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
	 * initializes IK chains
	 */
	void initIKSolvers();

	void refreshIKSolvers(bool overrideDefault = false);


	OpenVRTracking::DevicePose getAssignedPose(Joint::JointNames joint);

	void calibrate();

	void calibrateDeviceRoles();

	void calibrateDeviceToJointOffsets();

	void calibrateDeviceToJointOffset(Joint::JointNames jointName);

	void calibrateScale();

	void calibrateHintOffsets();

	/*!
	 * update the state of this observer
	 */
	virtual void notify(Subject* subject) override;

	void clearPointers() {

		// Clear memory for OpenVRConfig
		if (config != nullptr) delete config;

		// Clear memory for TrackingSystem
		if (trackingSystem != nullptr) delete trackingSystem;

		// Clear memory for HierarchicSkeleton
		if (hierarchicSkeleton != nullptr) delete hierarchicSkeleton;

		// Clear memory for IKSolverHip
		if (ikSolverHip != nullptr) delete ikSolverHip;

		// Clear memory for IKSolverSpine
		if (ikSolverSpine != nullptr) delete ikSolverSpine;

		// Clear memory for IKSolverLeg
		if (ikSolverLeftLeg != nullptr) delete ikSolverLeftLeg;

		// Clear memory for IKSolverLeg
		if (ikSolverRightLeg != nullptr) delete ikSolverRightLeg;

		// Clear memory for IKSolverArm
		if (ikSolverLeftArm != nullptr) delete ikSolverLeftArm;

		// Clear memory for IKSolverArm
		if (ikSolverRightArm != nullptr) delete ikSolverRightArm;

		if (skeleton != nullptr) delete skeleton;
	}

	void enableCalibrationMode() {

		ikSolverSpine->enableCalibrationMode();
		ikSolverLeftLeg->enableCalibrationMode();
		ikSolverRightLeg->enableCalibrationMode();
		ikSolverLeftArm->enableCalibrationMode();
		ikSolverRightArm->enableCalibrationMode();
	}

	void disableCalibrationMode() {

		ikSolverSpine->disableCalibrationMode();
		ikSolverLeftLeg->disableCalibrationMode();
		ikSolverRightLeg->disableCalibrationMode();
		ikSolverLeftArm->disableCalibrationMode();
		ikSolverRightArm->disableCalibrationMode();
	}

// TODO: private
public:
//private:

	OpenVRTracking* trackingSystem = nullptr;

	HierarchicSkeleton* hierarchicSkeleton = nullptr;

	// IKSolvers
	IKSolverHip* ikSolverHip = nullptr;
	IKSolverSpine* ikSolverSpine = nullptr;
	IKSolverLeg* ikSolverLeftLeg = nullptr;
	IKSolverLeg* ikSolverRightLeg = nullptr;
	IKSolverArm* ikSolverLeftArm = nullptr;
	IKSolverArm* ikSolverRightArm = nullptr;

	OpenVRConfig* config = nullptr;

	bool shouldCalibrate = false;

	ButtonHelper buttonHelper;


	void ovrTrack();

	std::atomic<bool> isTrackReading;

	std::thread* m_ovrTrackingThread;


	std::mutex pointCollectionTrackingLock;
	std::mutex skeletonPoolTrackingLock;

	Skeleton* skeleton = nullptr;

};