#include "OpenVRTracking.h"

#pragma region Helper
/*!
 * Converts JointName constructor to OpenVR input path (k_pchPathUser...) found in openvr_capi.v
 *
 * \param jointName Joint::JointNames
 * \return k_pchPathUser... found in openvr_capi.v
 */
static const char* ConvertJointNameToInputPath(Joint::JointNames jointName) {

	// Map JointName to k_pchPathUser variants found in openvr_capi.v
	switch (jointName) {
	case Joint::HIPS:
		return k_pchPathUserWaist;

	case Joint::CHEST:
		return k_pchPathUserChest;

	case Joint::SHOULDER_L:
		return k_pchPathUserShoulderLeft;

	case Joint::FOREARM_L:
		return k_pchPathUserElbowLeft;

	case Joint::HAND_L:
		return k_pchPathUserHandLeft;

	case Joint::SHOULDER_R:
		return k_pchPathUserShoulderRight;

	case Joint::FOREARM_R:
		return k_pchPathUserElbowRight;

	case Joint::HAND_R:
		return k_pchPathUserHandRight;

	case Joint::LEG_L:
		return k_pchPathUserKneeLeft;

	case Joint::FOOT_L:
		return k_pchPathUserFootLeft;

	case Joint::LEG_R:
		return k_pchPathUserKneeRight;

	case Joint::FOOT_R:
		return k_pchPathUserFootRight;

	case Joint::HEAD:
		return k_pchPathUserHead;

	default:
		return nullptr;
	}
}

/*!
 * Converts OpenVR input path to JointName
 *
 * \param inputPath (k_pchPathUser...) found in openvr_capi.v
 * \return Joint::JointNames
 */
static Joint::JointNames ConvertInputPathToJointName(const char* inputPath) {

	// Map k_pchPathUser variants found in openvr_capi.v to JointNames
	if (inputPath == k_pchPathUserWaist) {
		return Joint::HIPS;
	}
	else if (inputPath == k_pchPathUserChest) {
		return Joint::CHEST;
	}
	else if (inputPath == k_pchPathUserShoulderLeft) {
		return Joint::SHOULDER_L;
	}
	else if (inputPath == k_pchPathUserElbowLeft) {
		return Joint::FOREARM_L;
	}
	else if (inputPath == k_pchPathUserHandLeft) {
		return Joint::HAND_L;
	}
	else if (inputPath == k_pchPathUserShoulderRight) {
		return Joint::SHOULDER_R;
	}
	else if (inputPath == k_pchPathUserElbowRight) {
		return Joint::FOREARM_R;
	}
	else if (inputPath == k_pchPathUserHandRight) {
		return Joint::HAND_R;
	}
	else if (inputPath == k_pchPathUserKneeLeft) {
		return Joint::LEG_L;
	}
	else if (inputPath == k_pchPathUserFootLeft) {
		return Joint::FOOT_L;
	}
	else if (inputPath == k_pchPathUserKneeRight) {
		return Joint::LEG_R;
	}
	else if (inputPath == k_pchPathUserFootRight) {
		return Joint::FOOT_R;
	}
	else if (inputPath == k_pchPathUserHead) {
		return Joint::HEAD;
	}
}

/*!
 * Converts JointName constructor to OpenVR input path (k_pchPathUser...) found in openvr_capi.v
 *
 * \param jointName Joint::JointNames
 * \return k_pchPathUser... found in openvr_capi.v
 */
static int GetDeviceIndexForJoint(Joint::JointNames jointName) {

	// Convert JointName to input path
	const char* inputPath = ConvertJointNameToInputPath(jointName);

	// Error: input path is empty
	if (inputPath == NULL) {
		return -1;
	}

	// Retrieve handle for specified path in the input system
	vr::VRInputValueHandle_t inputHandle = vr::k_ulInvalidInputValueHandle;
	vr::EVRInputError inputError = vr::VRInput()->GetInputSourceHandle(inputPath, &inputHandle);

	// Error: handle for path could not be retrieved
	if (inputError != vr::EVRInputError::VRInputError_None) {
		return -2;
	}

	// Retrieve information for the origin the action
	vr::InputOriginInfo_t inputOriginInfo;
	inputError = vr::VRInput()->GetOriginTrackedDeviceInfo(inputHandle, &inputOriginInfo, sizeof(inputOriginInfo));

	// Error: InputOriginInfo could not be retrieved
	if (inputError != vr::EVRInputError::VRInputError_None) {
		//Console::logError("GetOriginTrackedDeviceInfo Error: " + toString((int)inputError) + " for " + inputPath);
		return -3;
	}
	// Return DeviceIndex from InputOriginInfo
	else {
		return inputOriginInfo.trackedDeviceIndex;
	}
}

static OpenVRTracking::DeviceClass ConvertDeviceClass(const vr::ETrackedDeviceClass& ovrDeviceClass) {

	OpenVRTracking::DeviceClass deviceClass = OpenVRTracking::DeviceClass::Invalid;

	switch (ovrDeviceClass) {
	case vr::ETrackedDeviceClass::TrackedDeviceClass_HMD:
		deviceClass = OpenVRTracking::DeviceClass::HMD;
		break;

	case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:
		deviceClass = OpenVRTracking::DeviceClass::Controller;
		break;

	case vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker:
		deviceClass = OpenVRTracking::DeviceClass::Tracker;
		break;
	}

	return deviceClass;
}

static std::string GetTrackedDevicePorpertyString(vr::IVRSystem* pVRSystem, const vr::TrackedDeviceIndex_t& deviceIndex, const vr::ETrackedDeviceProperty& property) {

	// Create string for return value
	std::string propertyString;

	// Init error for pointing property functions to it
	vr::ETrackedPropertyError error = vr::TrackedProp_Success;

	// Request property with length = 0 to get the required buffer length
	uint32_t requiredBufferLength = pVRSystem->GetStringTrackedDeviceProperty(deviceIndex, property, NULL, 0, &error);
	if (requiredBufferLength == 0 || error != vr::TrackedProp_BufferTooSmall) {
		return propertyString;
	}

	// Create char buffer with required length & request property
	char* pCharBuffer = new char[requiredBufferLength];
	pVRSystem->GetStringTrackedDeviceProperty(deviceIndex, property, pCharBuffer, requiredBufferLength, &error);

	// If successful: write buffer to string
	if (error == vr::TrackedProp_Success) {
		propertyString = pCharBuffer;
	}

	// Free memory
	delete[] pCharBuffer;

	return propertyString;
}

static Vector3f ConvertToPosition(const vr::HmdMatrix34_t& trackingMatrix) {

	return Vector3f(-trackingMatrix.m[0][3], trackingMatrix.m[1][3], -trackingMatrix.m[2][3]);
}

static Quaternionf ConvertToRotation(const vr::HmdMatrix34_t& trackingMatrix) {

	Quaternionf q = Quaternionf(
		sqrtf(fmaxf(0, 1 + trackingMatrix.m[0][0] + trackingMatrix.m[1][1] + trackingMatrix.m[2][2])) / 2,
		copysignf(
			sqrtf(fmaxf(0, 1 + trackingMatrix.m[0][0] - trackingMatrix.m[1][1] - trackingMatrix.m[2][2])) / 2,
			trackingMatrix.m[2][1] - trackingMatrix.m[1][2]
		),
		copysignf(
			sqrtf(fmaxf(0, 1 - trackingMatrix.m[0][0] + trackingMatrix.m[1][1] - trackingMatrix.m[2][2])) / 2,
			trackingMatrix.m[0][2] - trackingMatrix.m[2][0]
		),
		-copysignf(
			sqrtf(fmaxf(0, 1 - trackingMatrix.m[0][0] - trackingMatrix.m[1][1] + trackingMatrix.m[2][2])) / 2,
			trackingMatrix.m[1][0] - trackingMatrix.m[0][1]
		)
	);

	// TODO: inverse vermeiden und Rechnung umstellen
	return q.inverse();
}
#pragma endregion

OpenVRTracking::OpenVRTracking() :
	trackedPoseCount(0),
	predictSecondsFromNow(0)
{

}

OpenVRTracking::~OpenVRTracking() {

	// Free Memory
	if (devicePoses) {
		delete[] devicePoses;
	}

	// Shutdown VR System
	if (pVRSystem) {
		vr::VR_Shutdown();
		pVRSystem = NULL;
	}
}

void OpenVRTracking::init() {
	devicePoses = NULL;

	// Error handling for not installed VR Runtime
	if (!vr::VR_IsRuntimeInstalled()) {
		throw Exception("OpenVR Runtime not found. Please install OpenVR.");
	}

	// Error handling for disconnected HMD
	if (!vr::VR_IsHmdPresent()) {
		throw Exception("HMD is not present. Please connect a HMD.");
	}

	// Initialize VR System
	vr::EVRInitError initError = vr::VRInitError_None;
	pVRSystem = vr::VR_Init(&initError, vr::VRApplication_Background);

	// Error handling for VR initialization
	if (initError != vr::VRInitError_None) {
		pVRSystem = NULL;

		// Filter some error types to define better messages for the user
		switch (initError) {

		case vr::VRInitError_Init_NoServerForBackgroundApp:
			throw Exception("Compositor not found. Please start SteamVR.");
			break;
		}

		std::string errorCode = vr::VR_GetVRInitErrorAsSymbol(initError);
		std::string errorDescription = vr::VR_GetVRInitErrorAsEnglishDescription(initError);

		throw Exception(errorDescription + " (Error Code: " + errorCode + ")");
	}

	// Error handling for Compositor errors
	if (!vr::VRCompositor()) {
		pVRSystem = NULL;
		throw Exception("Compositor initialization failed. Please restart SteamVR.");
	}

	// ActionManifest not needed
	/*
	TCHAR binPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, binPath);

	std::string path = binPath; path += "\action_manifest.json";

	vr::EVRInputError inputError = vr::VRInput()->SetActionManifestPath(path.c_str());
	*/

	// Loop over all valid DeviceIndexes and try to add valid Devices
	for (vr::TrackedDeviceIndex_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++) {

		// Skip not connected Devices
		if (!isDeviceConnected(deviceIndex)) {
			continue;
		}

		// Try to add Device to Vector
		tryAddDevice(deviceIndex);
	}

	for (Device& device : Devices) {
		// Create Empty Pose and insert at back
		Poses.emplace_back();

		// Check for higher DeviceIndex
		if (device.Index >= trackedPoseCount) {
			trackedPoseCount = device.Index + 1;
		}
	}

	// Delete old Device Poses
	if (devicePoses) {
		delete[] devicePoses;
	}

	// Initialize OpenVR Pose Array for each device
	devicePoses = new vr::TrackedDevicePose_t[trackedPoseCount];

	updateDeviceRoles();
}

void OpenVRTracking::start() {

	// TODO: Bei start Poses & Devices aktualisieren?
	updateDeviceRoles();
}

// TODO: Methode wird durch init() ersetzt
void OpenVRTracking::loadDevices() {

	// Initialize Device Vector
	Devices = GetConnectedDevices();

	// Reserve a Pose for each Device
	Poses.reserve(Devices.size());

	// Initialize Poses Vector for each device
	// Find out highest DeviceIndex
	trackedPoseCount = 0;

	for (Device& device : Devices) {

		// Create Empty Pose and insert at back
		Poses.emplace_back();

		// Check for higher DeviceIndex
		if (device.Index >= trackedPoseCount) {
			trackedPoseCount = device.Index + 1;
		}
	}

	// Delete old Device Poses
	if (devicePoses) {
		delete[] devicePoses;
	}

	// Initialize OpenVR Pose Array for each device
	devicePoses = new vr::TrackedDevicePose_t[trackedPoseCount];

	updateDeviceRoles();
}

OpenVRTracking::Device* OpenVRTracking::getDevice(unsigned int deviceIndex) {

	for (int i = 0; i < Devices.size(); i++) {
		if (Devices[i].Index == deviceIndex) {
			return &Devices[i];
		}
	}

	return nullptr;
}

OpenVRTracking::DevicePose* OpenVRTracking::getPose(unsigned int deviceIndex) {

	for (int i = 0; i < Devices.size(); i++) {
		if (Devices[i].Index == deviceIndex) {
			return &Poses[i];
		}
	}

	return nullptr;
}

void OpenVRTracking::receiveDevicePoses() {

	float predictedSecondsFromNow = 0.0f;
	if (predictSecondsFromNow > 0.0f) {
		float secondsSinceLastVsync;
		pVRSystem->GetTimeSinceLastVsync(&secondsSinceLastVsync, NULL);

		float displayFrequency = pVRSystem->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
		float frameDuration = 1.f / displayFrequency;
		float vSyncToPhotons = pVRSystem->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SecondsFromVsyncToPhotons_Float);

		predictedSecondsFromNow = predictSecondsFromNow + frameDuration - secondsSinceLastVsync + vSyncToPhotons;
	}

	// Get predicted Tracking Pose and write to Pose Array
	pVRSystem->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, predictedSecondsFromNow, devicePoses, trackedPoseCount);

	// Convert Pose Array to MMH Format
	// for(int i = Devices.size(); i--; )
	for (int i = 0; i < Devices.size(); i++) {

		// Get tracked Pose Matrix in relation to DeviceIndex
		vr::TrackedDevicePose_t& trackedPose = devicePoses[Devices[i].Index];

		// TODO: Nur valide Posen übernehmen?
		/*if (trackedPose.bPoseIsValid) {

		}*/

		// Call copy constructor
		Poses[i].ExtractPose(trackedPose.mDeviceToAbsoluteTracking);

		/*
		// Convert Data to MMH Format
		OpenVRTracking::DevicePose& pose = Poses[i];
		pose.Valid = trackedPose.bPoseIsValid;
		pose.Position = ConvertToPosition(trackedPose.mDeviceToAbsoluteTracking);
		pose.Rotation = ConvertToRotation(trackedPose.mDeviceToAbsoluteTracking);
		*/
	}
}

bool OpenVRTracking::isDeviceConnected(unsigned int deviceIndex) {

	return pVRSystem->IsTrackedDeviceConnected(deviceIndex);
}

void OpenVRTracking::setPredictionTime(float secondsFromNow) {

	predictSecondsFromNow = secondsFromNow;
}

void OpenVRTracking::tryAddDevice(unsigned int deviceIndex) {

	// Skip already added Devices
	if (getDevice(deviceIndex) != nullptr) {
		return;
	}

	// Read OpenVR-Device specific Class and convert 
	vr::ETrackedDeviceClass ovrDeviceClass = pVRSystem->GetTrackedDeviceClass(deviceIndex);
	OpenVRTracking::DeviceClass deviceClass = ConvertDeviceClass(ovrDeviceClass);

	// Skip unwanted Devices
	if (deviceClass == OpenVRTracking::DeviceClass::Invalid) {
		return;
	}

	// Assign serial number as identifier
	std::string identifier = GetTrackedDevicePorpertyString(pVRSystem, deviceIndex, vr::ETrackedDeviceProperty::Prop_SerialNumber_String);

	// Add Device to Vector
	Devices.emplace_back(deviceIndex, deviceClass, identifier);

	// Update Role if Device is a Controller
	if (deviceClass == OpenVRTracking::DeviceClass::Controller) {
		//updateDeviceRole(deviceIndex);
	}
}

void OpenVRTracking::updateDeviceRoles() {

	Console::logWarning("OpenVRTracking::updateDeviceRoles()");

	// You have to call UpdateActionState to get all Device Roles
	vr::VRActiveActionSet_t actionSet = { 0 };
	vr::VRInput()->UpdateActionState(&actionSet, sizeof(actionSet), 1);

	std::map<unsigned int, Joint::JointNames> newIndexToJoint;

	std::map<Joint::JointNames, unsigned int> newJointToIndex;

	bool headFound = false;

	// Loop over all Joints in Joint::JointNames
	for (Joint::JointNames joint = Joint::HIPS; joint < Joint::HEAD; joint = Joint::JointNames(joint + 1)) {
		int deviceIndex = GetDeviceIndexForJoint(joint);

		if (deviceIndex >= 0) {

			newJointToIndex[joint] = deviceIndex;

			newIndexToJoint[deviceIndex] = joint;

			if (joint == Joint::HEAD) {
				headFound = true;
			}
		}
	}

	// Head could not be found -> Add Head to HMD Index
	if (!headFound) {
		for (const Device& device : Devices) {
			if (device.Class == DeviceClass::HMD) {
				newIndexToJoint[device.Index] = Joint::HEAD;
				break;
			}
		}
	}

	for (Device& device : Devices) {
		if (newIndexToJoint.count(device.Index) == 0) {
			continue;
		}

		Joint::JointNames joint = newIndexToJoint.at(device.Index);
		Console::log("Device #" + toString((int)device.Index) + " " + getDeviceClassType(device.Class) + " " + device.Identifier + " = " + Joint::getJointName(joint));
	}
}

std::vector<OpenVRTracking::Device> OpenVRTracking::GetConnectedDevices() {

	std::vector<OpenVRTracking::Device> trackedDeviceVector;

	// Reserve Memory for already known Devices
	trackedDeviceVector.reserve(trackedPoseCount);

	for (vr::TrackedDeviceIndex_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++) {

		// Read OpenVR-Device specific Class and convert 
		vr::ETrackedDeviceClass ovrDeviceClass = pVRSystem->GetTrackedDeviceClass(deviceIndex);
		OpenVRTracking::DeviceClass deviceClass = ConvertDeviceClass(ovrDeviceClass);

		// Skip unwanted Devices
		if (deviceClass == OpenVRTracking::DeviceClass::Invalid) {
			continue;
		}

		std::string identifier = GetTrackedDevicePorpertyString(pVRSystem, deviceIndex, vr::ETrackedDeviceProperty::Prop_SerialNumber_String);
		trackedDeviceVector.emplace_back(deviceIndex, deviceClass, identifier);
	}

	return trackedDeviceVector;
}

void OpenVRTracking::pollEvents() {

	vr::VREvent_t event;

	// Poll all Events from Queue
	while (pVRSystem->PollNextEvent(&event, sizeof(event))) {

		Console::log("VREvent (" + toString((int)event.eventType) + "): " + pVRSystem->GetEventTypeNameFromEnum((vr::EVREventType)event.eventType));

		switch (event.eventType) {

			// Device activated or updated
		case vr::EVREventType::VREvent_TrackedDeviceActivated:
		case vr::EVREventType::VREvent_TrackedDeviceUpdated:

			Console::logWarning("event.trackedDeviceIndex: " + toString((int)event.trackedDeviceIndex));

			//updateDevice(event.trackedDeviceIndex);
			break;

			// Device deactivated
		case vr::EVREventType::VREvent_TrackedDeviceDeactivated:
			Console::logWarning("event.trackedDeviceIndex: " + toString((int)event.trackedDeviceIndex));

			break;

			// Tracker
		case vr::EVREventType::VREvent_TrackersSectionSettingChanged:
			// Update all Devices
			updateDeviceRoles();
			break;

			// Device Role changed
		case vr::EVREventType::VREvent_TrackedDeviceRoleChanged:
			//prop = GetTrackedDevicePorpertyString(pVRSystem, event.trackedDeviceIndex, vr::ETrackedDeviceProperty::Prop_InputProfilePath_String);
			//Console::log("Prop_InputProfilePath_String: " + prop);

			// Update all Devices
			updateDeviceRoles();

			//updateDeviceRole(event.trackedDeviceIndex);
			break;

			// Button pressed
		case vr::EVREventType::VREvent_ButtonPress:

			if (vr::EVRButtonId::k_EButton_SteamVR_Trigger == event.data.controller.button) {
				Console::log("Trigger PRESSED on DeviceIndex: " + toString((int)event.trackedDeviceIndex));
			}

			break;

			// Button unpressed
		case vr::EVREventType::VREvent_ButtonUnpress:

			break;
		}
	}
}



void OpenVRTracking::calibrateDeviceRoles() {
}