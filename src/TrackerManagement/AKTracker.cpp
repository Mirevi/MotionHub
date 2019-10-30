#include "AKTracker.h"

AKTracker::AKTracker()
{



}

AKTracker::AKTracker(int idCam)
{

	m_idCam = idCam;
	init();

}

void AKTracker::init()
{

	m_cam = NULL;
	VERIFY_K4A_FUNCTION(k4a_device_open(m_idCam, &m_cam), "[cam id = " + std::to_string(m_idCam) + "] + Open K4A Device failed!");

	// Start camera. Make sure depth camera is enabled.
	m_configCam = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	m_configCam.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
	m_configCam.color_resolution = K4A_COLOR_RESOLUTION_OFF;
	VERIFY_K4A_FUNCTION(k4a_device_start_cameras(m_cam, &m_configCam), "[cam id = " + std::to_string(m_idCam) + "] + Start K4A cameras failed!");

	VERIFY_K4A_FUNCTION(k4a_device_get_calibration(m_cam, m_configCam.depth_mode, m_configCam.color_resolution, &m_calibrationCam), "[cam id = " + std::to_string(m_idCam) + "] + Get depth camera calibration failed!");

	m_tracker = NULL;
	m_configTracker = K4ABT_TRACKER_CONFIG_DEFAULT;
	VERIFY_K4A_FUNCTION(k4abt_tracker_create(&m_calibrationCam, m_configTracker, &m_tracker), "[cam id = " + std::to_string(m_idCam) + "] + Body tracker initialization failed!");

}

void AKTracker::start()
{

	m_isTracking = true;

	trackingThread = new std::thread(&AKTracker::track, this);
	trackingThread->detach();
	
}

void AKTracker::track()
{

	Console::log("[cam id = " + std::to_string(m_idCam) + "] AKTracker::start(): Started tracking thread.");

	while (m_isTracking)
	{

		if(!m_isDataAvailable)
			update();

	}

	Console::log("[cam id = " + std::to_string(m_idCam) + "] AKTracker::start(): Stopped tracking thread.");

}

void AKTracker::update()
{

	k4a_capture_t sensor_capture;
	k4a_wait_result_t get_capture_result = k4a_device_get_capture(m_cam, &sensor_capture, K4A_WAIT_INFINITE);

	if (get_capture_result == K4A_WAIT_RESULT_SUCCEEDED)
	{

		k4a_wait_result_t queue_capture_result = k4abt_tracker_enqueue_capture(m_tracker, sensor_capture, K4A_WAIT_INFINITE);
		k4a_capture_release(sensor_capture); // Remember to release the sensor capture once you finish using it
		if (queue_capture_result == K4A_WAIT_RESULT_TIMEOUT)
		{
			// It should never hit timeout when K4A_WAIT_INFINITE is set.
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Add capture to tracker process queue timeout!");
			return;
		}
		else if (queue_capture_result == K4A_WAIT_RESULT_FAILED)
		{
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Add capture to tracker process queue failed!");
			return;
		}

		k4abt_frame_t body_frame = NULL;
		k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(m_tracker, &body_frame, K4A_WAIT_INFINITE);
		if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED)
		{
			// Successfully popped the body tracking result. Start your processing

			//update all skeletons with current data in body_frame
			extractSkeleton(&body_frame);

			//clean up skeleton pool - remove inactive skeletons
			cleanSkeletonPool(&body_frame);

			//for (auto itPoolSkeletons = poolSkeletons.begin(); itPoolSkeletons != poolSkeletons.end(); itPoolSkeletons++)
			//{

			//	Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::track(): Skeleton with id = " + std::to_string(itPoolSkeletons->first) + ", pelvis position = " + itPoolSkeletons->second.m_joints[(Joint::jointNames)0].getJointPosition().toString());

			//}

			k4abt_frame_release(body_frame); // Remember to release the body frame once you finish using it

			m_isDataAvailable = true;


		}
		else if (pop_frame_result == K4A_WAIT_RESULT_TIMEOUT)
		{
			//  It should never hit timeout when K4A_WAIT_INFINITE is set.
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

void AKTracker::stop()
{

	m_isTracking = false;

}

void AKTracker::destroy()
{

	k4abt_tracker_shutdown(m_tracker);
	k4abt_tracker_destroy(m_tracker);
	k4a_device_stop_cameras(m_cam);
	k4a_device_close(m_cam);

	Console::log("[cam id = " + std::to_string(m_idCam) + "] AKTracker::destroy(): Destroyed tracker.");

	delete this;

}

//loops through all k4a skeletons and gives all active skeletons into parsing method
void AKTracker::extractSkeleton(k4abt_frame_t* body_frame)
{

	m_numBodies = k4abt_frame_get_num_bodies(*body_frame);

	for (int indexSkeleton = 0; indexSkeleton < m_numBodies; indexSkeleton++)
	{
		//get the skeleton and the id
		k4abt_skeleton_t skeleton;
		k4abt_frame_get_body_skeleton(*body_frame, indexSkeleton, &skeleton);
		uint32_t id = k4abt_frame_get_body_id(*body_frame, indexSkeleton);

		bool createNewSkeleton = true;

		// update existing skeleton
		for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
		{

			if (id = itPoolSkeletons->first)
			{

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

			m_skeletonPool.insert(std::pair<int, Skeleton*>(id, parseSkeleton(&skeleton, id)));

			Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::updateSkeleton(): Created new skeleton with id = " + std::to_string(id) + ".");

		}
	}
}

//takes data from a k4a skeleton and pushes it into the list
Skeleton* AKTracker::parseSkeleton(k4abt_skeleton_t* skeleton, int id)
{
	Skeleton* currSkeleton = new Skeleton(id);

	//loop through all joints, get the position and rotation and pass them into the joint map
	for (int jointIndex = 0; jointIndex < K4ABT_JOINT_COUNT; jointIndex++)
	{

		k4a_float3_t skeleton_position = skeleton->joints[jointIndex].position;
		k4a_quaternion_t skeleton_rotation = skeleton->joints[jointIndex].orientation;

		//convert from k4a Vectors and quaternions into custom vectors
		Vector3 pos = Vector3(skeleton_position.xyz.x, skeleton_position.xyz.y, skeleton_position.xyz.z);
		Vector4 rot = Vector4(skeleton_rotation.wxyz.x, skeleton_rotation.wxyz.y, skeleton_rotation.wxyz.z, skeleton_rotation.wxyz.w);
		Joint::JointConfidence confidence = (Joint::JointConfidence)skeleton->joints[jointIndex].confidence_level;

		//Console::log("Joint " + std::to_string(jointIndex) + " on position " + pos.toString());

		//if(confidence == Joint::JointConfidence::HIGH)
		//	Console::log("JOINT HAS HIGH CONFIDENCE!");

		switch (jointIndex)
		{

			case 0:
				currSkeleton->m_joints.insert({ Joint::HIPS, Joint(pos, rot, confidence) });
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

	currSkeleton->setHeight(currSkeleton->m_joints[Joint::HEAD].getJointPosition().m_xyz.y);

	// Console::log("Parsed skeleton with id = " + std::to_string(currSkeleton->getSid()) + " joint count = " + std::to_string(currSkeleton->m_joints.size()) + ".");

	return currSkeleton;
}

//erases all unused skeletons from list
void AKTracker::cleanSkeletonPool(k4abt_frame_t* bodyFrame)
{
	//all skeletons with ids in ths list will be erased at the end of this method
	std::list<int> idSkeletonsToErase;

	//loop though all skeletons in pool
	for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
	{	
		//current skeleton
		int idCurrPoolSkeleton = itPoolSkeletons->first;
		bool isK4aSkeletonInPool = false;

		//loop thorugh all k4a skeletons in frame
		for (int indexK4aSkeleton = 0; indexK4aSkeleton < m_numBodies; indexK4aSkeleton++)
		{
			//current k4a skeleton
			int idCurrK4aSkeleton = k4abt_frame_get_body_id(*bodyFrame, indexK4aSkeleton);

			//if 
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

		m_skeletonPool.erase(*itIndexIdSkeletonsToErase);

		Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::cleanSkeletonList(): Removed skeleton with id = " + std::to_string(*itIndexIdSkeletonsToErase) + " from pool!");

		if(m_skeletonPool.size() == 0)
			Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::cleanSkeletonList(): Pool closed! No skeletons detected.");
		else
			Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::cleanSkeletonList(): Skeleton pool count = " + std::to_string(m_skeletonPool.size()) + ".");

	}
}