#pragma once

#include <vector>
#include <Dense>

#include <openvr.h>

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
		bool Valid;

		DevicePose()
			: Position(Vector3f::Zero()), Rotation(Quaternionf::Identity()), Valid(false) {
		}

		DevicePose(Vector3f Position, Quaternionf Rotation) {
			this->Position = Position;
			this->Rotation = Rotation;
		}
	};

	struct TrackingError {
		std::string Code;
		std::string Description;
	};

	bool Valid = true;

	std::vector<Device> Devices;

	std::vector<DevicePose> Poses;

	OpenVRTracking();

	~OpenVRTracking();

	void LoadDevices();

	void ReceiveDevicePoses();

	bool IsDeviceConnected(unsigned int deviceIndex);

	void SetPredictionTime(float secondsFromNow);

	TrackingError GetErrorDescriptor();

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

	std::vector<Device> GetConnectedDevices();

	unsigned int trackedPoseCount;

	float predictSecondsFromNow;

	vr::IVRSystem* pVRSystem;

	vr::TrackedDevicePose_t* devicePoses;

	TrackingError trackingError;
};