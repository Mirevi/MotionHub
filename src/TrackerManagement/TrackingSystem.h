#pragma once
#include <vector>
#include <Dense>

using namespace Eigen;

class TrackingSystem {
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

		// TODO: Direkt Vector4f verwenden? Warum nutzt MMH Vector4f?
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

public:

	bool Valid = true;

	std::vector<Device> Devices;

	std::vector<DevicePose> Poses;

public:

	TrackingSystem() {

	}

	// Allow Subclasses to  be destructed in polymorphism
	virtual ~TrackingSystem() {

	}

	virtual void LoadDevices() = 0;

	virtual void ReceiveDevicePoses() = 0;

	virtual bool IsDeviceConnected(unsigned int deviceIndex) = 0;

	virtual TrackingError GetErrorDescriptor() = 0;

public:

	static std::string GetDeviceClassType(DeviceClass deviceClass) {
		std::string type = "Invalid";

		switch (deviceClass) {
		case TrackingSystem::HMD:
			type = "HMD";
			break;

		case TrackingSystem::Controller:
			type = "Controller";
			break;

		case TrackingSystem::Tracker:
			type = "Tracker";
			break;
		}

		return type;
	}
};