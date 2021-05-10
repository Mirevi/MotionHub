#include "AKTracker.h"

// default constructor
AKTracker::AKTracker(int id, NetworkManager* networkManager, ConfigManager* configManager)
{
	// initialize azure kinect camera and body tracker
	if (!init()) {
		m_tracker = NULL;
		m_cam = NULL;

		valid = false;

		throw Exception("No connected Azure Kinect found.");
	}

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_azureKinect_" + std::to_string(id);

	m_networkManager = networkManager;
	m_configManager = configManager;

	//tracker is enabled
	m_isEnabled = true;

	/*//set default values for offsets
	setPositionOffset(Vector3f(configManager->getFloatFromStartupConfig("xPosAzure"), configManager->getFloatFromStartupConfig("yPosAzure"), configManager->getFloatFromStartupConfig("zPosAzure")));
	setRotationOffset(Vector3f(configManager->getFloatFromStartupConfig("xRotAzure"), configManager->getFloatFromStartupConfig("yRotAzure"), configManager->getFloatFromStartupConfig("zRotAzure")));
	setScaleOffset(Vector3f(configManager->getFloatFromStartupConfig("xSclAzure"), configManager->getFloatFromStartupConfig("ySclAzure"), configManager->getFloatFromStartupConfig("zSclAzure")));*/

	// read property values from config
	readOffsetFromConfig();
}

AKTracker::~AKTracker() 
{
	// shutdown and destroy azure kinect tracker
	if (m_tracker != NULL) {
		k4abt_tracker_shutdown(m_tracker);
		k4abt_tracker_destroy(m_tracker);
	}

	// stop and close camera
	if(m_cam != NULL) {
		k4a_device_stop_cameras(m_cam);
		k4a_device_close(m_cam);
	}
}

bool AKTracker::init()
{
	uint32_t device_count = k4a_device_get_installed_count();

	for (int i = 0; i < device_count; i++) {
		m_cam = NULL;
		// open camera ---> ERROR:  libusb device(s) are all unavalable.
		if (k4a_device_open(i, &m_cam) != K4A_RESULT_SUCCEEDED) {
			Console::logError("[cam id = " + std::to_string(i) + "] + Open K4A Device failed!");
			continue;
		}

		// setup camera config
		m_configCam = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
		m_configCam.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
		m_configCam.color_resolution = K4A_COLOR_RESOLUTION_OFF;
		// start camera
		if (k4a_device_start_cameras(m_cam, &m_configCam) != K4A_RESULT_SUCCEEDED) {
			Console::logError("[cam id = " + std::to_string(i) + "] + Start K4A cameras failed!");
			k4a_device_close(m_cam);
			continue;
		}

		// calibrate camera
		if (k4a_device_get_calibration(m_cam, m_configCam.depth_mode, m_configCam.color_resolution, &m_calibrationCam) != K4A_RESULT_SUCCEEDED) {
			Console::logError("[cam id = " + std::to_string(i) + "] + Get depth camera calibration failed!");
			k4a_device_stop_cameras(m_cam);
			k4a_device_close(m_cam);
			continue;
		}

		m_tracker = NULL;
		// setup tracker config
		m_configTracker = K4ABT_TRACKER_CONFIG_DEFAULT;

		// create tracker
		if (k4abt_tracker_create(&m_calibrationCam, m_configTracker, &m_tracker) != K4A_RESULT_SUCCEEDED) {
			Console::logError("[cam id = " + std::to_string(i) + "] + Body tracker initialization failed!");
			k4a_device_stop_cameras(m_cam);
			k4a_device_close(m_cam);
			continue;
		}

		// assign cam id
		m_idCam = i;

		// assign cam serial number
		size_t serial_num_size = 0;
		if (k4a_buffer_result_t::K4A_BUFFER_RESULT_TOO_SMALL == k4a_device_get_serialnum(m_cam, nullptr, &serial_num_size)) {
			char* serial = new char[serial_num_size];
			if (k4a_buffer_result_t::K4A_BUFFER_RESULT_SUCCEEDED == k4a_device_get_serialnum(m_cam, serial, &serial_num_size)) m_serial = serial;
			delete[] serial;
		}
		//std::cout << m_serial << std::endl;

		Console::log("TrackerManager::createTracker(): Creating AKtracker with cam ID = " + std::to_string(m_idCam) + " ...");
		return true;
	}
	return false;
}

/*void AKTracker::init()
{
	m_cam = NULL;
	// open camera ---> ERROR:  libusb device(s) are all unavalable.
	VERIFY_K4A_FUNCTION(k4a_device_open(m_idCam, &m_cam), "[cam id = " + std::to_string(m_idCam) + "] + Open K4A Device failed!");

	// setup camera config
	m_configCam = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	m_configCam.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
	m_configCam.color_resolution = K4A_COLOR_RESOLUTION_OFF;
	// start camera
	VERIFY_K4A_FUNCTION(k4a_device_start_cameras(m_cam, &m_configCam), "[cam id = " + std::to_string(m_idCam) + "] + Start K4A cameras failed!");

	// calibrate camera
	VERIFY_K4A_FUNCTION(k4a_device_get_calibration(m_cam, m_configCam.depth_mode, m_configCam.color_resolution, &m_calibrationCam), "[cam id = " + std::to_string(m_idCam) + "] + Get depth camera calibration failed!");

	m_tracker = NULL;
	// setup tracker config
	m_configTracker = K4ABT_TRACKER_CONFIG_DEFAULT;


	// create tracker
	VERIFY_K4A_FUNCTION(k4abt_tracker_create(&m_calibrationCam, m_configTracker, &m_tracker), "[cam id = " + std::to_string(m_idCam) + "] + Body tracker initialization failed!");

}*/

// get new skeleton data and parse it into the default skeleton
void AKTracker::track()
{

	// create sensor capture and result
	k4a_capture_t sensor_capture;
	k4a_wait_result_t get_capture_result = k4a_device_get_capture(m_cam, &sensor_capture, K4A_WAIT_INFINITE);

	// process capture if result succeeded
	if (get_capture_result == K4A_WAIT_RESULT_SUCCEEDED)
	{

		// add capture to body tracker process quene
		k4a_wait_result_t queue_capture_result = k4abt_tracker_enqueue_capture(m_tracker, sensor_capture, K4A_WAIT_INFINITE);
		k4a_capture_release(sensor_capture); // release the sensor capture once we finish using it

		// error handling quene capture timeout
		if (queue_capture_result == K4A_WAIT_RESULT_TIMEOUT)
		{
			// it should never hit timeout when K4A_WAIT_INFINITE is set
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Add capture to tracker process queue timeout");
			return;

		}
		// error handling quene capture failure
		else if (queue_capture_result == K4A_WAIT_RESULT_FAILED)
		{
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Add capture to tracker process queue failed");
			return;
		}

		// extract body frame out of capture
		k4abt_frame_t body_frame = NULL;
		k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(m_tracker, &body_frame, K4A_WAIT_INFINITE);

		// successfully popped the body tracking result
		if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED)
		{

			// extract skeletons from body frame and parse them into default skeleton pool
			extractSkeleton(&body_frame);



			// clean up skeleton pool - remove inactive skeletons
			cleanSkeletonPool(&body_frame);

			// remember to release the body frame once you finish using it
			k4abt_frame_release(body_frame);

			//count tracking cycles
			m_trackingCycles++;

			// set data available to true
			m_isDataAvailable = true;


		}
		// error handling
		else if (pop_frame_result == K4A_WAIT_RESULT_TIMEOUT)
		{
			//  it should never hit timeout when K4A_WAIT_INFINITE is set
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Pop body frame result timeout");
			m_isDataAvailable = false;

			return;
		}
		else
		{
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Pop body frame result failed");
			return;
		}
	}
	else if (get_capture_result == K4A_WAIT_RESULT_TIMEOUT)
	{
		// It should never hit time out when K4A_WAIT_INFINITE is set.
		Console::logError("[cam id = " + std::to_string(m_idCam) + "] Get depth frame time out");
		return;
	}
	else
	{
		Console::logError("[cam id = " + std::to_string(m_idCam) + "] Get depth capture returned error: " + std::to_string(get_capture_result));
		return;
	}
}

// extract skeletons from body frame and parse them into default skeleton pool
void AKTracker::extractSkeleton(k4abt_frame_t* body_frame)
{
	// set number of detected bodies in frame
	m_countDetectedSkeleton = k4abt_frame_get_num_bodies(*body_frame);

	//Console::log(std::to_string(m_numBodies));

	m_skeletonPoolLock.lock();

	// skeleton loop
	for (int indexSkeleton = 0; indexSkeleton < m_countDetectedSkeleton; indexSkeleton++)
	{
		// get the skeleton and the id
		k4abt_skeleton_t skeleton;
		k4abt_frame_get_body_skeleton(*body_frame, indexSkeleton, &skeleton);
		uint32_t id = k4abt_frame_get_body_id(*body_frame, indexSkeleton);

		// find existing skeleton or insert a new empty one
		m_hasSkeletonPoolChanged = m_hasSkeletonPoolChanged || m_skeletonPool.emplace(id, id).second;

		// update all joints of skeleton with new data
		Skeleton* currSkeleton = parseSkeleton(&skeleton, id);
		m_skeletonPool[id].m_joints = currSkeleton->m_joints;
		delete currSkeleton;

		if (m_hasSkeletonPoolChanged) Console::log("AkTracker::updateSkeleton(): [cam id = " + std::to_string(m_idCam) + "] Created new skeleton with id = " + std::to_string(id) + ".");
	}
	m_skeletonPoolLock.unlock();
}

//takes data from a k4a skeleton and pushes it into the list
Skeleton* AKTracker::parseSkeleton(k4abt_skeleton_t * skeleton, int id)
{

	// skeleton data container
	Skeleton* currSkeleton = new Skeleton(id);

	// loop through all joints, get the position and rotation and pass them into the joint map
	for (int jointIndex = 0; jointIndex < K4ABT_JOINT_COUNT; jointIndex++)
	{

		// get position and rotation
		k4a_float3_t skeleton_position = skeleton->joints[jointIndex].position;
		k4a_quaternion_t skeleton_rotation = skeleton->joints[jointIndex].orientation;


		// convert from k4a Vectors and quaternions into Eigen vector and quaternion with coordinate transformation
		Vector4f pos = Vector4f(skeleton_position.xyz.x, skeleton_position.xyz.y, skeleton_position.xyz.z, 1);
		pos.head<3>() = AngleAxisf(M_PI, Vector3f::UnitX()) * (pos.head<3>() * 0.001f); // The 0.001 translates Azure Kinect millimeters to MMH meters
		Quaternionf rot = Quaternionf(skeleton_rotation.wxyz.w, skeleton_rotation.wxyz.x, skeleton_rotation.wxyz.y, skeleton_rotation.wxyz.z);

		// get joint confidence level from azure kinect body tracker API
		Joint::JointConfidence confidence = (Joint::JointConfidence)skeleton->joints[jointIndex].confidence_level;

		// map azure kinect skeleton joints to default skeleton joints and set confidence level
		//rotations are converted from global to local
		Joint::JointNames jointName;

		switch (jointIndex) {
		case 0:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
			jointName = Joint::HIPS;
			break;

		case 1:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
			jointName = Joint::SPINE;
			break;

		case 2:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
			jointName = Joint::CHEST;
			break;

		case 3:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
			jointName = Joint::NECK;
			break;

		case 4:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, 0, 0));
			jointName = Joint::SHOULDER_L;
			break;

		case 5:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, 0, 0));
			jointName = Joint::ARM_L;
			break;

		case 6:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, 0, 0));
			jointName = Joint::FOREARM_L;
			break;

		case 7:
			rot *= azureKinectEulerToQuaternion(Vector3f(0, 180, 180));
			jointName = Joint::HAND_L;
			break;

		case 11:
			rot *= azureKinectEulerToQuaternion(Vector3f(270, 0, 0));
			jointName = Joint::SHOULDER_R;
			break;

		case 12:
			rot *= azureKinectEulerToQuaternion(Vector3f(270, 0, 0));
			jointName = Joint::ARM_R;
			break;

		case 13:
			rot *= azureKinectEulerToQuaternion(Vector3f(270, 0, 0));
			jointName = Joint::FOREARM_R;
			break;

		case 14:
			rot *= azureKinectEulerToQuaternion(Vector3f(0, 0, 0));
			jointName = Joint::HAND_R;
			break;

		case 18:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
			jointName = Joint::UPLEG_L;
			break;

		case 19:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
			jointName = Joint::LEG_L;
			break;

		case 20:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
			jointName = Joint::FOOT_L;
			break;

		case 21:
			rot *= azureKinectEulerToQuaternion(Vector3f(0, -90, 0));
			jointName = Joint::TOE_L;
			break;

		case 22:
			rot *= azureKinectEulerToQuaternion(Vector3f(-90, -90, 0));
			jointName = Joint::UPLEG_R;
			break;

		case 23:
			rot *= azureKinectEulerToQuaternion(Vector3f(-90, -90, 0));
			jointName = Joint::LEG_R;
			break;

		case 24:
			rot *= azureKinectEulerToQuaternion(Vector3f(-90, -90, 0));
			jointName = Joint::FOOT_R;
			break;

		case 25:
			rot *= azureKinectEulerToQuaternion(Vector3f(0, 90, 180));
			jointName = Joint::TOE_R;
			break;

		case 26:
			rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
			jointName = Joint::HEAD;
			break;

		default:
			break;
		}
		rot = Quaternionf(rot.y(), rot.z(), rot.w(), rot.x());

		currSkeleton->m_joints.insert({ jointName, Joint(applyOffset(pos), applyOffset(rot), confidence) });
	}

	// set body heigt based on head position
	currSkeleton->setHeight(currSkeleton->m_joints[Joint::HEAD].getJointPosition().y());

	// return parsed default skeleton
	return currSkeleton;

}

// erase all unused skeletons from pool
void AKTracker::cleanSkeletonPool(k4abt_frame_t* bodyFrame)
{

	//all skeletons with ids in this list will be erased at the end of this method
	std::list<int> idSkeletonsToErase;

	m_skeletonPoolLock.lock();

	// loop through all skeletons in pool
	for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
	{
		// get current skeleton id
		int idCurrPoolSkeleton = itPoolSkeletons->first;
		bool isK4aSkeletonInPool = false;

		//loop thorugh all k4a skeletons in frame
		for (int indexK4aSkeleton = 0; indexK4aSkeleton < m_countDetectedSkeleton; indexK4aSkeleton++)
		{
			// current k4a skeleton id
			int idCurrK4aSkeleton = k4abt_frame_get_body_id(*bodyFrame, indexK4aSkeleton);

			// if k4a skeleton is in pool set isK4aSkeletonInPool to true
			if (idCurrPoolSkeleton == idCurrK4aSkeleton)
			{
				isK4aSkeletonInPool = true;
			}
		}

		if (!isK4aSkeletonInPool)
		{
			idSkeletonsToErase.push_back(idCurrPoolSkeleton);
		}
	}
	for (auto itIndexIdSkeletonsToErase = idSkeletonsToErase.begin(); itIndexIdSkeletonsToErase != idSkeletonsToErase.end(); itIndexIdSkeletonsToErase++)
	{
		// erase skeleton with id
		m_skeletonPool.erase(*itIndexIdSkeletonsToErase);

		//skeleton was removed, so UI updates
		m_hasSkeletonPoolChanged = true;

		Console::log("AkTracker::cleanSkeletonList(): [cam id = " + std::to_string(m_idCam) + "] Removed skeleton with id = " + std::to_string(*itIndexIdSkeletonsToErase) + " from pool");
	}
	m_skeletonPoolLock.unlock();
}

Quaternionf AKTracker::convertKinectRotation(Quaternionf value)
{

	return eulerToQuaternion(Vector3f(0.0f, 180.0f, 0.0f)) * value;

}

Quaternionf AKTracker::azureKinectEulerToQuaternion(Vector3f euler)
{

	Quaternionf qRotation;

	//calculate rotation quaternion
	qRotation = AngleAxisf(euler.x() * M_PI / 180, Vector3f::UnitX())
		  * AngleAxisf(euler.y() * M_PI / 180, Vector3f::UnitY())
		  * AngleAxisf(euler.z() * M_PI / 180, Vector3f::UnitZ());

	//invert z-compunent (this works for our left handed coordinate system)
	qRotation = Quaternionf(qRotation.w(), qRotation.x(), qRotation.y(), -qRotation.z());

	return qRotation;

}

std::string AKTracker::getTrackerType()
{
	return "Azure";
}

std::string AKTracker::getTrackerIdentifier()
{
	return m_serial;
}
