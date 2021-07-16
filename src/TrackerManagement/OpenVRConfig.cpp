#include "OpenVRConfig.h"

OpenVRConfig::OpenVRConfig(ConfigManager* configManager, OpenVRTracking* trackingSystem) {

	this->configManager = configManager;
	this->trackingSystem = trackingSystem;
}

void OpenVRConfig::read() {

	// Init out var for reading the config
	std::string jointName;
	Vector3f offsetPosition;
	Vector3f offsetEuler;

	// Loop over all devices
	for (const auto& device : trackingSystem->Devices) {

		// Try to read joint config. if not found -> skip device
		if (!configManager->readString("joint", jointName, trackerType, device.identifier)) {
			continue;
		}

		// get joint from joint name
		int joint = Joint::toJointNames(jointName);

		// Skip if joint could not be found
		if (joint == Joint::NDEF) {
			continue;
		}

		// assign joint to device
		trackingSystem->assignJointToDevice(joint, device.index);
		Console::logError(Joint::toString(device.joint));

		// Create key: device & joint
		std::string configKey = generateKey(device.deviceClass, device.joint);

		// Read offset values by device & joint, reset if they not exist
		if (!configManager->readVec3f("position", offsetPosition, trackerType, configKey)) {
			offsetPosition = Vector3f::Zero();
		}
		if (!configManager->readVec3f("rotation", offsetEuler, trackerType, configKey)) {
			offsetEuler = Vector3f::Zero();
		}

		// Add offset if position or rotation is not zero
		if (!offsetPosition.isApprox(Vector3f::Zero()) || !offsetEuler.isApprox(Vector3f::Zero())) {
			Console::logWarning(toString(offsetPosition));

			jointToDeviceOffset[device.joint] = OpenVRTracking::DevicePose(offsetPosition, eulerToQuaternion(offsetEuler));
		}
	}
}

void OpenVRConfig::write() {
	// TODO: Config leeren?

	// Loop over all devices
	for (const auto& device : trackingSystem->Devices) {

		// Skip if device is unassigned
		if (device.joint == Joint::NDEF) {
			continue;
		}

		// Identifier to Joint
		configManager->writeString("joint", Joint::toString(device.joint), trackerType, device.identifier);

		// get offset from assigned Joint
		const auto offset = getOffset(device.joint);

		// Create key: device & joint
		std::string configKey = generateKey(device.deviceClass, device.joint);

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

	return OpenVRTracking::DevicePose();
}

void OpenVRConfig::writeDefaults() {

	std::string configKey;

	// Write HMD data if not exist
	configKey = generateKey(OpenVRTracking::HMD, Joint::HEAD);
	if (1 == 1 || !configManager->exists(trackerType, configKey)) {

		// offset
		configManager->writeVec3f("position", Vector3f(0, -0.14f, -0.14f), trackerType, configKey);
	}
}

std::string OpenVRConfig::generateKey(OpenVRTracking::DeviceClass deviceClass, Joint::JointNames joint) {

	return trackingSystem->getDeviceClassType(deviceClass) + "_" + Joint::toString(joint);
}
