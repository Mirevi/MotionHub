#include "AKTracker.h"
#include "MotionHubUtil/Vector3.h"

// default constructor
AKTracker::AKTracker(int id, int idCam)
{
	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_azureKinect_" + std::to_string(id);

	// assign cam id
	m_idCam = idCam;

	m_properties->isEnabled = true;

	m_properties->positionOffset = Vector3f(0.0f, 0.95f, 2.2f);
	m_properties->rotationOffset = Vector3f(0.0f, 0.0f, 0.0f);
	m_properties->scaleOffset = Vector3f(-0.001f, -0.001f, -0.001f);



	//create new Matrix and set it to be identity
	m_offsetMatrix = transformMatrix(m_properties->positionOffset, m_properties->rotationOffset, m_properties->scaleOffset);


	//using cout to test because there is not .toString()
	std::cout << m_offsetMatrix << std::endl;

	// initialize azure kinect camera and body tracker
	init();

}

void AKTracker::init()
{
	Console::log("AKTracker::init(): start initialization with cam id = " + std::to_string(m_idCam) + "]");


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


	Console::log("AKTracker::init(): init done");
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

// tracking loop
void AKTracker::update()
{

	Console::log("[cam id = " + std::to_string(m_idCam) + "] AKTracker::track(): Started tracking thread.");

	// track while tracking is true
	while (m_properties->isTracking)
	{

		// if no new data is procressed
		if(!m_isDataAvailable)
			track(); // get new data

	}

	//clean skeleton pool after tracking
	clean();

	Console::log("[cam id = " + std::to_string(m_idCam) + "] AKTracker::track(): Stopped tracking thread.");

}

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

// stop tracking loop / thread
void AKTracker::stop()
{
	//is not tracking, so the update loop exits 
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

	Console::log("[cam id = " + std::to_string(m_idCam) + "] AKTracker::destroy(): Destroyed tracker with camera id = " + std::to_string(m_idCam) + ".");

	// delete this object
	delete this;

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

		// update existing skeleton
		for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
		{

			if (id == itPoolSkeletons->first)
			{

				// update all joints of existing skeleon with new data
				m_skeletonPool[id]->m_joints = parseSkeleton(&skeleton, id)->m_joints;

				//Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::updateSkeleton(): Skeleton with id = " + std::to_string(id) + " pelvis position = " + poolSkeletons[id].m_joints[Joint::JOINT_PELVIS].getJointPosition().toString() + ".");

				//Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::updateSkeleton(): Updated skeleton with id = " + std::to_string(id) + ".");

				createNewSkeleton = false;

				break;

			}
		}

		// create new skeleton
		if (createNewSkeleton)
		{

			// create new skeleton and add it to the skeleton pool
			m_skeletonPool.insert(std::pair<int, Skeleton*>(id, parseSkeleton(&skeleton, id)));
			
			//skeleton was added/removed, so UI updates
			m_hasSkeletonPoolChanged = true;

			Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::updateSkeleton(): Created new skeleton with id = " + std::to_string(id) + ".");

		}
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


		// convert from k4a Vectors and quaternions into custom vectors with coordinate transformation
		Vector4f pos = m_offsetMatrix * Vector4f(skeleton_position.xyz.x, skeleton_position.xyz.y, skeleton_position.xyz.z, 1);
		Quaternionf rot = Quaternionf(skeleton_rotation.wxyz.w, skeleton_rotation.wxyz.x, skeleton_rotation.wxyz.y, skeleton_rotation.wxyz.z);

		// get joint confidence level from azure kinect body tracker API
		Joint::JointConfidence confidence = (Joint::JointConfidence)skeleton->joints[jointIndex].confidence_level;

		// map azure kinect skeleton joints to default skeleton joints and set confidence level
		switch (jointIndex)
		{

			case 0:
				currSkeleton->m_joints.insert({ Joint::HIPS, Joint(pos, rot, confidence) });

				Console::log(Vector3(pos).toString());

				break;

			case 1:
				currSkeleton->m_joints.insert({ Joint::SPINE, Joint(pos, rot, confidence) });
				break;

			case 2:
				currSkeleton->m_joints.insert({ Joint::CHEST, Joint(pos, rot, confidence) });
				break;

			case 3:
				currSkeleton->m_joints.insert({ Joint::NECK, Joint(pos, rot, confidence) });
				break;

			case 4:
				currSkeleton->m_joints.insert({ Joint::SHOULDER_L, Joint(pos, rot, confidence) });
				break;

			case 5:
				currSkeleton->m_joints.insert({ Joint::ARM_L, Joint(pos, rot, confidence) });
				break;

			case 6:
				currSkeleton->m_joints.insert({ Joint::FOREARM_L, Joint(pos, rot, confidence) });
				break;

			case 7:
				currSkeleton->m_joints.insert({ Joint::HAND_L, Joint(pos, rot, confidence) });
				break;

			case 11:
				currSkeleton->m_joints.insert({ Joint::SHOULDER_R, Joint(pos, rot, confidence) });
				break;

			case 12:
				currSkeleton->m_joints.insert({ Joint::ARM_R, Joint(pos, rot, confidence) });
				break;

			case 13:
				currSkeleton->m_joints.insert({ Joint::FOREARM_R, Joint(pos, rot, confidence) });
				break;

			case 14:
				currSkeleton->m_joints.insert({ Joint::HAND_R, Joint(pos, rot, confidence) });
				break;

			case 18:
				currSkeleton->m_joints.insert({ Joint::UPLEG_L, Joint(pos, rot, confidence) });
				break;

			case 19:
				currSkeleton->m_joints.insert({ Joint::LEG_L, Joint(pos, rot, confidence) });
				break;

			case 20:
				currSkeleton->m_joints.insert({ Joint::FOOT_L, Joint(pos, rot, confidence) });
				break;

			case 21:
				currSkeleton->m_joints.insert({ Joint::TOE_L, Joint(pos, rot, confidence) });
				break;

			case 22:
				currSkeleton->m_joints.insert({ Joint::UPLEG_R, Joint(pos, rot, confidence) });
				break;

			case 23:
				currSkeleton->m_joints.insert({ Joint::LEG_R, Joint(pos, rot, confidence) });
				break;

			case 24:
				currSkeleton->m_joints.insert({ Joint::FOOT_R, Joint(pos, rot, confidence) });
				break;

			case 25:
				currSkeleton->m_joints.insert({ Joint::TOE_R, Joint(pos, rot, confidence) });
				break;

			case 26:
				currSkeleton->m_joints.insert({ Joint::HEAD, Joint(pos, rot, confidence) });
				break;

			default:
				break;
		}
	}

	// set body heigt based on head position
	currSkeleton->setHeight(currSkeleton->m_joints[Joint::HEAD].getJointPosition().y());

	// Console::log("Parsed skeleton with id = " + std::to_string(currSkeleton->getSid()) + " joint count = " + std::to_string(currSkeleton->m_joints.size()) + ".");

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

		//skeleton was added/removed, so UI updates
		m_hasSkeletonPoolChanged = true;

		Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::cleanSkeletonList(): Removed skeleton with id = " + std::to_string(*itIndexIdSkeletonsToErase) + " from pool!");

		if (m_skeletonPool.size() == 0)
		{

			Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::cleanSkeletonList(): No skeletons detected.");

		}
		else
		{

			Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::cleanSkeletonList(): Skeleton pool count = " + std::to_string(m_skeletonPool.size()) + ".");

		}
	}
}