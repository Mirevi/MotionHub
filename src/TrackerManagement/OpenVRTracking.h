#pragma once

#include "MotionHubUtil/Exception.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Joint.h"

#include <openvr_capi.h>
#include <openvr.h>

#include <vector>
#include <algorithm> // Sort
#include <unordered_map>
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

		unsigned int index;
		DeviceClass deviceClass;
		// TODO: const char* ? string mem allocation > 15 chars
		std::string identifier;

		Joint::JointNames joint;

		Device()
			: index(0), deviceClass(DeviceClass::Invalid), identifier(""), joint(Joint::NDEF){
		}

		Device(unsigned int index, DeviceClass deviceClass, std::string identifier)
			: joint(Joint::NDEF) {

			this->index = index;
			this->deviceClass = deviceClass;
			this->identifier = identifier;
		}
	};

	// TODO: Pose auslagern? Und bei ReceiveDevicePoses übergeben?
	struct DevicePose {

		Vector3f position;
		Quaternionf rotation;
		// TODO: Valid nötig? -> Invalide Posen überspringen?
		//bool Valid;

		DevicePose() : 
			position(Vector3f::Zero()), 
			rotation(Quaternionf::Identity()) 
		{ }

		DevicePose(Vector3f position, Quaternionf rotation) {
			this->position = position;
			this->rotation = rotation;
		}

		void ExtractPose(const vr::HmdMatrix34_t& trackingMatrix) {
			// Extract Position
			position = Vector3f(-trackingMatrix.m[0][3], trackingMatrix.m[1][3], -trackingMatrix.m[2][3]);

			// Extract Rotation 
			rotation = Quaternionf(
				//q.w = sqrt(fmax(0, 1 + trackingMatrix.m[0][0] + trackingMatrix.m[1][1] + trackingMatrix.m[2][2])) / 2;
				- sqrt(fmax(0, 1 + trackingMatrix.m[0][0] + trackingMatrix.m[1][1] + trackingMatrix.m[2][2])) / 2,

				//q.x = sqrt(fmax(0, 1 + trackingMatrix.m[0][0] - trackingMatrix.m[1][1] - trackingMatrix.m[2][2])) / 2;
				//q.x = copysignf(q.x, trackingMatrix.m[2][1] - trackingMatrix.m[1][2]);
				copysignf(
					sqrt(fmax(0, 1 + trackingMatrix.m[0][0] - trackingMatrix.m[1][1] - trackingMatrix.m[2][2])) / 2,
					trackingMatrix.m[2][1] - trackingMatrix.m[1][2]
				),

				//q.y = sqrt(fmax(0, 1 - trackingMatrix.m[0][0] + trackingMatrix.m[1][1] - trackingMatrix.m[2][2])) / 2;
				//q.y = copysignf(q.y, trackingMatrix.m[0][2] - trackingMatrix.m[2][0]);
				- copysignf(
					sqrt(fmax(0, 1 - trackingMatrix.m[0][0] + trackingMatrix.m[1][1] - trackingMatrix.m[2][2])) / 2,
					trackingMatrix.m[0][2] - trackingMatrix.m[2][0]
				),

				//q.z = sqrt(fmax(0, 1 - trackingMatrix.m[0][0] - trackingMatrix.m[1][1] + trackingMatrix.m[2][2])) / 2;
				//q.z = copysignf(q.z, trackingMatrix.m[1][0] - trackingMatrix.m[0][1]);
				copysignf(
					sqrt(fmax(0, 1 - trackingMatrix.m[0][0] - trackingMatrix.m[1][1] + trackingMatrix.m[2][2])) / 2,
					trackingMatrix.m[1][0] - trackingMatrix.m[0][1]
				)
			);
		}
	};

public:

	OpenVRTracking();

	~OpenVRTracking();

	void init();

	void start();

	Device* getDevice(unsigned int deviceIndex);

	void removeDevice(unsigned int deviceIndex);

	DevicePose* getPose(unsigned int deviceIndex);

	DevicePose* getPose(Joint::JointNames joint);

	void receiveDevicePoses();

	bool isDeviceConnected(unsigned int deviceIndex);

	void setPredictionTime(float secondsFromNow);

	void pollEvents();

	void calibrateDeviceRoles();

	static std::string getDeviceClassType(DeviceClass deviceClass) {
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

public:

	std::vector<Device> Devices;

	std::vector<DevicePose> Poses;

private:

	void tryAddDevice(unsigned int deviceIndex);

	void updateDevices();

	void updateDeviceRoles();

	std::vector<Device> GetConnectedDevices();

	unsigned int trackedPoseCount;

	float predictSecondsFromNow;

	vr::IVRSystem* pVRSystem;

	vr::TrackedDevicePose_t* devicePoses;

	std::unordered_map<unsigned int, Joint::JointNames> userIndexToJoint;

	std::unordered_map<Joint::JointNames, int> jointToDevice;
};