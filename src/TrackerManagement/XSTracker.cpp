#include "XSTracker.h"


// default constructor
XSTracker::XSTracker(int id, NetworkManager* networkManager, ConfigManager* configManager)
{
	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_xsens_" + std::to_string(id);

	m_networkManager = networkManager;
	m_configManager = configManager;

	//default is enabled
	m_isEnabled = true;

	//frame countdown for unused avatars, 3000 seems like a good number
	m_cleanSkeletonCountDown = 3000;


	//set default values for offsets
	//setPositionOffset(Vector3f(configManager->getFloatFromStartupConfig("xPosXsens"),
	//	configManager->getFloatFromStartupConfig("yPosXsens"),
	//	configManager->getFloatFromStartupConfig("zPosXsens")
	//));

	//setRotationOffset(Vector3f(configManager->getFloatFromStartupConfig("xRotXsens"),
	//	configManager->getFloatFromStartupConfig("yRotXsens"),
	//	configManager->getFloatFromStartupConfig("zRotXsens")
	//));

	//setScaleOffset(Vector3f(configManager->getFloatFromStartupConfig("xSclXsens"),
	//	configManager->getFloatFromStartupConfig("ySclXsens"),
	//	configManager->getFloatFromStartupConfig("zSclXsens")
	//));
	readOffsetFromConfig();


}

XSTracker::~XSTracker()
{



}

// start Xsens tracker
void XSTracker::start()
{
	// set tracking to true
	m_isTracking = true;

	// udp server details
	std::string hostDestinationAddress = "localhost";
	int port = 9763;

	m_UdpServer = new UdpServer(hostDestinationAddress, (uint16_t)port);

	// start tracking thread and detach the thread from method scope runtime
	m_trackingThread = new std::thread(&XSTracker::update, this);
	m_trackingThread->detach();

	// still has to be done: check if should record 
	// CHANGE HERE FOR RECORD
	//bool isRecordBvh = true;
	if (isRecordBvh) {
		//start bvhStream !! INCLUDE TIMESTAMP
		bvhWriters = new std::vector<XSBvhWriter*>();

		//setupBvhParameters();
		//m_recordingThread = new std::thread(&XSTracker::writeMotionDataInBvhStream, this);
	}

}

// stop tracking loop / thread
void XSTracker::stop()
{
	//is not tracking, so the update loop exits after current loop
	m_isTracking = false;


	// check for BVH Writers to finish before deleting udpServer
	int numBvhWriters = 0;
	if (isRecordBvh) {
		numBvhWriters = bvhWriters->size();
	}



	for (int i = 0; i < numBvhWriters; i++) {
		bvhWriters->at(i)->isRecording = false;
	}

	for (int i = 0; i < numBvhWriters; i++) {
		while (bvhWriters->at(i)->isWriting) {
			XsTime::msleep(50);
		}
	}

	delete(m_UdpServer);

	for (int i = 0; i < numBvhWriters; i++) {
		delete(bvhWriters->at(i));

	}
	if (isRecordBvh) {
		delete(bvhWriters);
	}


}

// shutdown and destroy xsens tracker
//void XSTracker::destroy()
//{
//	// delete this object
//	delete this;
//}


//void XSTracker::init()
//{
//	//no init code. The receiver is created on start, because it cannot be deactivated/paused
//}


// get new skeleton data and parse it into the default skeleton
void XSTracker::track()
{

	m_skeletonPoolLock.lock();

	m_quaternianDataWithId = m_UdpServer->getQuaternionDatagram();

	// check if data valid
	if (m_quaternianDataWithId != NULL && m_quaternianDataWithId->kinematics != NULL) {
		//get skeleton data from frame data
		extractSkeleton();
	}

	// frame countdown for avatars that are not longer present
	for (auto& entry : m_avatarList)
	{
		entry.second -= 1;
	}

	cleanSkeletonPool();

	/*!
	 * Important:
	 * 
	 * m_trackingCycles is overwritten for Rainer Schiller's master's thesis 
	 * change isFrameTimeNecessary to false if not needed
	 *
	 */

	bool isFrameTimeNecessary = true;

	if (isFrameTimeNecessary) {
		// overwrite cycle counter with Xsens frameTime for 
		m_trackingCycles = m_quaternianDataWithId->frameTime;
	}
	else {
		//increase tracking cycle counter
		m_trackingCycles++;
	}


	//new data is ready 
	m_isDataAvailable = true;

	m_skeletonPoolLock.unlock();

}

void XSTracker::extractSkeleton()
{

	int avatarID = m_quaternianDataWithId->avatarId;
	

	// add avatarID to avatar list
	m_avatarList[avatarID] = m_cleanSkeletonCountDown;

	//true as long new skeleton will be added to the pool
	bool createNewSkeleton = true;

	//loop through all MMH skeletons
	for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
	{
		//when skeletons have the same ID, the skeleton is alredy in pool and no new skeleton has to be created
		if (avatarID == itPoolSkeletons->first)
		{
			//convert Xsens skeleton into MMH skeleton
			Skeleton* currSkeleton = parseSkeleton(m_quaternianDataWithId, avatarID, &m_skeletonPool[avatarID]);

			if (currSkeleton != nullptr)
			{
				// update all joints of existing skeleon with new data
				m_skeletonPool[avatarID].m_joints = currSkeleton->m_joints;

			}

			//delete temp skeleton object (FIXED MEMORY LEAK)
			delete currSkeleton;

			//no new skeleton has to be created
			createNewSkeleton = false;

			break;
		}

	}



	// create new skeleton
	if (createNewSkeleton)
	{
		// create new skeleton and add it to the skeleton pool
		m_skeletonPool.insert({ avatarID, *parseSkeleton(m_quaternianDataWithId, avatarID, new Skeleton()) });

		//skeleton was added/removed, so UI updates
		m_hasSkeletonPoolChanged = true;

		Console::log("XSTracker::extractSkeleton(): Created new skeleton with id = " + std::to_string(avatarID) + ".");

		// BVH WRITER
		if (isRecordBvh) {
			XSBvhWriter* xsb = new XSBvhWriter(XSBvhWriter::SkeletonType::Xsens, m_UdpServer, avatarID);
			bvhWriters->push_back(xsb);
			Console::log("XSTracker::extractSkeleton(): Started recording BVH for skeleton with id = " + std::to_string(avatarID) + ".");
		}
	}
}


Quaternionf XSTracker::rotateAxis(const double& a, const double& xx, const double& yy, const double& zz)
{
	// Here we calculate the sin( theta / 2) once for optimization
	double factor = sin(a / 2.0);

	// Calculate the x, y and z of the quaternion
	double x = xx * factor;
	double y = yy * factor;
	double z = zz * factor;

	// Calcualte the w value by cos( theta / 2 )
	double w = cos(a / 2.0);
	Quaternionf rotQuat = Quaternionf(w, x, y, z);
	rotQuat.normalize();
	return rotQuat;
}

//takes data from a Xsens skeleton and pushes it into the list
Skeleton* XSTracker::parseSkeleton(ParserManager::QuaternionDataWithId* quaternianDataWithId, int id, Skeleton* oldSkeletonData)
{
	//skeleton data container
	Skeleton* currSkeleton = new Skeleton(id);

	//loop through all joints
	for (int i = 0; i < quaternianDataWithId->kinematics->size(); i++)
	{

		m_kinematics = quaternianDataWithId->kinematics;

		Vector4f pos = //m_offsetMatrix * 
			Vector4f(m_kinematics->at(i).position[1], m_kinematics->at(i).position[2], m_kinematics->at(i).position[0], 1.0f);
		Quaternionf rot = Quaternionf(m_kinematics->at(i).orientation[0], -m_kinematics->at(i).orientation[2], m_kinematics->at(i).orientation[3], -m_kinematics->at(i).orientation[1]);

		// rotate y-Axis by 180 degrees
		Quaternionf rotYAxis = rotateAxis(180 * M_PI / 180, 0, 1, 0);
		rot = rotYAxis * rot;

		//confidence values are not transmitted, default confidence is High
		Joint::JointConfidence confidence = Joint::JointConfidence::HIGH;



		//map the Xsens poses to the MMH skeleton joints
		switch (i)
		{

		case 0:
			currSkeleton->m_joints.insert({ Joint::HIPS, Joint(pos, rot, confidence) });
			break;

		case 1:
			//it may be necessary to multiply j = 1 with j = 2
			currSkeleton->m_joints.insert({ Joint::SPINE, Joint(pos, rot, confidence) });
			break;

		case 2:
			//currSkeleton->m_joints.insert({ Joint::SPINE, Joint(pos, rot, confidence) });
			break;

		case 3:
			//it may be necessary to multiply j = 3 with j = 4
			currSkeleton->m_joints.insert({ Joint::CHEST, Joint(pos, rot, confidence) });
			break;

		case 4:
			//currSkeleton->m_joints.insert({ Joint::CHEST, Joint(pos, rot, confidence) });
			break;

		case 5:
			currSkeleton->m_joints.insert({ Joint::NECK, Joint(pos, rot, confidence) });
			break;

		case 6:
			currSkeleton->m_joints.insert({ Joint::HEAD, Joint(pos, rot, confidence) });
			break;

		case 7:
			currSkeleton->m_joints.insert({ Joint::SHOULDER_R, Joint(pos, rot, confidence) });
			break;

		case 8:
			currSkeleton->m_joints.insert({ Joint::ARM_R, Joint(pos, rot, confidence) });
			break;

		case 9:
			currSkeleton->m_joints.insert({ Joint::FOREARM_R, Joint(pos, rot, confidence) });
			break;

		case 10:
			currSkeleton->m_joints.insert({ Joint::HAND_R, Joint(pos, rot, confidence) });
			break;

		case 11:
			currSkeleton->m_joints.insert({ Joint::SHOULDER_L, Joint(pos, rot, confidence) });
			break;

		case 12:
			currSkeleton->m_joints.insert({ Joint::ARM_L, Joint(pos, rot, confidence) });
			break;

		case 13:
			currSkeleton->m_joints.insert({ Joint::FOREARM_L, Joint(pos, rot, confidence) });
			break;

		case 14:
			currSkeleton->m_joints.insert({ Joint::HAND_L, Joint(pos, rot, confidence) });
			break;

		case 15:
			currSkeleton->m_joints.insert({ Joint::UPLEG_R, Joint(pos, rot, confidence) });
			break;

		case 16:
			currSkeleton->m_joints.insert({ Joint::LEG_R, Joint(pos, rot, confidence) });
			break;

		case 17:
			currSkeleton->m_joints.insert({ Joint::FOOT_R, Joint(pos, rot, confidence) });
			break;

		case 18:
			currSkeleton->m_joints.insert({ Joint::TOE_R, Joint(pos, rot, confidence) });
			break;

		case 19:
			currSkeleton->m_joints.insert({ Joint::UPLEG_L, Joint(pos, rot, confidence) });
			break;

		case 20:
			currSkeleton->m_joints.insert({ Joint::LEG_L, Joint(pos, rot, confidence) });
			break;

		case 21:
			currSkeleton->m_joints.insert({ Joint::FOOT_L, Joint(pos, rot, confidence) });
			break;

		case 22:
			currSkeleton->m_joints.insert({ Joint::TOE_L, Joint(pos, rot, confidence) });
			break;

		default:
			break;

		}

	}

	// set body heigt based on head position
	currSkeleton->setHeight(currSkeleton->m_joints[Joint::HEAD].getJointPosition().y());

	//return skeleton with correct joint poses
	return currSkeleton;

}

// erase all unused skeletons from pool
void XSTracker::cleanSkeletonPool()
{
	int avatarToBeErased = -1;

	//loop thorugh all Xsens avatars; if frameCountdown of avatar < 0 -> erase its skeleton
	for (auto const& [id, frameCountdown] : m_avatarList)
	{
		if (frameCountdown < 0) {
			// erase skeleton with this id
			m_skeletonPool.erase(id);
			// remove from avatarList
			avatarToBeErased = id;

			// skeleton was added/removed, so UI updates
			m_hasSkeletonPoolChanged = true;

			Console::log("XSTracker::cleanSkeletonList(): Removed skeleton with id = " + std::to_string(id) + " from pool!");

		}

	}

	// remove from avatarList
	if (avatarToBeErased != -1) {
		m_avatarList.erase(avatarToBeErased);
	}

}


std::string XSTracker::getTrackerType()
{
	return "Xsens";
}