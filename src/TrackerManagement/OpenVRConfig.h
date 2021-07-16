#pragma once

#include "MotionHubUtil/ConfigManager.h"
#include "MotionHubUtil/Joint.h"
#include "OpenVRTracking.h"


#include "MotionHubUtil/Console.h"

/*!
 * \class OpenVRTracking
 *
 * \brief Manages VR Tracking with the Open VR API
 */
class OpenVRConfig {

public:

	OpenVRConfig(ConfigManager* configManager, OpenVRTracking* trackingSystem);

	void read();

	void write();

	void writeDefaults();

	OpenVRTracking::DevicePose getOffset(Joint::JointNames joint);

private:

	std::string generateKey(OpenVRTracking::DeviceClass deviceClass, Joint::JointNames joint);

private:

	std::string trackerType = "OpenVR";

	ConfigManager* configManager = nullptr;

	OpenVRTracking* trackingSystem = nullptr;

	std::unordered_map<Joint::JointNames, OpenVRTracking::DevicePose> jointToDeviceOffset;
};