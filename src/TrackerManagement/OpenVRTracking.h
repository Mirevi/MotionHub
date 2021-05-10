#pragma once

#include "MotionHubUtil/Exception.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Joint.h"

#include <openvr_capi.h>
#include <openvr.h>

#include <vector>
#include <map>
#include <Dense>

using namespace Eigen;

class OpenVRTracking {
public:

	enum DeviceClass {
		Invalid,
		HMD,
		Controller,
		Tracker
	};

	struct Device {

		unsigned int Index;
		DeviceClass Class;
		// TODO: const char* ? string mem allocation > 15 chars
		std::string Identifier;

		Device()
			: Index(0), Class(DeviceClass::Invalid), Identifier("") {
		}

		Device(unsigned int index, DeviceClass deviceClass, std::string identifier) {
			this->Index = index;
			this->Class = deviceClass;
			this->Identifier = identifier;
		}
	};

	// TODO: Pose auslagern? Und bei ReceiveDevicePoses übergeben?
	struct DevicePose {

		Vector3f Position;
		Quaternionf Rotation;
		// TODO: Valid nötig? -> Invalide Posen überspringen?
		//bool Valid;

		DevicePose() : 
			Position(Vector3f::Zero()), 
			Rotation(Quaternionf::Identity()) 
		{ }

		DevicePose(Vector3f Position, Quaternionf Rotation) {
			this->Position = Position;
			this->Rotation = Rotation;
		}

		void ExtractPose(const vr::HmdMatrix34_t& trackingMatrix) {
			// Extract Position
			Position = Vector3f(-trackingMatrix.m[0][3], trackingMatrix.m[1][3], -trackingMatrix.m[2][3]);

			// Extract Rotation 
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
			Rotation = q.inverse();
		}
	};

	bool Valid = true;

	std::vector<Device> Devices;

	std::vector<DevicePose> Poses;

	OpenVRTracking();

	~OpenVRTracking();

	void init();

	void start();

	void LoadDevices();

	Device* GetDevice(unsigned int deviceIndex);

	void ReceiveDevicePoses();

	bool IsDeviceConnected(unsigned int deviceIndex);

	void SetPredictionTime(float secondsFromNow);

	void PollEvents();

public:

	static std::string GetDeviceClassType(DeviceClass deviceClass) {
		std::string type = "Invalid";

		switch (deviceClass) {
		case DeviceClass::HMD:
			type = "HMD";
			break;

		case DeviceClass::Controller:
			type = "Controller";
			break;

		case DeviceClass::Tracker:
			type = "Tracker";
			break;
		}

		return type;
	}
private:

	void tryAddDevice(unsigned int deviceIndex);

	void updateDeviceRoles();

	std::vector<Device> GetConnectedDevices();

	unsigned int trackedPoseCount;

	float predictSecondsFromNow;

	vr::IVRSystem* pVRSystem;

	vr::TrackedDevicePose_t* devicePoses;

	std::map<unsigned int, Joint::JointNames> userIndexToJoint;
};