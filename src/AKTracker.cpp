#include "AKTracker.h"

AKTracker::AKTracker()
{

	init(K4A_DEVICE_CONFIG_INIT_DISABLE_ALL);

}

AKTracker::AKTracker(k4a_device_configuration_t configDevice, int idCam)
{

	m_idCam = idCam;
	init(configDevice);

}

void AKTracker::init(k4a_device_configuration_t configDevice)
{

	cam = NULL;
	VERIFY_K4A_FUNCTION(k4a_device_open(m_idCam, &cam), "[cam id = " + std::to_string(m_idCam) + "] + Open K4A Device failed!");

	// Start camera. Make sure depth camera is enabled.
	configCam = configDevice;
	configCam.depth_mode = K4A_DEPTH_MODE_WFOV_2X2BINNED;
	configCam.color_resolution = K4A_COLOR_RESOLUTION_OFF;
	VERIFY_K4A_FUNCTION(k4a_device_start_cameras(cam, &configCam), "[cam id = " + std::to_string(m_idCam) + "] + Start K4A cameras failed!");

	VERIFY_K4A_FUNCTION(k4a_device_get_calibration(cam, configCam.depth_mode, configCam.color_resolution, &calibrationCam), "[cam id = " + std::to_string(m_idCam) + "] + Get depth camera calibration failed!");

	tracker = NULL;
	configTracker = K4ABT_TRACKER_CONFIG_DEFAULT;
	VERIFY_K4A_FUNCTION(k4abt_tracker_create(&calibrationCam, configTracker, &tracker), "[cam id = " + std::to_string(m_idCam) + "] + Body tracker initialization failed!");

}

void AKTracker::start()
{

	Tracker::start();

}

void AKTracker::stop()
{

	Tracker::stop();



	k4abt_tracker_shutdown(tracker);
	k4abt_tracker_destroy(tracker);
	k4a_device_stop_cameras(cam);
	k4a_device_close(cam);

	Console::log("[cam id = " + std::to_string(m_idCam) + "] AKTracker::stop(): Stopped body tracking!");

}

void AKTracker::track()
{

	k4a_capture_t sensor_capture;
	k4a_wait_result_t get_capture_result = k4a_device_get_capture(cam, &sensor_capture, K4A_WAIT_INFINITE);

	if (get_capture_result == K4A_WAIT_RESULT_SUCCEEDED)
	{

		k4a_wait_result_t queue_capture_result = k4abt_tracker_enqueue_capture(tracker, sensor_capture, K4A_WAIT_INFINITE);
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
		k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(tracker, &body_frame, K4A_WAIT_INFINITE);
		if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED)
		{
			// Successfully popped the body tracking result. Start your processing

			//update all skeletons with current data in body_frame
			updateSkeletons(&body_frame);

			//clean up skeleton pool - remove inactive skeletons
			cleanSkeletonList(&body_frame);

			//for (auto itPoolSkeletons = poolSkeletons.begin(); itPoolSkeletons != poolSkeletons.end(); itPoolSkeletons++)
			//{

			//	Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::track(): Skeleton with id = " + std::to_string(itPoolSkeletons->first) + ", pelvis position = " + itPoolSkeletons->second.m_joints[(Joint::jointNames)0].getJointPosition().toString());

			//}

			k4abt_frame_release(body_frame); // Remember to release the body frame once you finish using it
		}
		else if (pop_frame_result == K4A_WAIT_RESULT_TIMEOUT)
		{
			//  It should never hit timeout when K4A_WAIT_INFINITE is set.
			Console::logError("[cam id = " + std::to_string(m_idCam) + "] Pop body frame result timeout!");
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

//loops through all k4a skeletons and gives all active skeletons into parsing method
void AKTracker::updateSkeletons(k4abt_frame_t* body_frame)
{
	m_numBodies = k4abt_frame_get_num_bodies(*body_frame);
	//Console::log(std::to_string(m_numBodies) + " bodies found");

	for (size_t indexSkeleton = 0; indexSkeleton < m_numBodies; indexSkeleton++)
	{
		//get the skeleton and the id
		k4abt_skeleton_t skeleton;
		k4abt_frame_get_body_skeleton(*body_frame, indexSkeleton, &skeleton);
		uint32_t id = k4abt_frame_get_body_id(*body_frame, indexSkeleton);

		if (id > m_idCurrMaxSkeletons)
		{
			m_idCurrMaxSkeletons = id;

			poolSkeletons.insert(std::pair<int, Skeleton>(id, *parseSkeleton(&skeleton, id)));

			Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::updateSkeleton(): Created new skeleton with id = " + std::to_string(id) + ".");

		}
		else
		{
			poolSkeletons[id] = *parseSkeleton(&skeleton, id);

			//Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::updateSkeleton(): Updated skeleton with id = " + std::to_string(id) + ".");

		}
	}
}

//takes data from a k4a skeleton and pushes it into the list
Skeleton* AKTracker::parseSkeleton(k4abt_skeleton_t* skeleton, int id)
{
	Skeleton* currSkeleton = new Skeleton(id);

	//loop through all joints, get the position and rotation and pass them into the joint map
	for (size_t jointIndex = 0; jointIndex < K4ABT_JOINT_COUNT; jointIndex++)
	{
		k4a_float3_t skeleton_position = skeleton->joints->position;
		k4a_quaternion_t skeleton_rotation = skeleton->joints->orientation;

		//convert from k4a Vectors and quaternions into custom vectors
		Vector3 pos = Vector3(skeleton_position.xyz.x, skeleton_position.xyz.y, skeleton_position.xyz.z);
		Vector4 rot = Vector4(skeleton_rotation.wxyz.x, skeleton_rotation.wxyz.y, skeleton_rotation.wxyz.z, skeleton_rotation.wxyz.w);

		//save joint data in skeleton object
		currSkeleton->m_joints.insert({ (Joint::jointNames)jointIndex, Joint(pos, rot) });

	}

	return currSkeleton;
}

//erases all unused skeletons from list
void AKTracker::cleanSkeletonList(k4abt_frame_t* bodyFrame)
{
	//all skeletons with ids in ths list will be erased at the end of this method
	std::list<int> idSkeletonsToErase;

	//loop though all skeletons in pool
	for (auto itPoolSkeletons = poolSkeletons.begin(); itPoolSkeletons != poolSkeletons.end(); itPoolSkeletons++)
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
		poolSkeletons.erase(*itIndexIdSkeletonsToErase);

		Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::cleanSkeletonList(): Removed skeleton with id = " + std::to_string(*itIndexIdSkeletonsToErase) + " from pool!");

		if(poolSkeletons.size() == 0)
			Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::cleanSkeletonList(): Pool closed! No skeletons detected.");
		else
			Console::log("[cam id = " + std::to_string(m_idCam) + "] AkTracker::cleanSkeletonList(): Skeleton pool count = " + std::to_string(poolSkeletons.size()) + ".");

	}
}