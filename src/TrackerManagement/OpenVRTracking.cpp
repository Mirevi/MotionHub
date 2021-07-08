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

#pragma region DeviceRoleAssigner

struct DeviceRoleAssigner {

	DeviceRoleAssigner(OpenVRTracking* trackingSystem) {

		this->trackingSystem = trackingSystem;
	}

	void assignWithinDistance(float allowedDistanceToCenter) {

		// Sum all positions from all device poses
		Vector3f centerPosition = calculateCenterPosition();

		// Init vectors for storing distances with pose count
		int poseCount = trackingSystem->Poses.size();
		distancesToCenter = std::vector<float>(poseCount, -1);
		distancesToHead = std::vector<float>(poseCount, -1);

		// Init vector for sorting devices
		std::vector<unsigned int> sortedDeviceIndexes;
		
		// Store distances to mean & sort out invalid devices
		for (int i = 0; i < trackingSystem->Devices.size(); i++) {

			float distanceToCenter = distance(centerPosition, trackingSystem->Poses[i].position);

			// is in allowed distance?
			if (distanceToCenter < allowedDistanceToCenter) {
				distancesToCenter[i] = distanceToCenter;

				//unsigned int deviceIndex = trackingSystem->Devices[i].Index;
				sortedDeviceIndexes.push_back(i);
			}
		}

		// Find and assign head 
		assignHead(sortedDeviceIndexes);

		// Store distances to head
		for (unsigned int& deviceIndex : sortedDeviceIndexes) {

			float distanceToHead = distance(headPosition, trackingSystem->Poses[deviceIndex].position);
			
			distancesToHead[deviceIndex] = distanceToHead;
		}

		// Sort remaining devices by distance to head descending
		std::sort(sortedDeviceIndexes.begin(), sortedDeviceIndexes.end()
			, [this](unsigned int a, unsigned int b) {

				return distancesToHead[a] > distancesToHead[b];
		});

		// Find and assign both feet
		assignFeet(sortedDeviceIndexes);

		// Sort remaining devices by distance to center descending
		std::sort(sortedDeviceIndexes.begin(), sortedDeviceIndexes.end()
			, [this](unsigned int a, unsigned int b) {

			return distancesToCenter[a] > distancesToCenter[b];
		});

		// Find and assign both hands
		assignHands(sortedDeviceIndexes);

		// Hip = closest point to center
		if (sortedDeviceIndexes.size() > 0) {
			int hipIndex = sortedDeviceIndexes[sortedDeviceIndexes.size() - 1];

			// Remove hip indexes from sorted devices
			removeFromVector(sortedDeviceIndexes, hipIndex);

			// assign hip joint to device
			assignRoleToDevice(Joint::HIPS, hipIndex);

			Console::log("Hip:" + trackingSystem->Devices[hipIndex].identifier);
		}

		// Assign remainign devices
		assignRemaining(sortedDeviceIndexes);
	}

private:

	Vector3f calculateCenterPosition() {
		// Sum all positions from all device poses
		Vector3f centerPosition = Vector3f::Zero();
		for (const OpenVRTracking::DevicePose& pose : trackingSystem->Poses) {
			centerPosition += pose.position;
		}

		// Divide by Pose count 
		int poseCount = trackingSystem->Poses.size();
		return Vector3f(centerPosition.x() / poseCount, centerPosition.y() / poseCount, centerPosition.z() / poseCount);
	}

	void assignRoleToDevice(Joint::JointNames role, unsigned int deviceIndex) {

		trackingSystem->Devices[deviceIndex].joint = role;
	}

	void removeFromVector(std::vector<unsigned int>& trargetVector, unsigned int value) {

		trargetVector.erase(std::remove(trargetVector.begin(), trargetVector.end(), value), trargetVector.end());
	}

	void assignHead(std::vector<unsigned int>& sortedDeviceIndexes) {

		// Enough devices left?
		if (sortedDeviceIndexes.size() < 1) {
			Console::logWarning("assignHead: sortedDevices.size() < 1");
			return;
		}

		// Search for HeadIndex == highest y position in T-Pose
		unsigned int headIndex = sortedDeviceIndexes[0];
		for (unsigned int& deviceIndex : sortedDeviceIndexes) {

			// Is device position higher than stored device?
			if (trackingSystem->Poses[deviceIndex].position.y() > trackingSystem->Poses[headIndex].position.y()) {
				headIndex = deviceIndex;
			}
		}

		// Remove head index from sorted devices
		removeFromVector(sortedDeviceIndexes, headIndex);

		// assign head joint to device
		assignRoleToDevice(Joint::HEAD, headIndex);

		// Store head position & forward +
		const auto& headPose = trackingSystem->Poses[headIndex];
		headPosition = headPose.position;
		headForward = headPose.rotation * Vector3f(0, 0, 1);

		Console::log("Head:" + trackingSystem->Devices[headIndex].identifier);
	}

	void assignFeet(std::vector<unsigned int>& sortedDeviceIndexes) {

		// Enough devices left?
		if (sortedDeviceIndexes.size() < 2) {
			Console::logWarning("assignFeet: sortedDeviceIndexes.size() < 2");
			return;
		}

		// Farest devices from head are feet
		unsigned int leftFootIndex = sortedDeviceIndexes[0];
		unsigned int rightFootIndex = sortedDeviceIndexes[1];

		// Remove feet indexes from sorted devices
		removeFromVector(sortedDeviceIndexes, leftFootIndex);
		removeFromVector(sortedDeviceIndexes, rightFootIndex);

		// Add feet indexes to limbs
		limbDevices.push_back(leftFootIndex);
		limbDevices.push_back(rightFootIndex);

		// Calculate point between feet
		const auto& leftFootPose = trackingSystem->Poses[leftFootIndex];
		const auto& rightFootPose = trackingSystem->Poses[rightFootIndex];
		betweenFeet = lerp(leftFootPose.position, rightFootPose.position, 0.5f);

		// Generate up axis from between feet to head
		upAxis = (headPosition - betweenFeet).normalized();

		// Ortho normalize head forward on up axis
		orthoNormalize(upAxis, headForward);

		// Create vectors from point between feet to feet
		Vector3f centerToLeftFoot = (leftFootPose.position - betweenFeet);
		Vector3f centerToRightFoot = (rightFootPose.position - betweenFeet);

		// ortho normalize both feet vectors on up axis
		orthoNormalize(upAxis, centerToLeftFoot);
		orthoNormalize(upAxis, centerToRightFoot);

		// Calculate angles between feet vectors and head forward
		float leftFootAngle = signedAngle(centerToLeftFoot, headForward, upAxis);
		float rightFootAngle = signedAngle(centerToRightFoot, headForward, upAxis);

		// Swap indexes if angle is negative
		if ((leftFootAngle - rightFootAngle) < 0) {
			float tempIndex = leftFootIndex;
			leftFootIndex = rightFootIndex;
			rightFootIndex = tempIndex;
		}

		// assign joints to devices
		assignRoleToDevice(Joint::FOOT_L, leftFootIndex);
		assignRoleToDevice(Joint::FOOT_R, rightFootIndex);

		Console::log("Left Foot:" + trackingSystem->Devices[leftFootIndex].identifier);
		Console::log("Right Foot:" + trackingSystem->Devices[rightFootIndex].identifier);
	}

	void assignHands(std::vector<unsigned int>& sortedDeviceIndexes) {
		
		// Enough devices left?
		if (sortedDeviceIndexes.size() < 2) {
			Console::logWarning("assignHands: sortedDeviceIndexes.size() < 2");
			return;
		}

		// Farest devices from ceare are hands
		unsigned int leftHandIndex = sortedDeviceIndexes[0];
		unsigned int rightHandIndex = sortedDeviceIndexes[1];

		// Remove feet indexes from sorted devices
		removeFromVector(sortedDeviceIndexes, leftHandIndex);
		removeFromVector(sortedDeviceIndexes, rightHandIndex);

		// Add feet indexes to limbs
		limbDevices.push_back(leftHandIndex);
		limbDevices.push_back(rightHandIndex);

		// Create vectors from point between feet to hands
		Vector3f centerToLeftHand = (trackingSystem->Poses[leftHandIndex].position - betweenFeet);
		Vector3f centerToRightHand = (trackingSystem->Poses[rightHandIndex].position - betweenFeet);

		// ortho normalize both hand vectors on up axis
		orthoNormalize(upAxis, centerToLeftHand);
		orthoNormalize(upAxis, centerToRightHand);

		// Calculate angles between hand vectors and head forward
		float leftHandAngle = signedAngle(centerToLeftHand, headForward, upAxis);
		float rightHandAngle = signedAngle(centerToRightHand, headForward, upAxis);

		// Swap indexes if angle is negative
		if ((leftHandAngle - rightHandAngle) < 0) {
			float tempIndex = leftHandIndex;
			leftHandIndex = rightHandIndex;
			rightHandIndex = tempIndex;
		}

		// assign joints to devices
		assignRoleToDevice(Joint::HAND_L, leftHandIndex);
		assignRoleToDevice(Joint::HAND_R, rightHandIndex);

		Console::log("Left Hand:" + trackingSystem->Devices[leftHandIndex].identifier);
		Console::log("Right Hand:" + trackingSystem->Devices[rightHandIndex].identifier);
	}

	void assignRemaining(std::vector<unsigned int>& sortedDeviceIndexes) {
		
		// Exit when no devices left
		if (sortedDeviceIndexes.size() == 0) {
			return;
		}

		// Init vector to store assigned limb indexes
		std::vector<unsigned int> assignedLimbIndexes;

		// Loop over remaining devices and find closest limb device
		for (unsigned int& deviceIndex : sortedDeviceIndexes) {

			// Init distance & limb index
			float minDistance = FLT_MAX;
			int assignedLimbIndex = 0;

			// Loop over limb devices and find closest one
			for (unsigned int& limbDeviceIndex : limbDevices) {

				// Calculate distance from current device to limb device
				float distanceToDevice = distance(trackingSystem->Poses[deviceIndex].position, trackingSystem->Poses[limbDeviceIndex].position);

				// Is current distance closer? Override index & min distance
				if (distanceToDevice < minDistance) {
					assignedLimbIndex = limbDeviceIndex;
					minDistance = distanceToDevice;
				}
			}

			// Add index to limb index vector
			assignedLimbIndexes.push_back(assignedLimbIndex);
		}

		// std::unique removes equivalent elements from vector
		std::vector<unsigned int>::iterator uniqueIterator = std::unique(assignedLimbIndexes.begin(), assignedLimbIndexes.end());

		// Is iterator unique?
		bool isUnique = (uniqueIterator == assignedLimbIndexes.end());

		// Has vector duplicates? -> reassign duplicates
		if (!isUnique) {

			bool limbDeviceAssigned = false;

			// Loop over remaining devices
			for (int x = 0; x < sortedDeviceIndexes.size(); x++) {

				// Break if device was already assigned
				if (limbDeviceAssigned) {
					break;
				}

				// Assign current device index
				unsigned int deviceIndex = sortedDeviceIndexes[x];

				// Assign current limb device index
				unsigned int limbDeviceIndex = assignedLimbIndexes[x];

				// Loop over remaining devices, starting with current index + 1
				for (int z = (x + 1); z < sortedDeviceIndexes.size(); z++) {

					// Break if device was already assigned
					if (limbDeviceAssigned) {
						break;
					}

					// Continue if both limb device indexes dont match
					if (limbDeviceIndex != assignedLimbIndexes[z]) {
						continue;
					}

					// Assign other device index
					unsigned int otherDeviceIndex = sortedDeviceIndexes[z];					

					// Init target joint with left foot
					Joint::JointNames targetJoint = Joint::FOOT_L;
					// Is limb device already left foot? -> switch to right foot
					if (trackingSystem->Devices[limbDeviceIndex].joint == targetJoint) {
						targetJoint = Joint::FOOT_R;
					}

					// Calculate distance from device & other device to limb
					float distanceToDevice = distance(trackingSystem->Poses[deviceIndex].position, trackingSystem->Poses[limbDeviceIndex].position);
					float otherDistanceToDevice = distance(trackingSystem->Poses[otherDeviceIndex].position, trackingSystem->Poses[limbDeviceIndex].position);

					// Init index for change with current device
					int changeIndex = x;

					// Current device is closer -> Change other device
					if (distanceToDevice < otherDistanceToDevice) {
						changeIndex = z;	
					}

					// Find limb with target joint and switch limb assignment
					for (unsigned int currentDeviceIndex = 0; currentDeviceIndex < trackingSystem->Devices.size(); currentDeviceIndex++) {
						if (trackingSystem->Devices[currentDeviceIndex].joint == targetJoint) {
							assignedLimbIndexes[changeIndex] = currentDeviceIndex;

							limbDeviceAssigned = true;
							break;
						}
					}
				}
			}
		}

		// Loop throug all assigned devices and assign them
		for (unsigned int i = 0; i < assignedLimbIndexes.size(); i++) {

			// Assigne device & limb indexes
			unsigned int deviceIndex = sortedDeviceIndexes[i];
			unsigned int limbIndex = assignedLimbIndexes[i];

			// Get current target joint from limb device index
			Joint::JointNames targetJoint = trackingSystem->Devices[limbIndex].joint;
			// and get previous joint in hierarchy
			int prevJointIndex = static_cast<int>(targetJoint) - 1;
			Joint::JointNames previousJoint = static_cast<Joint::JointNames>(prevJointIndex);

			// Assign previous joint to device
			assignRoleToDevice(previousJoint, deviceIndex);

			Console::log(std::string(Joint::getJointName(previousJoint)) + ": " + trackingSystem->Devices[deviceIndex].identifier);
		}
	}

private:

	OpenVRTracking* trackingSystem;

	std::vector<float> distancesToCenter;
	std::vector<float> distancesToHead;

	Vector3f betweenFeet;
	Vector3f upAxis;

	Vector3f headPosition;
	Vector3f headForward;

	std::vector<unsigned int> limbDevices;
};
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

	updateDevices();

	updateDeviceRoles();
}

void OpenVRTracking::start() {
	
	updateDevices();

	// TODO: Bei start Poses & Devices aktualisieren?
	updateDeviceRoles();
}

void OpenVRTracking::updateDevices() {

	// Loop over all valid DeviceIndexes and try to add valid Devices
	for (vr::TrackedDeviceIndex_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++) {

		// Skip/Remove not connected Devices
		if (!isDeviceConnected(deviceIndex)) {

			// Remove device if disconnected
			Device* device = getDevice(deviceIndex);
			if (device != nullptr) {
				removeDevice(deviceIndex);
			}

			// Skip device index
			continue;
		}

		// Try to add Device to Vector
		tryAddDevice(deviceIndex);
	}

	// Clear poses
	Poses.clear();

	// Loop over all Devices and create poses for each
	for (const auto& device : Devices) {
		// Create Empty Pose and insert at back
		Poses.emplace_back();

		// Check for higher DeviceIndex
		if (device.index >= trackedPoseCount) {
			trackedPoseCount = device.index + 1;
		}
	}

	// Delete old Device Poses
	if (devicePoses) {
		delete[] devicePoses;
	}

	// Initialize OpenVR Pose Array for each device
	devicePoses = new vr::TrackedDevicePose_t[trackedPoseCount];
}

OpenVRTracking::Device* OpenVRTracking::getDevice(unsigned int deviceIndex) {

	for (int i = 0; i < Devices.size(); i++) {
		if (Devices[i].index == deviceIndex) {
			return &Devices[i];
		}
	}

	return nullptr;
}

void OpenVRTracking::removeDevice(unsigned int deviceIndex) {
	for (int i = 0; i < Devices.size(); i++) {
		if (Devices[i].index == deviceIndex) {

			Devices.erase(Devices.begin() + i);
			break;
		}
	}
}

OpenVRTracking::DevicePose* OpenVRTracking::getPose(unsigned int deviceIndex) {

	for (int i = 0; i < Devices.size(); i++) {
		if (Devices[i].index == deviceIndex) {
			return &Poses[i];
		}
	}

	return nullptr;
}

OpenVRTracking::DevicePose* OpenVRTracking::getPose(Joint::JointNames joint) {
	auto deviceIndexIterator = jointToDevice.find(joint);

	if (deviceIndexIterator != jointToDevice.end()) {
		return &Poses[deviceIndexIterator->second];
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
		vr::TrackedDevicePose_t& trackedPose = devicePoses[Devices[i].index];

		// TODO: Nur valide Posen übernehmen?
		if (trackedPose.bPoseIsValid) {
			// Call copy constructor
			Poses[i].ExtractPose(trackedPose.mDeviceToAbsoluteTracking);
		}

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

	std::unordered_map<unsigned int, Joint::JointNames> newIndexToJoint;

	std::unordered_map<Joint::JointNames, unsigned int> newJointToIndex;

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
		for (const auto& device : Devices) {
			if (device.deviceClass == DeviceClass::HMD) {
				newIndexToJoint[device.index] = Joint::HEAD;
				break;
			}
		}
	}

	for (const auto& device : Devices) {
		if (newIndexToJoint.count(device.index) == 0) {
			continue;
		}

		Joint::JointNames joint = newIndexToJoint.at(device.index);
		Console::log("Device #" + toString((int)device.index) + " " + getDeviceClassType(device.deviceClass) + " " + device.identifier + " = " + Joint::getJointName(joint));
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

		// Assign serial number as identifier
		std::string identifier = GetTrackedDevicePorpertyString(pVRSystem, deviceIndex, vr::ETrackedDeviceProperty::Prop_SerialNumber_String);

		// Add Device to Vector
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

			// Set OpenVRButtonSubject states & notify Observers
			ovrButtonSubject.setDeviceIndexState(event.trackedDeviceIndex);
			ovrButtonSubject.setButtonState(static_cast<vr::EVRButtonId>(event.data.controller.button));
			ovrButtonSubject.notifyObservers();

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

	Poses[0].position = Vector3f(0, 1.76f, 0);

	Devices.push_back(Device(1, DeviceClass::Controller, "Left Controller"));
	Poses.push_back(DevicePose(Vector3f(-0.67f, 1.43f, 0), Quaternionf::Identity()));

	Devices.push_back(Device(3, DeviceClass::Controller, "Right Controller"));
	Poses.push_back(DevicePose(Vector3f(0.64f, 1.42f, 0), Quaternionf::Identity()));

	Devices.push_back(Device(4, DeviceClass::Tracker, "Hip"));
	Poses.push_back(DevicePose(Vector3f(0, 1, 0), Quaternionf::Identity()));

	Devices.push_back(Device(5, DeviceClass::Tracker, "Left Foot"));
	Poses.push_back(DevicePose(Vector3f(-0.08f, 0.1f, 0), Quaternionf::Identity()));

	Devices.push_back(Device(6, DeviceClass::Tracker, "Right Foot"));
	Poses.push_back(DevicePose(Vector3f(0.08f, 0.1f, 0), Quaternionf::Identity()));



	Devices.push_back(Device(8, DeviceClass::Tracker, "Right Leg"));
	Poses.push_back(DevicePose(Vector3f(0.1f, 0.5f, 0), Quaternionf::Identity()));

	Devices.push_back(Device(9, DeviceClass::Tracker, "Left Leg"));
	Poses.push_back(DevicePose(Vector3f(-0.09f, 0.51f, 0), Quaternionf::Identity()));

	Devices.push_back(Device(10, DeviceClass::Tracker, "Right Arm"));
	Poses.push_back(DevicePose(Vector3f(0.4f, 1.3f, 0), Quaternionf::Identity()));

	if (Devices.size() >= 6) {
		DeviceRoleAssigner* deviceRoleAssigner = new DeviceRoleAssigner(this);
		deviceRoleAssigner->assignWithinDistance(1.5f);

		delete deviceRoleAssigner;
	}
	else {
		Console::logError("Device Count < 6");
	}

	jointToDevice.clear();

	for (int i = 0; i < Devices.size(); i++) {
		if (Devices[i].joint != Joint::NDEF) {
			jointToDevice[Devices[i].joint] = i;
		}
	}

	Console::log("Calibrated");
}