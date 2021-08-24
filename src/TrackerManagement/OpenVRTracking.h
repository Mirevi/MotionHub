#pragma once

#include "MotionHubUtil/Exception.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Joint.h"
#include "OpenVRButtonSubject.h"

#include <openvr_capi.h>
#include <openvr.h>

#include <vector>
#include <algorithm> // Sort
#include <unordered_map>
#include <Dense>

using namespace Eigen;

/*!
 * \class OpenVRTracking
 *
 * \brief Manages VR Tracking with the Open VR API
 */
class OpenVRTracking {

public:

	// enum for the various devices
	enum DeviceClass {
		Invalid,
		HMD,
		Controller,
		Tracker
	};

	/*!
	 * \struct Device
	 *
	 * \brief Container for tracking devices
	 */
	struct Device {

		// represents the device index from the Open VR API
		unsigned int index;

		// unnique identifier (e.g. serial number)
		std::string identifier;

		DeviceClass deviceClass;

		Device()
			: index(0), deviceClass(DeviceClass::Invalid), identifier("") {
		}

		Device(unsigned int index, DeviceClass deviceClass, std::string identifier) {

			this->index = index;
			this->deviceClass = deviceClass;
			this->identifier = identifier;
		}
	};


	/*!
	 * \struct DevicePose
	 *
	 * \brief Container for tracking poses
	 */
	struct DevicePose {

		Vector3f position;

		Quaternionf rotation;

		DevicePose() : 
			position(Vector3f::Zero()), 
			rotation(Quaternionf::Identity()) 
		{ }

		DevicePose(Vector3f position, Quaternionf rotation) {
			this->position = position;
			this->rotation = rotation;
		}

		void extractPose(const vr::HmdMatrix34_t& trackingMatrix) {
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

		bool isNull() {
			return isPositionNull() && isRotationNull();
		}

		bool isPositionNull() {
			return position.isApprox(Vector3f::Zero());
		}

		bool isRotationNull() {
			return rotation.isApprox(Quaternionf::Identity());
		}
	};

public:

	/*!
	 * default constructor
	 */
	OpenVRTracking();

	/*!
	 * destructor
	 */
	~OpenVRTracking();

	/*!
	 * initializes the VR System
	 */
	void init();

	/*!
	 * refreshes the devices vector & starts the tracking
	 */
	void start();

	/*!
	 * returns a pointer to device with a specific device index
	 *
	 * \param deviceIndex the device index
	 * 
	 * \return pointer to a device, null if index was not found
	 */
	Device* getDevice(unsigned int deviceIndex);

	/*!
	 * removes a device from the collection by device index
	 *
	 * \param deviceIndex the device index
	 */
	void removeDevice(unsigned int deviceIndex);

	/*!
	 * returns a pointer to pose from a specific device by device index
	 *
	 * \param deviceIndex the device index
	 *
	 * \return pointer to a pose, null if index was not found
	 */
	DevicePose* getPose(unsigned int deviceIndex);

	/*!
	 * updates the devices poses from the VR System
	 */
	void receiveDevicePoses();

	/*!
	 * chekcs if the device is currently connected
	 * 
	 * \param deviceIndex the device index
	 *
	 * \return true if connected, false if not
	 */
	bool isDeviceConnected(unsigned int deviceIndex);

	/*!
	 * sets the prediction time of the VR System
	 * 
	 * \param secondsFromNow the prediction time in seconds
	 */
	void setPredictionTime(float secondsFromNow);

	/*!
	 * polls all events from the VR System
	 */
	void pollEvents();

	/*!
	 * Assigns device roles within a T-Pose
	 */
	std::unordered_map<unsigned int, Joint::JointNames> getCalibratedDeviceRoles();

	int readDeviceIndexForJoint(Joint::JointNames jointName);

	/*!
	* Register an observer for button presses
	*
	* \param observer the observer object to be registered
	*/
	void registerButtonPressObserver(Observer* observer) {
		ovrButtonSubject.registerObserver(observer);
	}

	/*!
	* Unregister an observer for button presses
	*
	* \param observer the observer object to be unregistered
	*/
	void removeButtonPressObserver(Observer* observer) {
		ovrButtonSubject.removeObserver(observer);
	}

	/*!
	 * Converts a device class to string
	 */
	static std::string toString(DeviceClass deviceClass) {
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

	// collection for Devices
	std::vector<Device> Devices;

	// collection for Devices transforms
	std::vector<DevicePose> Poses;

private:

	void tryAddDevice(unsigned int deviceIndex);

	void updateDevices();

	std::vector<Device> getConnectedDevices();

	unsigned int trackedPoseCount;

	float predictSecondsFromNow;

	vr::IVRSystem* pVRSystem;

	vr::TrackedDevicePose_t* devicePoses;

	std::unordered_map<unsigned int, DevicePose*> deviceToPose;

	OpenVRButtonSubject ovrButtonSubject;
};