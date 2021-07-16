#include "OpenVRConfig.h"

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

		// Read offset values by device & joint, reset if they not exist
		if (!configManager->readVec3f("position", offsetPosition, trackerType, configKey)) {
			offsetPosition = Vector3f::Zero();
		}
		if (!configManager->readVec3f("rotation", offsetEuler, trackerType, configKey)) {
			offsetEuler = Vector3f::Zero();
		}

		// Add offset to map if position or rotation is not zero
		if (!offsetPosition.isApprox(Vector3f::Zero()) || !offsetEuler.isApprox(Vector3f::Zero())) {
			Console::logWarning(toString(offsetPosition));

			// Create & add DevicePose with offsets
			jointToDeviceOffset[joint] = OpenVRTracking::DevicePose(offsetPosition, eulerToQuaternion(offsetEuler));
		}
	}
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

void OpenVRConfig::assignJointToDevice(Joint::JointNames joint, unsigned int deviceIndex) {

	jointToDevice[joint] = deviceIndex;
	deviceToJoint[deviceIndex] = joint;

	// TODO: RAUS
	//trackingSystem->assignJointToDevice(joint, deviceIndex);
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

	// Loop over all new joint:device assignments
	for (const auto& jointDevicePair : newJointToDevice) {

		// Is joint & device not assigned? -> Add assignment
		if(jointToDevice.count(jointDevicePair.first) == 0 && deviceToJoint.count(jointDevicePair.second) == 0) {
			assignJointToDevice(jointDevicePair.first, jointDevicePair.second);
		}
	}
}

void OpenVRConfig::writeDefaults() {

	std::string configKey;

	Console::log("write default");

	// Write HMD:HEAD data if not exist
	configKey = generateKey(OpenVRTracking::HMD, Joint::HEAD);
	if (1 == 1 || !configManager->exists(trackerType, configKey)) {

		configManager->writeVec3f("position", Vector3f(0, -0.14f, -0.14f), trackerType, configKey);
	}

	// Write Controller:HAND_L data if not exist
	configKey = generateKey(OpenVRTracking::Controller, Joint::HAND_L);
	if (1 == 1 || !configManager->exists(trackerType, configKey)) {

		configManager->writeVec3f("position", Vector3f(0, 0, -0.16f), trackerType, configKey);
		// TODO: rotation offset prüfen
		configManager->writeVec3f("rotation", Vector3f(90, 0, 0), trackerType, configKey);
	}

	// Write Tracker:FOOT_L data if not exist
	configKey = generateKey(OpenVRTracking::Tracker, Joint::FOOT_L);
	if (1 == 1 || !configManager->exists(trackerType, configKey)) {

		configManager->writeVec3f("position", Vector3f(0.008f, -0.036f, -0.16f), trackerType, configKey);
		// TODO: rotation offset prüfen
		//configManager->writeVec3f("rotation", Vector3f(90, 0, 0), trackerType, configKey);
	}

	// Write Tracker:FOOT_R data if not exist
	configKey = generateKey(OpenVRTracking::Tracker, Joint::FOOT_R);
	if (1 == 1 || !configManager->exists(trackerType, configKey)) {

		configManager->writeVec3f("position", Vector3f(0.008f, -0.036f, -0.16f), trackerType, configKey);
		// TODO: rotation offset prüfen
		//configManager->writeVec3f("rotation", Vector3f(90, 0, 0), trackerType, configKey);
	}
}

int OpenVRConfig::getDeviceIndex(Joint::JointNames joint) {
	
	auto deviceIterator= jointToDevice.find(joint);
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

std::string OpenVRConfig::generateKey(OpenVRTracking::DeviceClass deviceClass, Joint::JointNames joint) {

	// return DeviceClass:JointName
	return OpenVRTracking::toString(deviceClass) + ":" + Joint::toString(joint);
}
