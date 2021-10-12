#include "OVRTracker.h"


static bool DEBUG_DEVICES_ON_START = true;

static bool DEBUG_HMD = false;

static bool DEBUG_HIP = false;
static bool DEBUG_SKELETON_POS = false;
static bool DEBUG_SKELETON_ROT = false;

static bool DEBUG_FOOT = false;
static bool DEBUG_ARM = false;

static bool SOLVE_IK = true;
static bool SOLVE_ARMS = true;

static bool USE_TEST_SKELETON = true;

static std::string DebugIdentifier(std::string identifier) {

	if (identifier == "LHR-774276F7") {
		return "HEAD";
	}
	else if (identifier == "LHR-F0DCF515") {
		return "HAND_L";
	}
	else if (identifier == "LHR-9AC396F1") {
		return "HAND_R";
	}
	else if (identifier == "LHR-E6910BC0") {
		return "FOOT_L";
	}
	else if (identifier == "LHR-6E2EA3BC") {
		return "FOOT_R";
	}
	else if (identifier == "LHR-61441A45") {
		return "HIPS";
	}
	else {
		return "! NOT FOUND !";
	}
}

OVRTracker::OVRTracker() {

	// Initialize OpenVR & IK Solvers
	init();
}

OVRTracker::OVRTracker(int id, NetworkManager* networkManager, ConfigManager* configManager) {

	m_networkManager = networkManager;
	m_configManager = configManager;

	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_openVR_" + toString(id);

	//default is enabled
	m_isEnabled = true;

	// Initialize OpenVR, Config & IK Solvers
	init();

	readOffsetFromConfig();
}

OVRTracker::~OVRTracker() {

	clearPointers();
}

void OVRTracker::start() {

	// start tracking in tracking system
	trackingSystem->start();

	// Refresh values from config
	refresh(true);

	// set tracking to true
	m_isTracking = true;

	//there schould only be one skeleton in the file
	m_countDetectedSkeleton = m_skeletonPool.size();

	//skeleton was added/removed, so UI updates
	m_hasSkeletonPoolChanged = true;

	// Init IKSolvers
	initIKSolvers();

	// Clear point collection
	m_pointCollection.clear();

	// Loop over all devices and convert them to points
	for (const auto& device : trackingSystem->Devices) {

		// Cast device class to point type
		Point::PointType pointType = static_cast<Point::PointType>(device.deviceClass);

		// Add point to collection
		m_pointCollection.addPoint(device.index, pointType);

		// Update Joint if assigned
		Joint::JointNames joint = config->getJoint(device.index);
		if (joint != Joint::NDEF) {
			m_pointCollection.updatePoint(device.index, joint);
		}
	}

	// register tracker as observers
	trackingSystem->registerButtonPressObserver(this);

	if (DEBUG_DEVICES_ON_START) {
		Console::log("");
		for (const auto& device : trackingSystem->Devices) {
			Console::log(toString((int)device.index) + " " + OpenVRTracking::toString(device.deviceClass) + " " + device.identifier + " " + Joint::toString(config->getJoint(device.index)));
			Console::log(DebugIdentifier(device.identifier));
		}
		Console::log("");
	}

	// Update Confidence for each tracked joint
	for (const auto& device : trackingSystem->Devices) {
		auto deviceJoint = config->getJoint(device.index);
		auto skeletonJoint = hierarchicSkeleton->getJoint(deviceJoint);

		if (deviceJoint != Joint::NDEF && skeletonJoint != nullptr) {
			skeletonJoint->setConfidence(Joint::HIGH);
		}
	}

	// start tracking thread
	Tracker::start();
}

void OVRTracker::refresh(bool overrideDefaults) {

	// Refresh config from xml
	m_configManager->refresh();

	// Read global offsets
	readOffsetFromConfig();

	// Update prediction time from config
	//float mmhDelay = 0.0255f; // 0.0255f
	float predictionTime = config->readPredictionTime();
	if (predictionTime > 0.0f) {
		Console::log("OVRTracker::refresh: prediction time " + toString(predictionTime) + " s");
		trackingSystem->setPredictionTime(predictionTime);
	}

	// Read assigned devices from config
	config->readAssignedDevices();

	// Assign remaining devices
	config->updateUserDeviceRoles();

	// Read offset from config
	config->readOffsets();

	// Should use TestSkeleton?
	if (USE_TEST_SKELETON) {
		// Enable TestSkeleton if HintDevice is configured
		if (config->getDeviceIndex(Joint::LEG_L) >= 0
			|| config->getDeviceIndex(Joint::LEG_R) >= 0
			|| config->getDeviceIndex(Joint::FOREARM_L) >= 0
			|| config->getDeviceIndex(Joint::FOREARM_R) >= 0)
		{
			useTestSkeleton = true;
		}
	}

	// Init skeleton & set scale from config
	hierarchicSkeleton->setScale(config->readScale());
	hierarchicSkeleton->init();

	if (useTestSkeleton) {
		testHierarchicSkeleton->setScale(config->readScale());
		testHierarchicSkeleton->init();
	}

	// Update Filters
	int i = 0;
	for (auto& device : trackingSystem->Devices) {
		Joint::JointNames joint = config->getJoint(device.index);

		if (joint == Joint::HAND_L || joint == Joint::FOOT_L) {
			trackingSystem->positionFilters[i].setParams(1, 1.0f, 0.007f);
			trackingSystem->rotationFilters[i].setParams(2, 1.0f, 0.007f);
		}
		else if (joint == Joint::HIPS) {
			trackingSystem->positionFilters[i].setParams(60, 1.0f, 0.007f);
			trackingSystem->rotationFilters[i].setParams(120, 1.0f, 0.007f);
		}
		else {
			trackingSystem->positionFilters[i].setParams(30, 1.0f, 0.007f);
			trackingSystem->rotationFilters[i].setParams(60, 1.0f, 0.007f);
		}

		i++;
	}

	// Refresh IKSolvers (if initialized)
	refreshIKSolvers(overrideDefaults);
}

void OVRTracker::stop() {

	// write settings & calibration to config 
	config->write();
	config->writeScale(hierarchicSkeleton->getScale());

	// remove tracker from observers
	trackingSystem->removeButtonPressObserver(this);

	Tracker::stop();
}

void OVRTracker::init() {

	// Create & init OpenVR Tracking
	trackingSystem = new OpenVRTracking();

	try {
		trackingSystem->init();
	}
	catch (Exception exception) {
		delete trackingSystem;
		throw exception;
	}

	// Create IK Skeleton
	hierarchicSkeleton = new HierarchicSkeleton(m_properties->id);
	testHierarchicSkeleton = new HierarchicSkeleton(m_properties->id + 1);

	skeleton = new Skeleton(m_properties->id);
	testSkeleton = new Skeleton(m_properties->id + 1);

	// Init Config & try to write default config values
	config = new OpenVRConfig(m_configManager, trackingSystem);
	config->writeDefaults();
}

void OVRTracker::track() {

	// Update offsets from Inspector
	trackingSystem->setOffsets(m_properties->positionOffset, m_rotationOffset, m_properties->scaleOffset);

	// Poll Events in the Tracking System
	trackingSystem->pollEvents();

	// Update device poses
	trackingSystem->receiveDevicePoses();

	// Calibration toggle & Calibration
	if (GetAsyncKeyState('C') & 0x8000) {
		Console::log("OVRTracker: Should Calibrate");
		shouldCalibrate = true;
	}
	else if (GetAsyncKeyState(0x20) & 0x8000) { // Space
		calibrate();
	}

	// Calibration for target joints
	if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000) {
		calibrateDeviceToJointOffset(Joint::FOOT_L);
	}
	else if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000) {
		calibrateDeviceToJointOffset(Joint::FOOT_R);
	}
	else if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000) {
		calibrateDeviceToJointOffset(Joint::HIPS);
	}
	else if (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) {
		calibrateDeviceToJointOffset(Joint::HAND_L);
	}
	else if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) {
		calibrateDeviceToJointOffset(Joint::HAND_R);
	}
	else if (GetAsyncKeyState('S') & 0x8000) {
		if (shouldCalibrate) {
			shouldCalibrate = false;
			calibrateScale();
		}
	}

	if (GetAsyncKeyState('I') & 0x8000) {
		SOLVE_IK = true;
	}

	if (GetAsyncKeyState('R') & 0x8000) {
		refresh(false);
	}

	if (SOLVE_IK) {

		// Hint Hip with feet
		auto leftFootPose = getAssignedPose(Joint::FOOT_L);
		auto rightFootPose = getAssignedPose(Joint::FOOT_R);

		ikSolverHip->hint(leftFootPose.position, rightFootPose.position);

		if (useTestSkeleton) testIkSolverHip->hint(leftFootPose.position, rightFootPose.position);

		// Solve Hip
		auto hipPose = getAssignedPose(Joint::HIPS);
		if (DEBUG_SKELETON_POS) {
			hipPose.position += m_properties->positionOffset;
		}
		if (DEBUG_SKELETON_ROT) {
			hipPose.rotation = eulerToQuaternion(m_properties->rotationOffset);
		}
		ikSolverHip->solve(hipPose.position, hipPose.rotation);

		if (useTestSkeleton) testIkSolverHip->solve(hipPose.position, hipPose.rotation);

		// Solve Spine
		auto headPose = getAssignedPose(Joint::HEAD);

		if (DEBUG_SKELETON_POS) {
			headPose.position += m_properties->positionOffset;
		}

		ikSolverSpine->solve(headPose.position, headPose.rotation);

		if (useTestSkeleton) testIkSolverSpine->solve(headPose.position, headPose.rotation);

		// Hint LeftLeg
		auto leftLegPose = getAssignedPose(Joint::LEG_L);
		if (!leftLegPose.isNull()) {
			ikSolverLeftLeg->hint(leftLegPose.position, leftLegPose.rotation);
		}

		// Solve LeftLeg
		ikSolverLeftLeg->solve(leftFootPose.position, leftFootPose.rotation);

		if (useTestSkeleton) testIkSolverLeftLeg->solve(leftFootPose.position, leftFootPose.rotation);

		// Hint RightLeg
		auto rightLegPose = getAssignedPose(Joint::LEG_R);
		if (!rightLegPose.isNull()) {
			ikSolverRightLeg->hint(rightLegPose.position, rightLegPose.rotation);
		}

		// Solve RightLeg
		ikSolverRightLeg->solve(rightFootPose.position, rightFootPose.rotation);

		if (useTestSkeleton) testIkSolverRightLeg->solve(rightFootPose.position, rightFootPose.rotation);

		if (SOLVE_ARMS) {
			// Hint LeftArm
			auto leftForearmPose = getAssignedPose(Joint::FOREARM_L);
			if (!leftForearmPose.isNull()) {
				ikSolverLeftArm->hint(leftForearmPose.position, leftForearmPose.rotation);
			}

			// Solve LeftArm
			auto leftHandPose = getAssignedPose(Joint::HAND_L);
			ikSolverLeftArm->solve(leftHandPose.position, leftHandPose.rotation);

			if (useTestSkeleton) testIkSolverLeftArm->solve(leftHandPose.position, leftHandPose.rotation);

			// Hint RightArm
			auto rightForearmPose = getAssignedPose(Joint::FOREARM_R);
			if (!rightForearmPose.isNull()) {
				ikSolverRightArm->hint(rightForearmPose.position, rightForearmPose.rotation);
			}

			// Solve RightArm
			auto rightHandPose = getAssignedPose(Joint::HAND_R);
			ikSolverRightArm->solve(rightHandPose.position, rightHandPose.rotation);

			if (useTestSkeleton) testIkSolverRightArm->solve(rightHandPose.position, rightHandPose.rotation);
		}
	}

	hierarchicSkeleton->insert(nullptr);
	if (testHierarchicSkeleton != nullptr) {
		testHierarchicSkeleton->insert(nullptr);
	}

	auto poses = trackingSystem->Poses;

	skeleton->m_joints = hierarchicSkeleton->skeleton.m_joints;
	if (useTestSkeleton) {
		testSkeleton->m_joints = testHierarchicSkeleton->skeleton.m_joints;
	}
	//hierarchicSkeleton->insert(nullptr);


	// Lock point collection mutex
	m_pointCollectionLock.lock();

	int i = 0;
	for (auto& device : trackingSystem->Devices) {
		// Read pose from device index
		auto& pose = poses[i];

		auto joint = config->getJoint(device.index);
		if (joint == Joint::HAND_L || joint == Joint::FOOT_L) {
			pose = trackingSystem->FilteredPoses[i];
		}

		// Convert rotation & update point
		Quaternionf rotation = Quaternionf(-pose.rotation.y(), -pose.rotation.z(), pose.rotation.w(), pose.rotation.x());
		m_pointCollection.updatePoint(device.index, pose.position, rotation);

		i++;
	}

	// Unlock point collection mutex
	m_pointCollectionLock.unlock();

	extractSkeleton();

	m_isDataAvailable = true;
}

void OVRTracker::extractSkeleton() {

	m_skeletonPoolLock.lock();

	// Get id from tracker properties
	int id = m_properties->id;

	// Try to find skeleton with wid
	auto skeletonIterator = m_skeletonPool.find(id);

	// Skeleton found -> update joints
	if (skeletonIterator != m_skeletonPool.end()) {

		// update all joints of existing skeleon with new data
		skeletonIterator->second.m_joints = skeleton->m_joints;
	}
	// Skeleton not found -> insert new
	else {

		m_skeletonPool.insert({ id, *skeleton });

		//skeleton was added, so UI updates
		m_hasSkeletonPoolChanged = true;

		Console::log("OVRTracker::extractSkeleton(): Created new skeleton with id = " + std::to_string(id));
	}

	if (useTestSkeleton) {
		id = id + 1;

		skeletonIterator = m_skeletonPool.find(id);

		// Skeleton found -> update joints
		if (skeletonIterator != m_skeletonPool.end()) {

			// update all joints of existing skeleon with new data
			skeletonIterator->second.m_joints = testSkeleton->m_joints;
		}
		// Skeleton not found -> insert new
		else {

			m_skeletonPool.insert({ id, *testSkeleton });

			//skeleton was added, so UI updates
			m_hasSkeletonPoolChanged = true;

			Console::log("OVRTracker::extractSkeleton(): Created new skeleton with id = " + std::to_string(id));
		}

	}

	m_skeletonPoolLock.unlock();
}

void OVRTracker::initIKSolvers() {

	// Init Hip IKSolver
	if (ikSolverHip != nullptr) delete ikSolverHip;
	ikSolverHip = new IKSolverHip(&hierarchicSkeleton->hips);
	ikSolverHip->setLeftLeg(&hierarchicSkeleton->leftUpLeg, &hierarchicSkeleton->leftFoot);
	ikSolverHip->setRightLeg(&hierarchicSkeleton->rightUpLeg, &hierarchicSkeleton->rightFoot);
	ikSolverHip->init();

	// Init Spine IKSolver
	if (ikSolverSpine != nullptr) delete ikSolverSpine;
	ikSolverSpine = new IKSolverSpine(&hierarchicSkeleton->spine, &hierarchicSkeleton->chest, &hierarchicSkeleton->neck, &hierarchicSkeleton->head);
	ikSolverSpine->setHip(&hierarchicSkeleton->hips);
	ikSolverSpine->init();

	// Init LeftLeg IKSolver
	if (ikSolverLeftLeg != nullptr) delete ikSolverLeftLeg;
	ikSolverLeftLeg = new IKSolverLeg(&hierarchicSkeleton->leftUpLeg, &hierarchicSkeleton->leftLeg, &hierarchicSkeleton->leftFoot);
	ikSolverLeftLeg->init();

	// Init RightLeg IKSolver
	if (ikSolverRightLeg != nullptr) delete ikSolverRightLeg;
	ikSolverRightLeg = new IKSolverLeg(&hierarchicSkeleton->rightUpLeg, &hierarchicSkeleton->rightLeg, &hierarchicSkeleton->rightFoot);
	ikSolverRightLeg->init();

	// Init LeftArm IKSolver
	if (ikSolverLeftArm != nullptr) delete ikSolverLeftArm;
	ikSolverLeftArm = new IKSolverArm(&hierarchicSkeleton->leftShoulder, &hierarchicSkeleton->leftArm, &hierarchicSkeleton->leftForeArm, &hierarchicSkeleton->leftHand);
	ikSolverLeftArm->setChest(&hierarchicSkeleton->chest);
	ikSolverLeftArm->init();

	// Init RightArm IKSolver
	if (ikSolverRightArm != nullptr) delete ikSolverRightArm;
	ikSolverRightArm = new IKSolverArm(&hierarchicSkeleton->rightShoulder, &hierarchicSkeleton->rightArm, &hierarchicSkeleton->rightForeArm, &hierarchicSkeleton->rightHand);
	ikSolverRightArm->setChest(&hierarchicSkeleton->chest);
	ikSolverRightArm->init();

	// Solve in T-Pose
	ikSolverHip->solve(hierarchicSkeleton->hips.getGlobalPosition(), hierarchicSkeleton->hips.getGlobalRotation());
	ikSolverSpine->solve(hierarchicSkeleton->head.getGlobalPosition(), hierarchicSkeleton->head.getGlobalRotation());
	ikSolverLeftLeg->solve(hierarchicSkeleton->leftFoot.getGlobalPosition(), hierarchicSkeleton->leftFoot.getGlobalRotation());
	ikSolverRightLeg->solve(hierarchicSkeleton->rightFoot.getGlobalPosition(), hierarchicSkeleton->rightFoot.getGlobalRotation());
	ikSolverLeftArm->solve(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->leftHand.getGlobalRotation());
	ikSolverRightArm->solve(hierarchicSkeleton->rightHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalRotation());

	if (useTestSkeleton) {
		// Init Hip IKSolver
		if (testIkSolverHip != nullptr) delete testIkSolverHip;
		testIkSolverHip = new IKSolverHip(&testHierarchicSkeleton->hips);
		testIkSolverHip->setLeftLeg(&testHierarchicSkeleton->leftUpLeg, &testHierarchicSkeleton->leftFoot);
		testIkSolverHip->setRightLeg(&testHierarchicSkeleton->rightUpLeg, &testHierarchicSkeleton->rightFoot);
		testIkSolverHip->init();

		// Init Spine IKSolver
		if (testIkSolverSpine != nullptr) delete testIkSolverSpine;
		testIkSolverSpine = new IKSolverSpine(&testHierarchicSkeleton->spine, &testHierarchicSkeleton->chest, &testHierarchicSkeleton->neck, &testHierarchicSkeleton->head);
		testIkSolverSpine->setHip(&testHierarchicSkeleton->hips);
		testIkSolverSpine->init();

		// Init LeftLeg IKSolver
		if (testIkSolverLeftLeg != nullptr) delete testIkSolverLeftLeg;
		testIkSolverLeftLeg = new IKSolverLeg(&testHierarchicSkeleton->leftUpLeg, &testHierarchicSkeleton->leftLeg, &testHierarchicSkeleton->leftFoot);
		testIkSolverLeftLeg->init();

		// Init RightLeg IKSolver
		if (testIkSolverRightLeg != nullptr) delete testIkSolverRightLeg;
		testIkSolverRightLeg = new IKSolverLeg(&testHierarchicSkeleton->rightUpLeg, &testHierarchicSkeleton->rightLeg, &testHierarchicSkeleton->rightFoot);
		testIkSolverRightLeg->init();

		// Init LeftArm IKSolver
		if (testIkSolverLeftArm != nullptr) delete testIkSolverLeftArm;
		testIkSolverLeftArm = new IKSolverArm(&testHierarchicSkeleton->leftShoulder, &testHierarchicSkeleton->leftArm, &testHierarchicSkeleton->leftForeArm, &testHierarchicSkeleton->leftHand);
		testIkSolverLeftArm->setChest(&testHierarchicSkeleton->chest);
		testIkSolverLeftArm->init();

		// Init RightArm IKSolver
		if (testIkSolverRightArm != nullptr) delete testIkSolverRightArm;
		testIkSolverRightArm = new IKSolverArm(&testHierarchicSkeleton->rightShoulder, &testHierarchicSkeleton->rightArm, &testHierarchicSkeleton->rightForeArm, &testHierarchicSkeleton->rightHand);
		testIkSolverRightArm->setChest(&testHierarchicSkeleton->chest);
		testIkSolverRightArm->init();

		// Solve in T-Pose
		testIkSolverHip->solve(testHierarchicSkeleton->hips.getGlobalPosition(), testHierarchicSkeleton->hips.getGlobalRotation());
		testIkSolverSpine->solve(testHierarchicSkeleton->head.getGlobalPosition(), testHierarchicSkeleton->head.getGlobalRotation());
		testIkSolverLeftLeg->solve(testHierarchicSkeleton->leftFoot.getGlobalPosition(), testHierarchicSkeleton->leftFoot.getGlobalRotation());
		testIkSolverRightLeg->solve(testHierarchicSkeleton->rightFoot.getGlobalPosition(), testHierarchicSkeleton->rightFoot.getGlobalRotation());
		testIkSolverLeftArm->solve(testHierarchicSkeleton->leftHand.getGlobalPosition(), testHierarchicSkeleton->leftHand.getGlobalRotation());
		testIkSolverRightArm->solve(testHierarchicSkeleton->rightHand.getGlobalPosition(), testHierarchicSkeleton->rightHand.getGlobalRotation());
	}
}

void OVRTracker::refreshIKSolvers(bool overrideDefault) {
	
	if (ikSolverHip != nullptr) {
		ikSolverHip->refresh(overrideDefault);
	}
	
	if (ikSolverLeftLeg != nullptr) {
		ikSolverLeftLeg->refresh(overrideDefault);
	}

	if (ikSolverRightLeg != nullptr) {
		ikSolverRightLeg->refresh(overrideDefault);
	}

	if (ikSolverLeftArm != nullptr) {
		ikSolverLeftArm->refresh(overrideDefault);
	}

	if (ikSolverRightArm != nullptr) {
		ikSolverRightArm->refresh(overrideDefault);
	}

	if (hierarchicSkeleton != nullptr) {
		hierarchicSkeleton->invalidateGlobals();
	}



	if (testIkSolverHip != nullptr) {
		testIkSolverHip->refresh(overrideDefault);
	}

	if (testIkSolverLeftLeg != nullptr) {
		testIkSolverLeftLeg->refresh(overrideDefault);
	}

	if (testIkSolverRightLeg != nullptr) {
		testIkSolverRightLeg->refresh(overrideDefault);
	}

	if (testIkSolverLeftArm != nullptr) {
		testIkSolverLeftArm->refresh(overrideDefault);
	}

	if (testIkSolverRightArm != nullptr) {
		testIkSolverRightArm->refresh(overrideDefault);
	}

	if (testHierarchicSkeleton != nullptr) {
		testHierarchicSkeleton->invalidateGlobals();
	}
}

OpenVRTracking::DevicePose OVRTracker::getAssignedPose(Joint::JointNames joint) {

	// Get the pose from config & tracking system 
	auto pose = config->getPoseWithOffset(joint);

	// TODO: Debug entfernen
	switch (joint) {
	case Joint::HIPS:
		if (DEBUG_HIP) {
			pose.position = Vector3f(0, 1.0f, 0);
			//pose.rotation = Quaternionf::Identity();
		}
		break;
	case Joint::FOREARM_L:
		break;
	case Joint::HAND_L:
		break;
	case Joint::FOREARM_R:
		break;
	case Joint::HAND_R:
		break;
	case Joint::LEG_L:
		break;
	case Joint::FOOT_L:
		break;
	case Joint::LEG_R:
		break;
	case Joint::FOOT_R:
		break;
	case Joint::HEAD:
		if (DEBUG_HMD)
			pose.position = Vector3f(0, 1.6f, 0);
		break;
	}

	return pose;
}

void OVRTracker::calibrate() {

	// Only calibrate if desired
	if (!shouldCalibrate) {
		return;
	}

	// Reset calibration flag
	shouldCalibrate = false;

	calibrateDeviceRoles();

	calibrateDeviceToJointOffsets();

	calibrateScale();

	calibrateHintOffsets();

	refreshIKSolvers();

	Console::log("OVRTracker::calibrate");
}

void OVRTracker::calibrateDeviceRoles() {

	auto calibratedDeviceRoles = trackingSystem->getCalibratedDeviceRoles();

	if (calibratedDeviceRoles.size() > 0) {
		config->clearJointToDevice();

		// Write calibrated DeviceRoles to Config
		for (const auto& deviceJointPair : calibratedDeviceRoles) {

			auto device = trackingSystem->Devices[deviceJointPair.first];
			config->assignJointToDevice(deviceJointPair.second, device.index);
		}

		Console::log("Calibrated Device Roles");
	}
}

void OVRTracker::calibrateDeviceToJointOffsets() {

	config->calibrateDeviceToJointOffsets();

	Console::log("Calibrated Device Joint Offsets");
}

void OVRTracker::calibrateDeviceToJointOffset(Joint::JointNames joint) {

	config->calibrateDeviceToJointOffset(joint);

	Console::log("Calibrated Device Joint Offset: " + Joint::toString(joint));
}

void OVRTracker::calibrateScale() {

	enableCalibrationMode();

	Vector3f oldScale = hierarchicSkeleton->getScale();

	// Reset Skeleton scale & pose
	hierarchicSkeleton->reset();

	// Calculate position between feet
	Vector3f betweenToes = lerp(hierarchicSkeleton->leftToe.getGlobalPosition(), hierarchicSkeleton->rightToe.getGlobalPosition(), 0.5f);

	auto headPose = config->getPoseWithOffset(Joint::HEAD, false);
	auto leftHandPose = config->getPoseWithOffset(Joint::HAND_L, false);
	auto rightHandPose = config->getPoseWithOffset(Joint::HAND_R, false);

	float controllerdistance = distance(leftHandPose.position, rightHandPose.position);
	float beforeScaleDist = distance(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalPosition());

	// Scale Skeleton
	Vector3f scale = config->getCalibratedScale(hierarchicSkeleton);

	// Is scale invalid?
	if (scale.x() < 0.0f || scale.y() < 0.0f) {
		// Revert scaling
		hierarchicSkeleton->setScale(oldScale);
		hierarchicSkeleton->init();

		disableCalibrationMode();

		Console::logError("OVRTracker::calibrateScale: Invalid Scale x or y < 0");
		return;
	}

	hierarchicSkeleton->setScale(scale);
	hierarchicSkeleton->init();

	refreshIKSolvers(true);

	float afterScaleDist = distance(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalPosition());

	// Calibrate Hip Offset
	config->calibrateDeviceToJointOffset(Joint::HIPS);

	// Solve Hip with identity rotation
	auto hipPose = config->getPoseWithOffset(Joint::HIPS, false);
	ikSolverHip->solve(hipPose.position, Quaternionf::Identity());

	// y diff ermitteln und auf hip offset rechnen?
	Vector3f newBetweenToes = lerp(hierarchicSkeleton->leftToe.getGlobalPosition(), hierarchicSkeleton->rightToe.getGlobalPosition(), 0.5f);

	Vector3f toeOffset = newBetweenToes - betweenToes;

	Console::logError("feetOffset: " + toString(toeOffset));
	Console::logWarning("controllerdistance: " + toString(controllerdistance));
	Console::logWarning("beforeScaleDist: " + toString(beforeScaleDist));
	Console::logWarning("afterScaleDist: " + toString(afterScaleDist));

	// Read current hip offset from config
	Vector3f hipOffset = config->getOffsetPosition(Joint::HIPS);

	Vector3f headToHip = hipPose.position - headPose.position;

	Vector3f hipHeadOffset = projectOnPlane(headToHip, hipPose.rotation * Vector3f(0, 0, 1));

	Vector3f newHipOffset = hipOffset + Vector3f(hipHeadOffset.x(), -toeOffset.y(), 0);
	//Vector3f newHipOffset = hipOffset + Vector3f(0, -toeOffset.y(), 0);
	config->setOffsetPosition(Joint::HIPS, newHipOffset);

	hipPose = config->getPoseWithOffset(Joint::HIPS, false);
	ikSolverHip->solve(hipPose.position, hipPose.rotation);

	Console::log("Hip y Offset: " + toString(toeOffset.y()));

	Console::logError(toString(distance(betweenToes, newBetweenToes)));

	Vector3f leftHandOffset = leftHandPose.position - lerp(leftHandPose.position, rightHandPose.position, 0.25f);
	Vector3f rightHandOffset = rightHandPose.position - lerp(rightHandPose.position, leftHandPose.position, 0.25f);

	ikSolverSpine->solve(headPose.position, headPose.rotation);
	ikSolverLeftArm->solve(leftHandPose.position + leftHandOffset.normalized() * 0.1f, leftHandPose.rotation);
	ikSolverRightArm->solve(rightHandPose.position + rightHandOffset.normalized() * 0.1f, leftHandPose.rotation);

	// Solve LeftFoot & RightFoot lower than Zero to get max stretched distance
	auto leftFootPose = config->getPoseWithOffset(Joint::FOOT_L, false);
	auto rightFootPose = config->getPoseWithOffset(Joint::FOOT_R, false);
	ikSolverLeftLeg->solve(leftFootPose.position + Vector3f(0, -0.5f, 0), leftFootPose.rotation);
	ikSolverRightLeg->solve(rightFootPose.position + Vector3f(0, -0.5f, 0), rightFootPose.rotation);

	// Is LeftFoot tracked? -> Calculate Y Offset
	if (config->isJointAssigned(Joint::FOOT_L)) {
		auto leftFootPose = config->getPoseWithOffset(Joint::FOOT_L, false);
		Vector3f newOffsetL = (hierarchicSkeleton->leftFoot.getGlobalPosition()) - leftFootPose.position;

		Vector3f offsetL = config->getUserOffsetPosition(Joint::FOOT_L);
		offsetL.y() += newOffsetL.y();

		config->setUserOffsetPosition(Joint::FOOT_L, offsetL);
	}

	// Is RightFoot tracked? -> Calculate Y Offset
	if (config->isJointAssigned(Joint::FOOT_R)) {
		auto rightFootPose = config->getPoseWithOffset(Joint::FOOT_R, false);
		Vector3f newOffsetR = (hierarchicSkeleton->leftFoot.getGlobalPosition()) - rightFootPose.position;

		Vector3f offsetR = config->getUserOffsetPosition(Joint::FOOT_R);
		offsetR.y() += newOffsetR.y();

		config->setUserOffsetPosition(Joint::FOOT_R, offsetR);
	}

	float afterSolveDist = distance(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalPosition());
	Console::logWarning("afterSolveDist: " + toString(afterSolveDist));
	Console::logWarning("L -> LC: " + toString(distance(hierarchicSkeleton->leftHand.getGlobalPosition(), leftHandPose.position)));
	Console::logWarning("R -> RC: " + toString(distance(hierarchicSkeleton->rightHand.getGlobalPosition(), rightHandPose.position)));

	float xDelta = afterScaleDist / afterSolveDist;
	scale.x() *= xDelta;
	scale.z() *= xDelta;

	hierarchicSkeleton->setScale(scale);
	hierarchicSkeleton->init();

	if (useTestSkeleton) {
		testHierarchicSkeleton->setScale(scale);
		testHierarchicSkeleton->init();
	}

	refreshIKSolvers(true);

	// Write scale to config for Unity process
	config->writeScale(scale);

	disableCalibrationMode();
}

void OVRTracker::calibrateHintOffsets() {

	Joint::JointNames limbJoints[]{
		Joint::LEG_L,
		Joint::LEG_R,
		Joint::FOREARM_L,
		Joint::FOREARM_R
	};

	enableCalibrationMode();

	for (auto limbJoint : limbJoints) {
		int deviceIndex = config->getDeviceIndex(limbJoint);

		// Skip if limb is not assigned
		if (deviceIndex == -1) {
			continue;
		}

		// get joint from skeleton
		HierarchicJoint* joint = hierarchicSkeleton->getJoint(limbJoint);


		auto limbDevicePose = config->getPose(limbJoint);


		//config->calibrateDeviceToJointOffset(limbJoint);

		Quaternionf offsetRot = limbDevicePose->rotation.inverse() * joint->getGlobalRotation();
		config->setOffsetRotation(limbJoint, offsetRot);

		/*
		// Save user offset & set it to zero
		Vector3f oldOffset = config->getUserOffsetPosition(limbJoint);
		config->setUserOffsetPosition(limbJoint, Vector3f::Zero());
		config->setOffsetPosition(limbJoint, Vector3f::Zero());

		// TODO: Position Offset ermitteln und speichern
		auto limbPose = config->getPoseWithOffset(limbJoint, false);

		Vector3f offset = (joint->getGlobalPosition() - limbPose.position);

		config->setOffsetPosition(limbJoint, offset);

		// Restore old user offset
		config->setUserOffsetPosition(limbJoint, oldOffset);
		*/
	}

	disableCalibrationMode();
}

void OVRTracker::notify(Subject* subject) {

	// Cast subject to OpenVR button subject
	OpenVRButtonSubject* ovrButtonSubject = dynamic_cast<OpenVRButtonSubject*>(subject);

	// subject == OpenVRButtonSubject?
	if (ovrButtonSubject != nullptr) {
		Console::log("Is Trigger: " + toString(ovrButtonSubject->getButtonState() == vr::EVRButtonId::k_EButton_SteamVR_Trigger));

		switch (ovrButtonSubject->getButtonState()) {

			// Calibrate on trigger pressed
		case vr::EVRButtonId::k_EButton_SteamVR_Trigger:
			calibrate();
			break;

		case vr::EVRButtonId::k_EButton_ApplicationMenu:
			Console::log("k_EButton_ApplicationMenu");
			if (shouldCalibrate) {
				shouldCalibrate = false;
				calibrateScale();
			}

			break;
		}
	}
}

std::string OVRTracker::getTrackerType() {

	return "OpenVR";
}