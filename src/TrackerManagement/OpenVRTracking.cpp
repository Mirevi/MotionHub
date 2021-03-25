#include "OpenVRTracking.h"

#pragma region Helper
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

	vr::ETrackedPropertyError error = vr::TrackedProp_Success;

	uint32_t requiredBufferLength = pVRSystem->GetStringTrackedDeviceProperty(deviceIndex, property, NULL, 0, &error);
	if (requiredBufferLength == 0 || error != vr::TrackedProp_Success) {
		return "";
	}

	char* pCharBuffer = new char[requiredBufferLength];
	pVRSystem->GetStringTrackedDeviceProperty(deviceIndex, property, pCharBuffer, requiredBufferLength, &error);

	std::string propertyString;
	if (error == vr::TrackedProp_Success) {
		propertyString = pCharBuffer;
	}
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

	// Initialize VR System
	vr::EVRInitError eError = vr::VRInitError_None;
	pVRSystem = vr::VR_Init(&eError, vr::VRApplication_Background);

	// Errorhandling for VR initialization
	if (eError != vr::VRInitError_None) {
		trackingError.Code = vr::VR_GetVRInitErrorAsSymbol(eError);
		trackingError.Description = vr::VR_GetVRInitErrorAsEnglishDescription(eError);

		pVRSystem = NULL;

		Valid = false;
		return;
	}

	devicePoses = NULL;

	// TODO: Compositor nötig? Weil kein Input abgefragt wird?
	/*
	// Errorhandling for VR Compositor initialization
	if (!vr::VRCompositor()) {
		trackingError.Code = "VRCompError_InitFailed";
		trackingError.Description = "Compositor initialization failed";

		valid = false;
		return;
	}
	*/

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

void OpenVRTracking::LoadDevices() {

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
}

void OpenVRTracking::ReceiveDevicePoses() {

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
		// Convert Data to MMH Format
		OpenVRTracking::DevicePose& pose = Poses[i];
		pose.Valid = trackedPose.bPoseIsValid;
		pose.Position = ConvertToPosition(trackedPose.mDeviceToAbsoluteTracking);
		pose.Rotation = ConvertToRotation(trackedPose.mDeviceToAbsoluteTracking);
	}
}

bool OpenVRTracking::IsDeviceConnected(unsigned int deviceIndex) {
	return pVRSystem->IsTrackedDeviceConnected(deviceIndex);
}

void OpenVRTracking::SetPredictionTime(float secondsFromNow) {
	predictSecondsFromNow = secondsFromNow;
}

std::vector<TrackingSystem::Device> OpenVRTracking::GetConnectedDevices() {

	std::vector<TrackingSystem::Device> trackedDeviceVector;

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

TrackingSystem::TrackingError OpenVRTracking::GetErrorDescriptor() {
	return trackingError;
}
