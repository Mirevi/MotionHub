#include "CLTracker.h"

bool CLTracker::m_validTrackerExists;

CLTracker::CLTracker(int id, NetworkManager* networkManager, ConfigManager* configManager) {
	// check if another CLTracker already exists. If yes throw exception
	if (m_validTrackerExists) {
		throw Exception("Another CapturyLive Tracker was already created. Only one should be active at any time.");
	}
	m_validTrackerExists = true;

	// assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_CapturyLive_" + std::to_string(id);

	Property<std::string>* ip = new Property<std::string>("IP address", "127.0.0.1");
	m_properties->additionalProperties.insert({ "ipAddress", ip });
	m_ipAddress = &(ip->value);

	Property<int>* port = new Property<int>("Port", 2101);
	m_properties->additionalProperties.insert({ "port", port });
	m_port = &(port->value);

	m_networkManager = networkManager;
	m_configManager = configManager;

	// default is enabled
	m_isEnabled = true;

	// read property values from config
	readOffsetFromConfig();
	m_configManager->readString("ipAddress", *m_ipAddress, getTrackerType());
	m_configManager->readInt("port", *m_port, getTrackerType());
}

CLTracker::~CLTracker() {
	if (m_isTracking) stop();
	m_validTrackerExists = false;
}

void CLTracker::start() {
	// connect to CapturyLive
	connect();

	// start tracking in a new thread
	Tracker::start();
}

void CLTracker::stop() {
	// is not tracking, so the update loop exits 
	Tracker::stop();

	// disconnect from CapturyLive
	disconnect();
}

void CLTracker::track() {
	if (m_connected) {
		CapturyActor* actors = nullptr;
		m_countDetectedSkeleton = Captury_getActors((const CapturyActor**) &actors);

		m_skeletonPoolLock.lock();
		for (int i = 0; i < m_countDetectedSkeleton; i++) {
			CapturyPose* pose = Captury_getCurrentPose(actors[i].id);

			if (pose != nullptr) {
				m_hasSkeletonPoolChanged = m_hasSkeletonPoolChanged || m_skeletonPool.emplace(actors[i].id, actors[i].id).second;
				parseSkeleton(&m_skeletonPool[actors[i].id], &actors[i], pose);
			}

			Captury_freePose(pose);
		}
		// clean skeleton pool
		for (auto it = m_skeletonPool.begin(); it != m_skeletonPool.end();) {
			bool foundActor = false;
			for (int i = 0; i < m_countDetectedSkeleton; i++) {
				if (it->first == actors[i].id) {
					foundActor = true;
					++it;
					break;
				}
			}
			if (!foundActor) {
				m_skeletonPool.erase(it++);
				m_hasSkeletonPoolChanged = true;
			}
		}
		m_skeletonPoolLock.unlock();

		// increase tracking cycle counter
		m_trackingCycles++;

		// new data is ready
		m_isDataAvailable = true;
	}
}

bool CLTracker::connect() {
	m_connected = Captury_connect(m_ipAddress->c_str(), (unsigned short) *m_port) && Captury_startStreaming(CAPTURY_STREAM_POSES);
	return m_connected;
}

bool CLTracker::disconnect() {
	m_connected = !(Captury_stopStreaming() && Captury_disconnect());
	return !m_connected;
}

void CLTracker::parseSkeleton(Skeleton* skeleton, CapturyActor* actor, CapturyPose* pose) {
	for (int i = 0; i < actor->numJoints; i++) {
		std::pair<Joint::JointNames, bool> parsedName = parseJointName(&actor->joints[i]);
		if (parsedName.second) 
		{
			skeleton->m_joints[parsedName.first] = Joint(convertCapturyPosition(&pose->transforms[i]), convertCapturyEuler(&pose->transforms[i]), Joint::JointConfidence::HIGH);
		}
	}
}

std::pair<Joint::JointNames, bool> CLTracker::parseJointName(const CapturyJoint* joint) {
	std::string name = std::string(joint->name);
	if (name == "Head")
		return { Joint::JointNames::HEAD, true };
	//else if (name == "HeadEE")
	else if (name == "Neck")
		return { Joint::JointNames::NECK, true };
	else if (name == "Spine")
		return { Joint::JointNames::CHEST, true };
	//else if (name == "Spine3")
	else if (name == "Spine2")
		return { Joint::JointNames::SPINE, true };
	//else if (name == "Spine1")
	else if (name == "Spine4")
		return { Joint::JointNames::HIPS, true };
	//else if (name == "Root")
	else if (name == "RightShoulder")
		return { Joint::JointNames::SHOULDER_R, true };
	else if (name == "RightArm")
		return { Joint::JointNames::ARM_R, true };
	else if (name == "RightForeArm")
		return { Joint::JointNames::FOREARM_R, true };
	else if (name == "RightHand")
		return { Joint::JointNames::HAND_R, true };
	//else if (name == "RightHandEE")
	else if (name == "LeftShoulder")
		return { Joint::JointNames::SHOULDER_L, true };
	else if (name == "LeftArm")
		return { Joint::JointNames::ARM_L, true };
	else if (name == "LeftForeArm")
		return { Joint::JointNames::FOREARM_L, true };
	else if (name == "LeftHand")
		return { Joint::JointNames::HAND_L, true };
	//else if (name == "LeftHandEE")
	else if (name == "RightUpLeg")
		return { Joint::JointNames::UPLEG_R, true };
	else if (name == "RightLeg")
		return { Joint::JointNames::LEG_R, true };
	else if (name == "RightFoot")
		return { Joint::JointNames::FOOT_R, true };
	else if (name == "RightToeBase")
		return { Joint::JointNames::TOE_R, true };
	//else if (name == "RightFootEE")
	else if (name == "LeftUpLeg")
		return { Joint::JointNames::UPLEG_L, true };
	else if (name == "LeftLeg")
		return { Joint::JointNames::LEG_L, true };
	else if (name == "LeftFoot")
		return { Joint::JointNames::FOOT_L, true };
	else if (name == "LeftToeBase")
		return { Joint::JointNames::TOE_L, true };
	//else if (name == "LeftFootEE")
	else return { Joint::JointNames::HEAD, false };
}

Vector4f CLTracker::getCapturyJointPosition(CapturyJoint* joint, CapturyActor* actor) {
	Vector4f pos = Vector4f(joint->offset[0] * .001f, joint->offset[1] * .001f, joint->offset[2] * .001f, 1);
	if (joint->parent >= 0) pos += getCapturyJointPosition(&actor->joints[joint->parent], actor);
	return pos;
}

Quaternionf CLTracker::getCapturyJointRotation(CapturyJoint* joint, CapturyActor* actor) {
	return Quaternionf(sqrt(1 - joint->orientation[0] * joint->orientation[0] - joint->orientation[1] * joint->orientation[1] - joint->orientation[2] * joint->orientation[2]), joint->orientation[0], joint->orientation[1], joint->orientation[2]);
}

Vector4f CLTracker::convertCapturyPosition(const CapturyTransform* transform) {
	return applyOffset(Vector4f(transform->translation[0] * .001f, transform->translation[1] * .001f, transform->translation[2] * .001f, 1));
}

Quaternionf CLTracker::convertCapturyEuler(const CapturyTransform* transform) {
	return applyOffset(AngleAxisf(transform->rotation[2] * M_PI / 180, Vector3f::UnitZ()) * AngleAxisf(transform->rotation[1] * M_PI / 180, Vector3f::UnitY()) * AngleAxisf(transform->rotation[0] * M_PI / 180, Vector3f::UnitX()));
}


std::string CLTracker::getTrackerType()
{
	return "CapturyLive";
}