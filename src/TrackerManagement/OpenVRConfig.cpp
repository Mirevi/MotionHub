#include "OpenVRConfig.h"

static bool OVERRIDE_DEFAULTS = false;

OpenVRConfig::OpenVRConfig(ConfigManager* configManager, OpenVRTracking* trackingSystem) {

	this->configManager = configManager;
	this->trackingSystem = trackingSystem;
}

void OpenVRConfig::readAssignedDevices() {

	// Init out var for reading the config
	std::string jointName;

	// Loop over all devices
	for (const auto& device : trackingSystem->Devices) {

		// Try to read joint config. if not found -> skip device
		if (!configManager->readString("joint", jointName, trackerType, device.identifier)) {
			continue;
		}

		// get joint from joint name
		Joint::JointNames joint = Joint::toJointNames(jointName);

		// Skip if joint could not be found
		if (joint == Joint::NDEF) {
			continue;
		}

		// assign joint to device
		assignJointToDevice(joint, device.index);
		Console::logError(Joint::toString(joint));
	}
}

void OpenVRConfig::readOffsets() {

	// Init out vars for reading the config
	Vector3f offsetPosition;
	Vector3f offsetEuler;
	Vector3f spaceEuler;

	// Loop over all devices
	for (const auto& device : trackingSystem->Devices) {

		// Read joint info
		Joint::JointNames joint = getJoint(device.index);

		// Skip if joint is not assigned
		if (joint == Joint::NDEF) {
			continue;
		}

		// Create key: device & joint
		std::string configKey = generateKey(device.deviceClass, joint);

		// Read & set offset position by device & joint
		if (configManager->readVec3f("position", offsetPosition, trackerType, configKey)) {
			setOffsetPosition(joint, offsetPosition);
		}

		// Read & set offset rotation by device & joint
		if (configManager->readVec3f("rotation", offsetEuler, trackerType, configKey)) {
			setOffsetRotation(joint, eulerToQuaternion(offsetEuler));
		}

		// Read & set space rotation by device & joint
		if (configManager->readVec3f("space", spaceEuler, trackerType, configKey)) {
			setSpace(joint, eulerToQuaternion(spaceEuler));
		}
	}
}

void OpenVRConfig::writeScale(Vector3f scale) {

	std::string configKey = generateKey(OpenVRTracking::Tracker, Joint::HIPS);
	configManager->writeVec3f("scale", scale, trackerType, configKey);
}

Vector3f OpenVRConfig::readScale() {

	Vector3f scale;

	std::string configKey = generateKey(OpenVRTracking::Tracker, Joint::HIPS);

	if (!configManager->readVec3f("scale", scale, trackerType, configKey)) {
		scale = Vector3f::Ones();
	};

	return scale;
}

void OpenVRConfig::write() {

	// TODO: Config leeren?

	// Loop over all devices
	for (const auto& device : trackingSystem->Devices) {

		// Try to read joint from config
		Joint::JointNames joint = getJoint(device.index);

		// Skip if device is unassigned
		if (joint == Joint::NDEF) {
			continue;
		}

		// Identifier to Joint
		configManager->writeString("joint", Joint::toString(joint), trackerType, device.identifier);

		// get offset from assigned Joint
		const auto offset = getOffset(joint);

		// Create key: device & joint
		std::string configKey = generateKey(device.deviceClass, joint);

		// Device & joint to offset position
		if (!offset.position.isApprox(Vector3f::Zero())) {
			configManager->writeVec3f("position", offset.position, trackerType, configKey);
		}

		// Device & joint to offset rotation
		if (!offset.rotation.isApprox(Quaternionf::Identity())) {
			configManager->writeVec3f("rotation", quaternionToEuler(offset.rotation), trackerType, configKey);
		}

		// get space from assigned Joint
		const auto space = getSpace(joint);

		// Device & joint to space rotation
		if (!space.isApprox(Quaternionf::Identity())) {
			configManager->writeVec3f("space", quaternionToEuler(space), trackerType, configKey);
		}
	}
}

OpenVRTracking::DevicePose OpenVRConfig::getOffset(Joint::JointNames joint) {

	// find offset for specific joint
	auto deviceOffsetIterator = jointToDeviceOffset.find(joint);

	// offset could be found -> return pose
	if (deviceOffsetIterator != jointToDeviceOffset.end()) {
		return deviceOffsetIterator->second;
	}

	// return empty pose if offset is not configured
	return OpenVRTracking::DevicePose();
}

Quaternionf OpenVRConfig::getSpace(Joint::JointNames joint) {

	// find space for specific joint
	auto deviceSpaceIterator = jointToSpace.find(joint);

	// space could be found -> return pose
	if (deviceSpaceIterator != jointToSpace.end()) {
		return deviceSpaceIterator->second;
	}

	// return identity if space is not configured
	return Quaternionf::Identity();
}

void OpenVRConfig::setSpace(Joint::JointNames joint, Quaternionf space) {

	// override device space in config
	jointToSpace[joint] = space;
}


void OpenVRConfig::setOffsetPosition(Joint::JointNames joint, Vector3f position) {

	// get device offset from config
	auto deviceOffset = getOffset(joint);

	// override position
	deviceOffset.position = position;

	// override device offset in config
	jointToDeviceOffset[joint] = deviceOffset;
}

void OpenVRConfig::setOffsetRotation(Joint::JointNames joint, Quaternionf rotation) {

	// get device offset from config
	auto deviceOffset = getOffset(joint);

	// override rotation
	deviceOffset.rotation = rotation;

	// override device offset in config
	jointToDeviceOffset[joint] = deviceOffset;
}

void OpenVRConfig::assignJointToDevice(Joint::JointNames joint, unsigned int deviceIndex) {

	jointToDevice[joint] = deviceIndex;
	deviceToJoint[deviceIndex] = joint;
}

void OpenVRConfig::clearJointToDevice() {

	jointToDevice.clear();
	deviceToJoint.clear();
}

void OpenVRConfig::updateUserDeviceRoles() {

	// You have to call UpdateActionState to get all Device Roles
	vr::VRActiveActionSet_t actionSet = { 0 };
	vr::VRInput()->UpdateActionState(&actionSet, sizeof(actionSet), 1);

	// Init map for string joint to device assignments
	std::unordered_map<Joint::JointNames, unsigned int> newJointToDevice;

	// Init flag for head assignment
	bool headFound = false;

	// Loop over Joint::JointNames from HIPS to HEAD
	for (Joint::JointNames joint = Joint::HIPS; joint < Joint::HEAD; joint = Joint::JointNames(joint + 1)) {

		// Try to read device index from OpenVR Config
		int deviceIndex = trackingSystem->readDeviceIndexForJoint(joint);

		// Is valid device index
		if (deviceIndex >= 0) {

			// add joint:device assignment
			newJointToDevice[joint] = deviceIndex;

			// is joint Head? flag head assignment
			if (joint == Joint::HEAD) {
				headFound = true;
			}
		}
	}

	// Head could not be found -> Add Head to HMD index
	if (!headFound) {
		for (const auto& device : trackingSystem->Devices) {
			if (device.deviceClass == OpenVRTracking::HMD) {

				newJointToDevice[Joint::HEAD] = device.index;
				break;
			}
		}
	}

	// TODO: Hand not found, but Controller? 

	// Loop over all new joint:device assignments
	for (const auto& jointDevicePair : newJointToDevice) {

		// Is joint not assigned? -> Add assignment
		//if (jointToDevice.count(jointDevicePair.first) == 0 && deviceToJoint.count(jointDevicePair.second) == 0) {
		if (jointToDevice.count(jointDevicePair.first) == 0) {
			assignJointToDevice(jointDevicePair.first, jointDevicePair.second);
		}
	}
}

void OpenVRConfig::calibrateDeviceToJointOffsets() {

	// Loop over all configured joint:device pairs
	for (const auto& jointDevicePair : jointToDevice) {

		// Read joint from first element in pair
		Joint::JointNames joint = jointDevicePair.first;

		// Skip head if device is HMD
		switch (joint) {
		case Joint::HEAD:
			if (trackingSystem->getDevice(getDeviceIndex(joint))->deviceClass == OpenVRTracking::HMD) {
				continue;
			}
			break;
		}

		// Calibrate offset for joint
		calibrateDeviceToJointOffset(joint);
	}
}

void OpenVRConfig::calibrateDeviceToJointOffset(Joint::JointNames joint) {

	// Try to get hip & requested joint pose
	auto hipDevicePose = getPose(Joint::HIPS);
	auto jointDevicePose = getPose(joint);

	// Exit if hip pose is null
	if (hipDevicePose == nullptr) {
		Console::logError("OpenVRConfig::calibrateDeviceToJointOffset: HIP Device not found");
		return;
	}

	// Exit if joint pose is null
	if (jointDevicePose == nullptr) {
		Console::logError("OpenVRConfig::calibrateDeviceToJointOffset: JOINT Device not found");
		return;
	}

	// create forward axis from hip pose rotation & -forward vector
	Vector3f forward = hipDevicePose->rotation * Vector3f(0, 0, -1);

	// create world up vector
	Vector3f worldUp = Vector3f(0, 1, 0);

	// project foward vector on world up
	forward = projectOnPlane(forward, worldUp);

	// hip special case: forward must be flipped
	if (joint == Joint::HIPS) {
		//forward = -forward;
	}

	// create look rotation
	Quaternionf rotation = lookRotation(forward, worldUp);

	// set offset = rotation * inverse device rotation
	Quaternionf offset = rotation * jointDevicePose->rotation.inverse();
	setOffsetRotation(joint, offset);

	// set space = rotation in device space
	Quaternionf space = rotationToSpace(jointDevicePose->rotation, rotation);
	setSpace(joint, space);
}

Vector3f OpenVRConfig::getCalibratedScale(HierarchicSkeleton* hierarchicSkeleton) {

	// Read scale from 
	Vector3f scale = hierarchicSkeleton->getScale();

	auto headPose = getPoseWithOffset(Joint::HEAD);
	if (headPose.isPositionNull()) {
		return scale;
	}

	auto leftHandPose = getPoseWithOffset(Joint::HAND_L);
	auto rightHandPose = getPoseWithOffset(Joint::HAND_R);
	if (leftHandPose.isPositionNull() || rightHandPose.isPositionNull()) {
		return scale;
	}

	float yDelta = headPose.position.y() / hierarchicSkeleton->head.getGlobalPosition().y();

	float deviceDistance = distance(leftHandPose.position, rightHandPose.position);
	float handDistance = distance(hierarchicSkeleton->leftHand.getGlobalPosition(), hierarchicSkeleton->rightHand.getGlobalPosition());

	float xDelta = deviceDistance / handDistance;

	scale.y() *= yDelta;
	scale.x() *= xDelta;
	scale.z() = fminf(scale.x(), scale.y());

	return scale;
}

void OpenVRConfig::writeDefaults() {

	std::string configKey;

	Console::log("write default");

	// Write HMD:HEAD data if not exist
	configKey = generateKey(OpenVRTracking::HMD, Joint::HEAD);
	if (OVERRIDE_DEFAULTS || !configManager->exists(trackerType, configKey)) {

		configManager->writeVec3f("position", Vector3f(0, -0.14f, -0.14f), trackerType, configKey);
	}

	// Write Controller:HAND_L data if not exist
	configKey = generateKey(OpenVRTracking::Controller, Joint::HAND_L);
	if (OVERRIDE_DEFAULTS || !configManager->exists(trackerType, configKey)) {

		configManager->writeVec3f("position", Vector3f(0, 0.01f, -0.15f), trackerType, configKey);
		// TODO: rotation offset prüfen
		//onfigManager->writeVec3f("rotation", Vector3f(90, 0, 0), trackerType, configKey);
	}

	// Write Controller:HAND_R data if not exist
	configKey = generateKey(OpenVRTracking::Controller, Joint::HAND_R);
	if (OVERRIDE_DEFAULTS || !configManager->exists(trackerType, configKey)) {

		configManager->writeVec3f("position", Vector3f(0, 0.01f, -0.15f), trackerType, configKey);
		//configManager->writeVec3f("rotation", Vector3f(120.0f, 165.0f, -95.0f), trackerType, configKey);
	}

	// Write Tracker:HIPS data if not exist
	configKey = generateKey(OpenVRTracking::Tracker, Joint::HIPS);
	if (OVERRIDE_DEFAULTS || !configManager->exists(trackerType, configKey)) {

		configManager->writeVec3f("position", Vector3f(0, -0.03f, -0.1f), trackerType, configKey);
		// TODO: rotation offset prüfen
		//onfigManager->writeVec3f("rotation", Vector3f(90, 0, 0), trackerType, configKey);
	}

	// Write Tracker:FOOT_L data if not exist
	configKey = generateKey(OpenVRTracking::Tracker, Joint::FOOT_L);
	if (OVERRIDE_DEFAULTS || !configManager->exists(trackerType, configKey)) {

		configManager->writeVec3f("position", Vector3f(0.008f, -0.036f, -0.016f), trackerType, configKey);
		// TODO: rotation offset pruefen
		//configManager->writeVec3f("rotation", Vector3f(90.0f, 155.0f, -55.0f), trackerType, configKey);
	}

	// Write Tracker:FOOT_R data if not exist
	configKey = generateKey(OpenVRTracking::Tracker, Joint::FOOT_R);
	if (OVERRIDE_DEFAULTS || !configManager->exists(trackerType, configKey)) {

		configManager->writeVec3f("position", Vector3f(0.008f, -0.036f, -0.016f), trackerType, configKey);
		// TODO: rotation offset pruefen
		configManager->writeVec3f("rotation", Vector3f(90.0f, -155.0f, -55.0f), trackerType, configKey);
	}
}

int OpenVRConfig::getDeviceIndex(Joint::JointNames joint) {

	auto deviceIterator = jointToDevice.find(joint);
	if (deviceIterator != jointToDevice.end()) {
		return deviceIterator->second;
	}

	return -1;
}

Joint::JointNames OpenVRConfig::getJoint(unsigned int deviceIndex) {

	auto jointIterator = deviceToJoint.find(deviceIndex);
	if (jointIterator != deviceToJoint.end()) {
		return jointIterator->second;
	}

	return Joint::NDEF;
}

OpenVRTracking::DevicePose* OpenVRConfig::getPose(Joint::JointNames joint) {

	// Try to get a assigned device with requested joint
	int deviceIndex = getDeviceIndex(joint);

	// return pose if device index is valid
	if (deviceIndex >= 0) {
		trackingSystem->getPose(deviceIndex);
	}
	else {
		return nullptr;
	}
}

OpenVRTracking::DevicePose OpenVRConfig::getPoseWithOffset(Joint::JointNames joint) {

	// Init empty device pose
	auto pose = OpenVRTracking::DevicePose();

	// Get the pose from config & tracking system 
	auto devicePose = getPose(joint);

	// Return empty pose if not found
	if (devicePose == nullptr) {
		return pose;
	}

	// Apply device pose to pose
	if (devicePose != nullptr) {
		pose.position = devicePose->position;
		pose.rotation = devicePose->rotation;
	}

	// Try to get a offset from config
	auto offset = getOffset(joint);

	auto space = getSpace(joint);

	// Add offset position if not null
	if (!offset.isPositionNull()) {
		pose.position += pose.rotation * offset.position;
	}

	// Add offset rotation if not null
	if (!offset.isRotationNull()) {
		pose.rotation = offset.rotation * (space * pose.rotation * space.inverse());
	}

	return pose;
}

std::string OpenVRConfig::generateKey(OpenVRTracking::DeviceClass deviceClass, Joint::JointNames joint) {

	// return DeviceClass:JointName
	return OpenVRTracking::toString(deviceClass) + ":" + Joint::toString(joint);
}
