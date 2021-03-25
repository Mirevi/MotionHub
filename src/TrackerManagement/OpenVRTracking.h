#pragma once

#include "TrackingSystem.h"
#include <openvr.h>

class OpenVRTracking : public TrackingSystem {
public:

	OpenVRTracking();

	~OpenVRTracking();

	void LoadDevices() override;

	void ReceiveDevicePoses() override;

	bool IsDeviceConnected(unsigned int deviceIndex) override;

	void SetPredictionTime(float secondsFromNow);

	TrackingError GetErrorDescriptor() override;

private:

	std::vector<TrackingSystem::Device> GetConnectedDevices();

	unsigned int trackedPoseCount;

	float predictSecondsFromNow;

	vr::IVRSystem* pVRSystem;

	vr::TrackedDevicePose_t* devicePoses;

	TrackingError trackingError;
};