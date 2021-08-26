#pragma once

#include "MotionHubUtil/ConfigManager.h"
#include "MotionHubUtil/HierarchicSkeleton.h"
#include "OpenVRTracking.h"


#include "MotionHubUtil/Console.h"

/*!
 * \class OpenVRConfig
 *
 * \brief Manages OpenVR specific config values
 */
class OpenVRConfig {

public:

	/*!
	 * Constructor with given ConfigManager & OpenVRTracking
	 *
	 * \param configManager pointer to the ConfigManager instance
	 * \param trackingSystem pointer to the OpenVRTracking instance
	 */
	OpenVRConfig(ConfigManager* configManager, OpenVRTracking* trackingSystem);

	/*!
	 * Reads saved device:joint assignments from config
	 */
	void readAssignedDevices();
	
	/*!
	 * Reads saved offsets from config
	 */
	void readOffsets();

	/*!
	 * Writes offsets & device:joint assignments to config
	 */
	void write();

	void writeScale(Vector3f scale);

	Vector3f readScale();

	/*!
	 * Writes default offsets & device:joint assignments to config
	 */
	void writeDefaults();

	int getDeviceIndex(Joint::JointNames joint);

	Joint::JointNames getJoint(unsigned int deviceIndex);

	OpenVRTracking::DevicePose* getPose(Joint::JointNames joint);

	OpenVRTracking::DevicePose getPoseWithOffset(Joint::JointNames joint);

	/*!
	 * Returns offset to a assigned joint
	 *
	 * \param joint the assigned joint
	 * \return a DevicePose containing the offset position & rotation
	 */
	OpenVRTracking::DevicePose getOffset(Joint::JointNames joint);

	Quaternionf getSpace(Joint::JointNames joint);

	void setSpace(Joint::JointNames joint, Quaternionf space);

	void setOffsetPosition(Joint::JointNames joint, Vector3f position);

	void setOffsetRotation(Joint::JointNames joint, Quaternionf rotation);

	void assignJointToDevice(Joint::JointNames joint, unsigned int deviceIndex);

	void clearJointToDevice();

	void updateUserDeviceRoles();

	void calibrateDeviceToJointOffsets();

	void calibrateDeviceToJointOffset(Joint::JointNames jointName);

	Vector3f getCalibratedScale(HierarchicSkeleton* hierarchicSkeleton);

private:

	/*!
	 * Returns a config key for a DeviceClass & Joint
	 *
	 * \param deviceClass the DeviceClass
	 * \param joint the Joint
	 * \return DeviceClass:JointName
	 */
	std::string generateKey(OpenVRTracking::DeviceClass deviceClass, Joint::JointNames joint);

private:

	std::string trackerType = "OpenVR";

	ConfigManager* configManager = nullptr;

	OpenVRTracking* trackingSystem = nullptr;

	std::unordered_map<Joint::JointNames, unsigned int> jointToDevice;

	std::unordered_map<unsigned int, Joint::JointNames> deviceToJoint;

	std::unordered_map<Joint::JointNames, OpenVRTracking::DevicePose> jointToDeviceOffset;

	std::unordered_map<Joint::JointNames, Quaternionf> jointToSpace;
};