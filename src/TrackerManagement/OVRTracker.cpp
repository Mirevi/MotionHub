#include "OVRTracker.h"

static bool DEBUG_DEVICES_ON_START = false;

static bool DEBUG_HMD = false;

static bool DEBUG_HIP = false;
static bool DEBUG_SKELETON_ROT = false;

static bool DEBUG_FOOT = false;

static bool DEBUG_ARM = false;


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
}

OVRTracker::~OVRTracker() {

	clearPointers();
}

void OVRTracker::start() {

	// start tracking in tracking system
	trackingSystem->start();

	// set tracking to true
	m_isTracking = true;

	//there schould only be one skeleton in the file
	m_countDetectedSkeleton = m_skeletonPool.size();

	//skeleton was added/removed, so UI updates
	m_hasSkeletonPoolChanged = true;
	
	// Refresh config from xml
	m_configManager->refresh();

	// Read assigned devices from config
	config->readAssignedDevices();

	// Assign remaining devices
	config->updateUserDeviceRoles();

	// Read offset from config
	config->readOffsets();

	// set skeleton scale from config
	hierarchicSkeleton->setScale(config->readScale());

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


	oneEuroFilter = Vector3OneEuroFilter(20.0, 1.0, 0.0, 1.0);
	// start tracking thread
	Tracker::start();
}

void OVRTracker::stop() {

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

	// Init IK Sekelton
	hierarchicSkeleton = new HierarchicSkeleton(m_properties->id);
	hierarchicSkeleton->init();

	// Init IKSolvers
	initIKSolvers();

	// Init Config & try to write default config values
	config = new OpenVRConfig(m_configManager, trackingSystem);
	config->writeDefaults();
}

void printOvrFPS() {
	static std::chrono::time_point<std::chrono::steady_clock> oldOvrTime = std::chrono::high_resolution_clock::now();
	static int ovrFps; ovrFps++;

	if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - oldOvrTime) >= std::chrono::seconds{ 1 }) {
		oldOvrTime = std::chrono::high_resolution_clock::now();

		Console::log("OVR FPS: " + std::to_string(ovrFps));
		ovrFps = 0;
	}
}

void OVRTracker::track() {

	// Poll Events in the Tracking System
	trackingSystem->pollEvents();

	// Update prediction time
	float mmhDelay = 0.0f; // 0.05f;
	trackingSystem->setPredictionTime(mmhDelay);

	// Update device poses
	trackingSystem->receiveDevicePoses();

	// Calibration toggle & Calibration
	if (GetAsyncKeyState('C') & 0x8000) { // C
		Console::log("Should Calibrate");
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
	else if (GetAsyncKeyState(VK_RETURN) & 0x8000) { // Enter
		Console::log("Enter");
	}
	else if (GetAsyncKeyState('S') & 0x8000) { // S
		if (shouldCalibrate) {
			shouldCalibrate = false;
			calibrateScale();
		}
	}

	// Lock point collection mutex
	m_pointCollectionLock.lock();

	// Loop over all devices and update point collection
	for (int i = 0; i < trackingSystem->Devices.size(); i++) {

		// Read pose from device index
		const auto& pose = trackingSystem->Poses[i];

		// Convert Rotation
		//Quaternionf rotation = eulerToQuaternion(m_properties->rotationOffset);
		//rotation = Quaternionf(-rotation.y(), -rotation.z(), rotation.w(), rotation.x());
		//rotation = Quaternionf(-rotation.y(), -rotation.z(), -rotation.w(), -rotation.x());

		//rotation = Quaternionf(-rotation.y(), -rotation.z(), rotation.w(), rotation.x());

		Quaternionf rotation = Quaternionf(-pose.rotation.y(), -pose.rotation.z(), pose.rotation.w(), pose.rotation.x());

		Vector3f position = pose.position;
		if (trackingSystem->Devices[i].index == 4) {
			position = oneEuroFilter.filter(position);
		}

		// Update point position & rotation
		m_pointCollection.updatePoint(trackingSystem->Devices[i].index, position, rotation);
	}

	// Unlock point collection mutex
	m_pointCollectionLock.unlock();

	extractSkeleton();
	
	m_isDataAvailable = true;
}

void OVRTracker::extractSkeleton() {

	//true as long new skeleton will be added to the pool
	bool createNewSkeleton = true;

	m_skeletonPoolLock.lock();

	int id = m_properties->id;

	//loop through all MMH skeletons
	for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++) {

		//when skeletons have the same ID, the skeleton is alredy in pool and no new skeleton has to be created
		//if (skData.skeletonID == itPoolSkeletons->first)
		if (id == itPoolSkeletons->first) {

			Skeleton* currSkeleton = parseSkeleton(id, &m_skeletonPool[id]);

			if (currSkeleton != nullptr) {
				// update all joints of existing skeleon with new data
				m_skeletonPool[id].m_joints = currSkeleton->m_joints;
			}

			//delete temp skeleton object 
			delete currSkeleton;

			// no new skeleton has to be created
			createNewSkeleton = false;

			break;
		}
	}

	// create new skeleton
	if (createNewSkeleton) {

		m_skeletonPool.insert({ id,  *parseSkeleton(id, NULL) });

		//skeleton was added, so UI updates
		m_hasSkeletonPoolChanged = true;

		Console::log("DataHandlerManager::extractSkeleton(): Created new skeleton with id = " + std::to_string(id));
	}

	m_skeletonPoolLock.unlock();
}

Skeleton* OVRTracker::parseSkeleton(int id, Skeleton* oldSkeletonData) {

	// skeleton data container
	Skeleton* currSkeleton = new Skeleton(id);

	Joint::JointConfidence highConf = Joint::JointConfidence::HIGH;
	Joint::JointConfidence lowConf = Joint::JointConfidence::LOW;

	// Solve Hip
	auto hipPose = getAssignedPose(Joint::HIPS);

	if (DEBUG_SKELETON_ROT) {
		hipPose.rotation = eulerToQuaternion(m_properties->rotationOffset);
	}

	ikSolverHip->solve(hipPose.position, hipPose.rotation);
	hierarchicSkeleton->hips.setConfidence(Joint::JointConfidence::HIGH);

	// Solve Spine
	auto headPose = getAssignedPose(Joint::HEAD);
	ikSolverSpine->solve(headPose.position, headPose.rotation);
	hierarchicSkeleton->head.setConfidence(Joint::JointConfidence::HIGH);

	// Solve LeftLeg
	auto leftFootPose = getAssignedPose(Joint::FOOT_L);
	ikSolverLeftLeg->solve(leftFootPose.position, leftFootPose.rotation);
	hierarchicSkeleton->leftFoot.setConfidence(Joint::JointConfidence::HIGH);

	// Solve RightLeg
	auto rightFootPose = getAssignedPose(Joint::FOOT_R);
	ikSolverRightLeg->solve(rightFootPose.position, rightFootPose.rotation);
	hierarchicSkeleton->rightFoot.setConfidence(Joint::JointConfidence::HIGH);

	// Solve LeftArm
	auto leftHandPose = getAssignedPose(Joint::HAND_L);
	ikSolverLeftArm->solve(leftHandPose.position, leftHandPose.rotation);
	hierarchicSkeleton->leftHand.setConfidence(Joint::JointConfidence::HIGH);

	// Solve RightArm
	auto rightHandPose = getAssignedPose(Joint::HAND_R);
	ikSolverRightArm->solve(rightHandPose.position, rightHandPose.rotation);
	hierarchicSkeleton->rightHand.setConfidence(Joint::JointConfidence::HIGH);

	// Insert solved Skeleton to current Skeleton 
	hierarchicSkeleton->insert(currSkeleton);
	return currSkeleton;
}

void OVRTracker::initIKSolvers() {

	// Init Hip IKSolver
	ikSolverHip = new IKSolverHip(&hierarchicSkeleton->hips);
	ikSolverHip->init();

	// Init Spine IKSolver
	ikSolverSpine = new IKSolverSpine(&hierarchicSkeleton->spine, &hierarchicSkeleton->chest, &hierarchicSkeleton->neck, &hierarchicSkeleton->head);
	ikSolverSpine->setHip(&hierarchicSkeleton->hips);
	ikSolverSpine->init();

	// Init LeftLeg IKSolver
	ikSolverLeftLeg = new IKSolverLeg(&hierarchicSkeleton->leftUpLeg, &hierarchicSkeleton->leftLeg, &hierarchicSkeleton->leftFoot);
	ikSolverLeftLeg->init();

	// Init RightLeg IKSolver
	ikSolverRightLeg = new IKSolverLeg(&hierarchicSkeleton->rightUpLeg, &hierarchicSkeleton->rightLeg, &hierarchicSkeleton->rightFoot);
	ikSolverRightLeg->init();

	// Init LeftArm IKSolver
	ikSolverLeftArm = new IKSolverArm(&hierarchicSkeleton->leftShoulder, &hierarchicSkeleton->leftArm, &hierarchicSkeleton->leftForeArm, &hierarchicSkeleton->leftHand);
	ikSolverLeftArm->setChest(&hierarchicSkeleton->chest);
	ikSolverLeftArm->init();

	// Init RightArm IKSolver
	ikSolverRightArm = new IKSolverArm(&hierarchicSkeleton->rightShoulder, &hierarchicSkeleton->rightArm, &hierarchicSkeleton->rightForeArm, &hierarchicSkeleton->rightHand);
	ikSolverRightArm->setChest(&hierarchicSkeleton->chest);
	ikSolverRightArm->init();
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

	// Reset Skeleton scale & pose
	hierarchicSkeleton->reset();

	// Calculate position between feet
	Vector3f betweenToes = lerp(hierarchicSkeleton->leftToe.getGlobalPosition(), hierarchicSkeleton->rightToe.getGlobalPosition(), 0.5f);

	auto leftHandPose = config->getPoseWithOffset(Joint::HAND_L);
	auto rightHandPose = config->getPoseWithOffset(Joint::HAND_R);

	float controllerdistance = distance(leftHandPose.position, rightHandPose.position);
	float beforeScaleDist = distance(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalPosition());

	// Scale Skeleton
	Vector3f scale = config->getCalibratedScale(hierarchicSkeleton);
	hierarchicSkeleton->setScale(scale);

	float afterScaleDist = distance(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalPosition());

	// Calibrate Hip Offset
	config->calibrateDeviceToJointOffset(Joint::HIPS);

	// Solve Hip with identity rotation
	auto hipPose = getAssignedPose(Joint::HIPS);
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

	Vector3f newHipOffset = hipOffset + Vector3f(0, -toeOffset.y(), 0);
	config->setOffsetPosition(Joint::HIPS, newHipOffset);

	// Write scale to config for Unity process
	config->writeScale(hierarchicSkeleton->getScale());
	Console::logError(toString(distance(betweenToes, newBetweenToes)));
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