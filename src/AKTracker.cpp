#include "AKTracker.h"


//constructor
AKTracker::AKTracker()
{

	init(K4A_DEVICE_CONFIG_INIT_DISABLE_ALL);

}

AKTracker::AKTracker(k4a_device_configuration_t configDevice)
{

	init(configDevice);

}

void AKTracker::init(k4a_device_configuration_t configDevice)
{

	cam = NULL;
	VERIFY_K4A_FUNCTION(k4a_device_open(0, &cam), "Open K4A Device failed!");

	// Start camera. Make sure depth camera is enabled.
	configCam = configDevice;
	configCam.depth_mode = K4A_DEPTH_MODE_WFOV_2X2BINNED;
	configCam.color_resolution = K4A_COLOR_RESOLUTION_OFF;
	VERIFY_K4A_FUNCTION(k4a_device_start_cameras(cam, &configCam), "Start K4A cameras failed!");

	VERIFY_K4A_FUNCTION(k4a_device_get_calibration(cam, configCam.depth_mode, configCam.color_resolution, &calibrationCam), "Get depth camera calibration failed!");

	tracker = NULL;
	configTracker = K4ABT_TRACKER_CONFIG_DEFAULT;
	VERIFY_K4A_FUNCTION(k4abt_tracker_create(&calibrationCam, configTracker, &tracker), "Body tracker initialization failed!");

}

void AKTracker::start()
{

	Tracker::start();

	// mutex
	// atomic
	// observer pattern for trackerManager

}

void AKTracker::stop()
{

	Tracker::stop();

	Console::log("Finished body tracking processing!");

	k4abt_tracker_shutdown(tracker);
	k4abt_tracker_destroy(tracker);
	k4a_device_stop_cameras(cam);
	k4a_device_close(cam);
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
			Console::logError("Add capture to tracker process queue timeout!");
			return;
		}
		else if (queue_capture_result == K4A_WAIT_RESULT_FAILED)
		{
			Console::logError("Add capture to tracker process queue failed!");
			return;
		}

		k4abt_frame_t body_frame = NULL;
		k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(tracker, &body_frame, K4A_WAIT_INFINITE);
		if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED)
		{
			// Successfully popped the body tracking result. Start your processing


			//update all skeletons with current data in body_frame
			updateSkeletons(&body_frame);

			cleanSkeletonList(&body_frame);

			Console::log("pool size: " + std::to_string(poolSkeletons.size()));

			for (size_t i = 1; i <= poolSkeletons.size(); i++)
			{
				Console::log("skeleton " + std::to_string(i) + " position" + poolSkeletons[i].m_joints[(Joint::jointNames)0].getJointPosition().toString());
			}



			//Console::log("length of list: " + std::to_string(poolSkeletons.size()));
			//size_t num_bodies = k4abt_frame_get_num_bodies(body_frame);
			////printf("%zu bodies are detected!\n", num_bodies);
			//Console::log("AKTracker deteceted bodies = " + std::to_string(num_bodies));


			k4abt_frame_release(body_frame); // Remember to release the body frame once you finish using it
		}
		else if (pop_frame_result == K4A_WAIT_RESULT_TIMEOUT)
		{
			//  It should never hit timeout when K4A_WAIT_INFINITE is set.
			Console::logError("Pop body frame result timeout!");
			return;
		}
		else
		{
			Console::logError("Pop body frame result failed!");
			return;
		}
	}
	else if (get_capture_result == K4A_WAIT_RESULT_TIMEOUT)
	{
		// It should never hit time out when K4A_WAIT_INFINITE is set.
		Console::logError("Get depth frame time out!");
		return;
	}
	else
	{
		Console::logError("Get depth capture returned error: " + std::to_string(get_capture_result));
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

		if (id > highestSkeletonId)
		{
			highestSkeletonId = id;

			poolSkeletons.insert(std::pair<int, Skeleton>(id, *parseSkeleton(&skeleton, id)));
		}
		else
		{
			poolSkeletons[id] = *parseSkeleton(&skeleton, id);
		}

		//Console::log("length of list: " + std::to_string(poolSkeletons.size()));
		//Console::log("id:" + std::to_string(id));
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

		//if (i == 0)
		//{
		//	Console::log("Skeleton " + std::to_string(id) + " position: (" 
		//							 + std::to_string(pos.m_xyz.x) + ", " 
		//							 + std::to_string(pos.m_xyz.y) + ", " 
		//							 + std::to_string(pos.m_xyz.z) + ")");
		//}

		currSkeleton->m_joints.insert(std::pair<Joint::jointNames, Joint>((Joint::jointNames)jointIndex, Joint(pos, rot)));

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
	}
}