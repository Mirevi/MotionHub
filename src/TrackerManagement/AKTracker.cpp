#include "AKTracker.h"

// default constructor
AKTracker::AKTracker(int id, int idCam, NetworkManager* networkManager, ConfigManager* configManager)
{


	uint32_t device_count = k4a_device_get_installed_count();

	if (device_count > idCam)
	{

		// initialize azure kinect camera and body tracker
		init();

	}
	else
	{

		Console::logError("No connected Azure Kinect found.");

		valid = false;

		return;

	}

	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_azureKinect_" + std::to_string(id);

	m_networkManager = networkManager;
	m_configManager = configManager;

	// assign cam id
	m_idCam = idCam;

	//tracker is enabled
	m_properties->isEnabled = true;
	

	
	

	//set default values for offsets
	setPositionOffset(Vector3f(configManager->getFloatFromStartupConfig("xPosAzure"),
							   configManager->getFloatFromStartupConfig("yPosAzure"),
							   configManager->getFloatFromStartupConfig("zPosAzure")
							  ));

	setRotationOffset(Vector3f(configManager->getFloatFromStartupConfig("xRotAzure"),
							   configManager->getFloatFromStartupConfig("yRotAzure"),
							   configManager->getFloatFromStartupConfig("zRotAzure")
							  ));

	setScaleOffset(Vector3f(configManager->getFloatFromStartupConfig("xSclAzure"),
							configManager->getFloatFromStartupConfig("ySclAzure"),
							configManager->getFloatFromStartupConfig("zSclAzure")
							));







}

// start azure kinect tracker
void AKTracker::start()
{

	// set tracking to true
	m_properties->isTracking = true;

	// start tracking thread and detach the thread from method scope runtime
	m_trackingThread = new std::thread(&AKTracker::update, this);
	m_trackingThread->detach();
	
}

// stop tracking loop / thread
void AKTracker::stop()
{
	//is not tracking, so the update loop exits after current loop
	m_properties->isTracking = false;

}

// shutdown and destroy azure kinect tracker
void AKTracker::destroy()
{

	// shutdown and destroy tracker
	k4abt_tracker_shutdown(m_tracker);
	k4abt_tracker_destroy(m_tracker);

	// stop and close camera
	k4a_device_stop_cameras(m_cam);
	k4a_device_close(m_cam);

	// delete this object
	delete this;

}

void AKTracker::init()
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

}

//// tracking loop
//void AKTracker::update()
//{
//
//	// track while tracking is true
//	while (m_properties->isTracking)
//	{
//
//
//		// get new data
//		track();
//
//		//send Skeleton Pool to NetworkManager
//		m_networkManager->sendSkeletonPool(&m_skeletonPool, m_properties->id);
//
//	}
//
//	//clean skeleton pool after tracking
//	clean();
//
//}

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
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Add capture to tracker process queue timeout!");
			return;

		}
		// error handling quene capture failure
		else if (queue_capture_result == K4A_WAIT_RESULT_FAILED)
		{
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Add capture to tracker process queue failed!");
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
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Pop body frame result timeout!");
			m_isDataAvailable = false;

			return;
		}
		else
		{
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Pop body frame result failed!");
			return;
		}
	}
	else if (get_capture_result == K4A_WAIT_RESULT_TIMEOUT)
	{
		// It should never hit time out when K4A_WAIT_INFINITE is set.
		Console::logError("[cam id = " + std::to_string(m_idCam) + "] Get depth frame time out!");
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
	m_properties->countDetectedSkeleton = k4abt_frame_get_num_bodies(*body_frame);

	//Console::log(std::to_string(m_numBodies));

	// skeleton loop
	for (int indexSkeleton = 0; indexSkeleton < m_properties->countDetectedSkeleton; indexSkeleton++)
	{

		// get the skeleton and the id
		k4abt_skeleton_t skeleton;
		k4abt_frame_get_body_skeleton(*body_frame, indexSkeleton, &skeleton);
		uint32_t id = k4abt_frame_get_body_id(*body_frame, indexSkeleton);

		bool createNewSkeleton = true;

		m_skeletonPoolLock.lock();

		// update existing skeleton
		for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
		{

			if (id == itPoolSkeletons->first)
			{

				// update all joints of existing skeleon with new data
				m_skeletonPool[id].m_joints = parseSkeleton(&skeleton, id)->m_joints;

				createNewSkeleton = false;

				break;

			}
		}

		// create new skeleton
		if (createNewSkeleton)
		{

			// create new skeleton and add it to the skeleton pool
			m_skeletonPool.insert({ id, *parseSkeleton(&skeleton, id) });
			
			//skeleton was added, so UI updates
			m_hasSkeletonPoolChanged = true;

			Console::log("AkTracker::updateSkeleton(): [cam id = " + std::to_string(m_idCam) + "] Created new skeleton with id = " + std::to_string(id) + ".");

		}

		m_skeletonPoolLock.unlock();
	}
}

//takes data from a k4a skeleton and pushes it into the list
Skeleton* AKTracker::parseSkeleton(k4abt_skeleton_t* skeleton, int id)
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
		Vector4f pos	= m_offsetMatrix * Vector4f(skeleton_position.xyz.x, skeleton_position.xyz.y, skeleton_position.xyz.z, 1);
		Quaternionf rot = Quaternionf(skeleton_rotation.wxyz.w, skeleton_rotation.wxyz.x, skeleton_rotation.wxyz.y, skeleton_rotation.wxyz.z);

		// get joint confidence level from azure kinect body tracker API
		Joint::JointConfidence confidence = (Joint::JointConfidence)skeleton->joints[jointIndex].confidence_level;

		// map azure kinect skeleton joints to default skeleton joints and set confidence level
		//rotations are converted from global to local
		switch (jointIndex)
		{

			case 0:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::HIPS, Joint(pos, rot, confidence) });
				break;

			case 1:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::SPINE, Joint(pos, rot, confidence) });
				break;

			case 2:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::CHEST, Joint(pos, rot, confidence) });
				break;
				 
			case 3:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::NECK, Joint(pos, rot, confidence) });
				break;

			case 4:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, 0, 0));
				currSkeleton->m_joints.insert({ Joint::SHOULDER_L, Joint(pos, rot, confidence) });
				break;

			case 5:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, 0, 0));
				currSkeleton->m_joints.insert({ Joint::ARM_L, Joint(pos, rot, confidence) });
				break;

			case 6:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, 0, 0));
				currSkeleton->m_joints.insert({ Joint::FOREARM_L, Joint(pos, rot, confidence) });
				break;

			case 7:
				 rot *= azureKinectEulerToQuaternion(Vector3f(0, 180, 180));
				currSkeleton->m_joints.insert({ Joint::HAND_L, Joint(pos, rot, confidence) });
				break;

			case 11:
				 rot *= azureKinectEulerToQuaternion(Vector3f(270, 0, 0));
				currSkeleton->m_joints.insert({ Joint::SHOULDER_R, Joint(pos, rot, confidence) });
				break;

			case 12:
				 rot *= azureKinectEulerToQuaternion(Vector3f(270, 0, 0));
				currSkeleton->m_joints.insert({ Joint::ARM_R, Joint(pos, rot, confidence) });
				break;

			case 13:
				 rot *= azureKinectEulerToQuaternion(Vector3f(270, 0, 0));
				currSkeleton->m_joints.insert({ Joint::FOREARM_R, Joint(pos, rot, confidence) });
				break;

			case 14:
				 rot *= azureKinectEulerToQuaternion(Vector3f(0, 0, 0));
				currSkeleton->m_joints.insert({ Joint::HAND_R, Joint(pos, rot, confidence) });
				break;

			case 18:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::UPLEG_L, Joint(pos, rot, confidence) });
				break;

			case 19:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::LEG_L, Joint(pos, rot, confidence) });
				break;

			case 20:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::FOOT_L, Joint(pos, rot, confidence) });
				break;

			case 21:
				 rot *= azureKinectEulerToQuaternion(Vector3f(0, -90, 0));
				currSkeleton->m_joints.insert({ Joint::TOE_L, Joint(pos, rot, confidence) });
				break;

			case 22:
				 rot *= azureKinectEulerToQuaternion(Vector3f(-90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::UPLEG_R, Joint(pos, rot, confidence) });
				break;

			case 23:
				 rot *= azureKinectEulerToQuaternion(Vector3f(-90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::LEG_R, Joint(pos, rot, confidence) });
				break;

			case 24:
				 rot *= azureKinectEulerToQuaternion(Vector3f(-90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::FOOT_R, Joint(pos, rot, confidence) });
				break;

			case 25:
				 rot *= azureKinectEulerToQuaternion(Vector3f(0, 90, 180));
				currSkeleton->m_joints.insert({ Joint::TOE_R, Joint(pos, rot, confidence) });
				break;

			case 26:
				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
				currSkeleton->m_joints.insert({ Joint::HEAD, Joint(pos, rot, confidence) });
				break;

			default:
				break;
		}
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

	// loop through all skeletons in pool
	for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
	{	

		// get current skeleton id
		int idCurrPoolSkeleton = itPoolSkeletons->first;
		bool isK4aSkeletonInPool = false;

		//loop thorugh all k4a skeletons in frame
		for (int indexK4aSkeleton = 0; indexK4aSkeleton < m_properties->countDetectedSkeleton; indexK4aSkeleton++)
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

		Console::log("AkTracker::cleanSkeletonList(): [cam id = " + std::to_string(m_idCam) + "] Removed skeleton with id = " + std::to_string(*itIndexIdSkeletonsToErase) + " from pool!");

	}
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


std::vector<Vector3f> AKTracker::resetOffsets()
{



	Vector3f pos = Vector3f(0, 1.175, 2.2);
	Vector3f rot = Vector3f(-0.5, 0, 0);
	Vector3f scl = Vector3f(0.001, -0.001, -0.001);

	setPositionOffset(pos);
	setRotationOffset(rot);
	setScaleOffset(scl);

	std::vector<Vector3f> offsets = { pos, rot, scl };

	return offsets;

}