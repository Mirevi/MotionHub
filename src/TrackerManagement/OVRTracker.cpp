#include "OVRTracker.h"

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

	// Initialize OpenVR & IK Solvers
	init();
}

OVRTracker::~OVRTracker() {

	// Free memory for TrackingSystem
	if (trackingSystem != NULL) delete trackingSystem;

	// Free memory for HierarchicSkeleton
	if (hierarchicSkeleton != NULL) delete hierarchicSkeleton;

	// Free memory for IKSolverHip
	if (ikSolverHip != NULL) delete ikSolverHip;

	// Free memory for IKSolverSpine
	if (ikSolverSpine != NULL) delete ikSolverSpine;

	// Free memory for IKSolverLeg
	if (ikSolverLeftLeg != NULL) delete ikSolverLeftLeg;

	// Free memory for IKSolverLeg
	if (ikSolverRightLeg != NULL) delete ikSolverRightLeg;

	// Free memory for IKSolverArm
	if (ikSolverLeftArm != NULL) delete ikSolverLeftArm;

	// Free memory for IKSolverArm
	if (ikSolverRightArm != NULL) delete ikSolverRightArm;
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

	// Clear point collection
	m_pointCollection.clear();

	// Loop over all devices and convert them to points
	for (const auto& device : trackingSystem->Devices) {

		// Cast device class to point type
		Point::PointType pointType = static_cast<Point::PointType>(device.deviceClass);

		// Add point to collection
		m_pointCollection.addPoint(device.index, pointType);

		// TODO: Set Joint Name
		//m_pointCollection.updatePoint(device.Index, device.)
	}

	// register tracker as observers
	trackingSystem->registerButtonPressObserver(this);

	// start tracking thread and detach the thread from method scope runtime
	//m_trackingThread = new std::thread(&OVRTracker::update, this);
	//m_trackingThread->detach();
	Tracker::start();
}

void OVRTracker::stop() {

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
	hierarchicSkeleton = new HierarchicSkeleton();
	hierarchicSkeleton->init();

	// Init IKSolvers
	initIKSolvers();
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
		shouldCalibrate = true;
	}
	else if (GetAsyncKeyState(0x20) & 0x8000) { // Space
		calibrate();
	}

	// Lock point collection mutex
	m_pointCollectionLock.lock();

	// Loop over all devices and update points
	for (int i = 0; i < trackingSystem->Devices.size(); i++) {

		// Read pose from device index
		const auto& pose = trackingSystem->Poses[i];

		// Convert Rotation
		Quaternionf rotation = pose.rotation;
		//Quaternionf rotation = eulerToQuaternion(m_properties->rotationOffset);
		//rotation = Quaternionf(-rotation.y(), -rotation.z(), rotation.w(), rotation.x());
		//rotation = Quaternionf(-rotation.y(), -rotation.z(), -rotation.w(), -rotation.x());

		rotation = Quaternionf(-rotation.y(), -rotation.z(), rotation.w(), rotation.x());

		// Update point position & rotation
		m_pointCollection.updatePoint(trackingSystem->Devices[i].index, pose.position, rotation);

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

	if (1 == 0 && trackingSystem->Poses.size() > 1) {
		Vector3f v3pos = trackingSystem->Poses[0].position;
		Vector4f pos = Vector4f(v3pos.x(), v3pos.y(), v3pos.z(), 1.0f);

		Quaternionf offset = eulerToQuaternion(Vector3f(0.0f, 90.0f, 0.0f));
		Quaternionf rot = trackingSystem->Poses[0].rotation * offset;

		currSkeleton->m_joints.insert({ Joint::HEAD, Joint(pos, rot, Joint::JointConfidence::HIGH) });
	}

	//OpenVRTracking::DevicePose* hipPose = getAssignedPose(Joint::HIPS);
	//hipPose->Position = Vector3f(0, 1.0f, 0);
	//hipPose->Rotation = Quaternionf::Identity();

	ikSolverHip->solve(Vector3f(0, 1.0f, 0), Quaternionf::Identity());
	hierarchicSkeleton->hips.setConfidence(Joint::JointConfidence::HIGH);

	//OpenVRTracking::DevicePose* headPose = getAssignedPose(Joint::HEAD);
	//headPose->Position = Vector3f(0, 1.8f, 0);
	//headPose->Rotation = Quaternionf::Identity();

	//ikSolverSpine->solve(Vector3f(0, 1.8f, 0), Quaternionf::Identity());
	ikSolverSpine->solve(Vector3f(0, 1.8f, 0) + m_properties->positionOffset, eulerToQuaternion(m_properties->rotationOffset));
	hierarchicSkeleton->head.setConfidence(Joint::JointConfidence::HIGH);

	Vector3f position = m_properties->positionOffset;

	//ikSolverLeftLeg->middleUntwistWeight = m_properties->scaleOffset.x();
	//ikSolverLeftLeg->upperUntwistWeight = m_properties->scaleOffset.y();
	//ikSolverLeftLeg->DebugFloat1 = m_properties->scaleOffset.z();

	ikSolverLeftLeg->solve(position + Vector3f(0.1f, 0, 0), eulerToQuaternion(m_properties->rotationOffset));
	hierarchicSkeleton->leftFoot.setConfidence(Joint::JointConfidence::HIGH);

	//ikSolverRightLeg->middleUntwistWeight = m_properties->scaleOffset.x();
	//ikSolverRightLeg->upperUntwistWeight = m_properties->scaleOffset.y();
	//ikSolverRightLeg->DebugFloat1 = m_properties->scaleOffset.z();

	// TODO: Right untwisted nicht
	ikSolverRightLeg->solve(position - Vector3f(0.1f, 0, 0), eulerToQuaternion(m_properties->rotationOffset));
	hierarchicSkeleton->rightFoot.setConfidence(Joint::JointConfidence::HIGH);


	hierarchicSkeleton->insert(currSkeleton);
	return currSkeleton;

	//confidence values are not transmitted, default confidence is High
	Joint::JointConfidence medConf = Joint::JointConfidence::MEDIUM;
	Vector4f pos = Vector4f(0, 0, 0, 1);
	Quaternionf rot = Quaternionf::Identity();

	for (int i = 0; i < Joint::TOE_R; i++) {
		Joint::JointNames joint = Joint::JointNames(i);
		switch (joint) {
		case Joint::HEAD:
			break;

		default:
			currSkeleton->m_joints.insert({ joint, Joint(pos, rot, medConf) });
			break;
		}
	}

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
	ikSolverLeftArm->init();

	// Init RightArm IKSolver
	ikSolverRightArm = new IKSolverArm(&hierarchicSkeleton->rightShoulder, &hierarchicSkeleton->rightArm, &hierarchicSkeleton->rightForeArm, &hierarchicSkeleton->rightHand);
	ikSolverRightArm->init();
}

OpenVRTracking::DevicePose OVRTracker::getAssignedPose(Joint::JointNames joint) {

	return getAssignedPose(joint, true);
}

OpenVRTracking::DevicePose OVRTracker::getAssignedPose(Joint::JointNames joint, bool applyOffset) {

	// Init empty device pose
	auto pose = OpenVRTracking::DevicePose();

	// Try to get a pose from tracking system
	auto devicePose = trackingSystem->getPose(joint);

	// Apply transform if pose is not null
	if (devicePose != nullptr) {
		pose.position = devicePose->position;
		pose.rotation = devicePose->rotation;
	}
	// Return empty pose if null
	else {
		return pose;
	}

	// Return pose if offset is not requested
	if (!applyOffset) {
		return pose;
	}

	switch (joint) {
	case Joint::HIPS:
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
		break;

	// Try to get a offset from config
	auto offset = getOffset(joint);

	// Add offset if not null
	if (!offset.isNull()) {
		Console::logError(toString(offset.position));

		pose.position += pose.rotation * offset.position;
		pose.rotation *= offset.rotation;
	}

	// TODO: Offset zusammenrechnen und cachen
	// TODO: Offset in bezug auf Joint
	// TODO: Offset in bezug auf Device Class

	return pose;
}

OpenVRTracking::DevicePose OVRTracker::getOffset(Joint::JointNames joint) {

	// find offset for specific joint
	auto deviceOffsetIterator = jointToDeviceOffset.find(joint);

	// offset could be found -> return pose
	if (deviceOffsetIterator != jointToDeviceOffset.end()) {
		return deviceOffsetIterator->second;
	}

	return OpenVRTracking::DevicePose();
}

void OVRTracker::calibrate() {

	// Only calibrate if desired
	if (!shouldCalibrate) {
		return;
	}

	// Reset calibration flag
	shouldCalibrate = false;

	trackingSystem->calibrateDeviceRoles();
}

void OVRTracker::notify(Subject* subject) {

	// Cast subject to OpenVR button subject
	OpenVRButtonSubject* ovrButtonSubject = dynamic_cast<OpenVRButtonSubject*>(subject);

	// subject == OpenVRButtonSubject?
	if (ovrButtonSubject != nullptr) {
		Console::log("Is Trigger: " + toString(ovrButtonSubject->getButtonState() == vr::EVRButtonId::k_EButton_SteamVR_Trigger));

		// Calibrate on trigger pressed
		if (ovrButtonSubject->getButtonState() == vr::EVRButtonId::k_EButton_SteamVR_Trigger) {
			calibrate();
		}
	}

	//Console::log("updateObserver");
	//calibrate();
}

std::string OVRTracker::getTrackerType() {

	return "OpenVR";
}