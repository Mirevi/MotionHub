#include "OVRTracker.h"

OVRTracker::OVRTracker() {
	// Initialize OpenVR
	init();
}

OVRTracker::OVRTracker(int id, NetworkManager* networkManager, ConfigManager* configManager) {
	
	m_networkManager = networkManager;
	m_configManager = configManager;

	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_openVR_" + toString(id);

	//default is enabled
	m_isEnabled = true;

	// Initialize OpenVR
	init();
}

OVRTracker::~OVRTracker() {

}

void OVRTracker::start() {

	// set tracking to true
	m_isTracking = true;

	//there schould only be one skeleton in the file
	m_countDetectedSkeleton = m_skeletonPool.size();

	//skeleton was added/removed, so UI updates
	m_hasSkeletonPoolChanged = true;

	// TODO: Init & LINK IK Sekelton


	// start tracking thread and detach the thread from method scope runtime
	m_trackingThread = new std::thread(&OVRTracker::update, this);
	m_trackingThread->detach();
}

void OVRTracker::stop() {

	//is not tracking, so the update loop exits after current loop
	m_isTracking = false;

}

void OVRTracker::init() {

	trackingSystem.init();

	// Errorhandling when Trackingsystem could not be initialized
	/*if (!trackingSystem.Valid) {
		OpenVRTracking::TrackingError error = trackingSystem.GetErrorDescriptor();

		Console::logError(error.Description + " [" + error.Code + "]");

		valid = false;
		return;
	}*/

	// Load connected Tracking Devices
	//trackingSystem.LoadDevices();
}

void OVRTracker::update() {

	// track while tracking is true
	while (m_isTracking) {
		// get new data
		track();

		//send Skeleton Pool to NetworkManager
		m_networkManager->sendSkeletonPool(&getSkeletonPoolCache(), m_properties->id);
	}

	//clean skeleton pool after tracking
	clean();

	//skeleton was added/removed, so UI updates
	m_hasSkeletonPoolChanged = true;
}

void OVRTracker::track() {

	float mmhDelay = 0.0f; // 0.05f;
	trackingSystem.SetPredictionTime(mmhDelay);

	trackingSystem.ReceiveDevicePoses();

	//ikSolver.solve(trackingSystem.Poses[0].Position, trackingSystem.Poses[0].Rotation);

	extractSkeleton();

	m_isDataAvailable = true;
}

void OVRTracker::extractSkeleton() {

	//true as long new skeleton will be added to the pool
	bool createNewSkeleton = true;

	m_skeletonPoolLock.lock();

	int id = m_properties->id;

	//loop through all MMH skeletons
	for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++) {

		//when skeletons have the same ID, the skeleton is alredy in pool and no new skeleton has to be created
		//if (skData.skeletonID == itPoolSkeletons->first)
		if (id == itPoolSkeletons->first) {

			Skeleton* currSkeleton = parseSkeleton(id, &m_skeletonPool[id]);

			if (currSkeleton != nullptr) {
				// update all joints of existing skeleon with new data
				m_skeletonPool[id].m_joints = currSkeleton->m_joints;
			}

			//delete temp skeleton object 
			delete currSkeleton;

			// no new skeleton has to be created
			createNewSkeleton = false;

			break;
		}
	}

	// create new skeleton
	if (createNewSkeleton) {

		m_skeletonPool.insert({ id,  *parseSkeleton(id, NULL) });

		//skeleton was added, so UI updates
		m_hasSkeletonPoolChanged = true;

		Console::log("DataHandlerManager::extractSkeleton(): Created new skeleton with id = " + std::to_string(id));
	}

	m_skeletonPoolLock.unlock();

}

Skeleton* OVRTracker::parseSkeleton(int id, Skeleton* oldSkeletonData) {
	// skeleton data container
	Skeleton* currSkeleton = new Skeleton(id);

	if (trackingSystem.Poses.size() > 1) {
		Vector3f v3pos = trackingSystem.Poses[0].Position;
		Vector4f pos = Vector4f(v3pos.x(), v3pos.y(), v3pos.z(), 1.0f);

		Quaternionf offset = eulerToQuaternion(Vector3f(0.0f, 90.0f, 0.0f));
		Quaternionf rot = trackingSystem.Poses[0].Rotation * offset;

		currSkeleton->m_joints.insert({ Joint::HEAD, Joint(pos, rot, Joint::JointConfidence::HIGH) });
	}

	//confidence values are not transmitted, default confidence is High
	Joint::JointConfidence medConf = Joint::JointConfidence::MEDIUM;
	Vector4f pos = Vector4f(0, 0, 0, 1);
	Quaternionf rot = Quaternionf::Identity();

	for (int i = 0; i < Joint::TOE_R; i++) {
		Joint::JointNames joint = Joint::JointNames(i);
		switch (joint) {
		case Joint::HEAD:
			break;

		default:
			currSkeleton->m_joints.insert({ joint, Joint(pos, rot, medConf) });
			break;
		}
	}

	return currSkeleton;
}

std::string OVRTracker::getTrackerType() {
	return "OpenVR";
}