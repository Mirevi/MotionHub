#include "PN2Tracker.h"

#include <iostream>
#include <WS2tcpip.h>
#include <vector>
#include <sstream>
#include <iterator>

// default constructor
PN2Tracker::PN2Tracker(int id)
{
	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_azureKinect_" + std::to_string(id);

	// assign cam id
	// m_idCam = idCam;

	//tracker is enabled
	m_properties->isEnabled = true;

	//set default values for offsets
	setPositionOffset(Vector3f(0.0f, 1.175f, 2.2f));
	setRotationOffset(Vector3f(-5.0f, 0.0f, 0.0f));
	setScaleOffset(Vector3f(0.0010f, -0.0010f, -0.0010f));

	// initialize azure kinect camera and body tracker
	init();

}

// start azure kinect tracker
void PN2Tracker::start()
{

	// set tracking to true
	m_properties->isTracking = true;

	// start tracking thread and detach the thread from method scope runtime
	m_trackingThread = new std::thread(&PN2Tracker::update, this);
	m_trackingThread->detach();




	////////////////////////////////////////////////////////////
	// INITIALIZE WINSOCK
	////////////////////////////////////////////////////////////

	// Structure to store the WinSock version. This is filled in
	// on the call to WSAStartup()
	WSADATA data;

	// To start WinSock, the required version must be passed to
	// WSAStartup(). This server is going to use WinSock version
	// 2 so I create a word that will store 2 and 2 in hex i.e.
	// 0x0202
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Not ok! Get out quickly
		std::cout << "Can't start Winsock! " << wsOk;
		return;
	}

	////////////////////////////////////////////////////////////
	// SOCKET CREATION AND BINDING
	////////////////////////////////////////////////////////////

	// Create a socket, notice that it is a user datagram socket (UDP)
	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);

	// Create a server hint structure for the server
	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // Us any IP address available on the machine
	serverHint.sin_family = AF_INET; // Address format is IPv4
	serverHint.sin_port = htons(7001); // Convert from little to big endian 54000

	// Try and bind the socket to the IP and port
	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		std::cout << "Can't bind socket! " << WSAGetLastError() << std::endl;
		return;
	}

	////////////////////////////////////////////////////////////
	// MAIN LOOP SETUP AND ENTRY
	////////////////////////////////////////////////////////////

	sockaddr_in client; // Use to hold the client information (port / ip address)
	int clientLength = sizeof(client); // The size of the client information

	char buf[1024 * 3];

}

// stop tracking loop / thread
void PN2Tracker::stop()
{
	//is not tracking, so the update loop exits after current loop
	m_properties->isTracking = false;

}

// shutdown and destroy azure kinect tracker
void PN2Tracker::destroy()
{

	// shutdown and destroy tracker
	//k4abt_tracker_shutdown(m_tracker);
	//k4abt_tracker_destroy(m_tracker);

	// stop and close camera
	//k4a_device_stop_cameras(m_cam);
	//k4a_device_close(m_cam);

	// delete this object
	delete this;

}

void PN2Tracker::init()
{

	///////////////////////////////////////////////////////////////m_cam = NULL;
	// open camera ---> ERROR:  libusb device(s) are all unavalable

	// setup camera config
	//m_configCam = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	//m_configCam.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
	//m_configCam.color_resolution = K4A_COLOR_RESOLUTION_OFF;
	// start camera
	//VERIFY_K4A_FUNCTION(k4a_device_start_cameras(m_cam, &m_configCam), "[cam id = " + std::to_string(m_idCam) + "] + Start K4A cameras failed!");

	// calibrate camera
	//VERIFY_K4A_FUNCTION(k4a_device_get_calibration(m_cam, m_configCam.depth_mode, m_configCam.color_resolution, &m_calibrationCam), "[cam id = " + std::to_string(m_idCam) + "] + Get depth camera calibration failed!");

	///////////////////////////////////////////////////////////////m_tracker = NULL;
	// setup tracker config
	//m_configTracker = K4ABT_TRACKER_CONFIG_DEFAULT;


	// create tracker
	//VERIFY_K4A_FUNCTION(k4abt_tracker_create(&m_calibrationCam, m_configTracker, &m_tracker), "[cam id = " + std::to_string(m_idCam) + "] + Body tracker initialization failed!");

}

// tracking loop
void PN2Tracker::update()
{

	// track while tracking is true
	while (m_properties->isTracking)
	{

		// if no new data is procressed
		if (!m_isDataAvailable)
		{

			// get new data
			track();

		}
	}

	//clean skeleton pool after tracking
	clean();

}

// get new skeleton data and parse it into the default skeleton
void PN2Tracker::track()
{

	// create sensor capture and result
	//k4a_capture_t sensor_capture;
	//k4a_wait_result_t get_capture_result = k4a_device_get_capture(m_cam, &sensor_capture, K4A_WAIT_INFINITE);

	// process capture if result succeeded
	//if (get_capture_result == K4A_WAIT_RESULT_SUCCEEDED)
	//{

	//	// add capture to body tracker process quene
	//	k4a_wait_result_t queue_capture_result = k4abt_tracker_enqueue_capture(m_tracker, sensor_capture, K4A_WAIT_INFINITE);
	//	k4a_capture_release(sensor_capture); // release the sensor capture once we finish using it

	//	// error handling quene capture timeout
	//	if (queue_capture_result == K4A_WAIT_RESULT_TIMEOUT)
	//	{
	//		// it should never hit timeout when K4A_WAIT_INFINITE is set
	//		Console::logError("[cam id = " + std::to_string(m_idCam) + "] Add capture to tracker process queue timeout!");
	//		return;

	//	}
	//	// error handling quene capture failure
	//	else if (queue_capture_result == K4A_WAIT_RESULT_FAILED)
	//	{
	//		Console::logError("[cam id = " + std::to_string(m_idCam) + "] Add capture to tracker process queue failed!");
	//		return;
	//	}

	//	// extract body frame out of capture
	//	k4abt_frame_t body_frame = NULL;
	//	k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(m_tracker, &body_frame, K4A_WAIT_INFINITE);

	//	// successfully popped the body tracking result
	//	if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED)
	//	{

	//		// extract skeletons from body frame and parse them into default skeleton pool
	//		extractSkeleton(&body_frame);



	//		// clean up skeleton pool - remove inactive skeletons
	//		cleanSkeletonPool(&body_frame);

	//		// remember to release the body frame once you finish using it
	//		k4abt_frame_release(body_frame); 

	//		//count tracking cycles
	//		m_trackingCycles++;

	//		// set data available to true
	//		m_isDataAvailable = true;


	//	}
	//	// error handling
	//	else if (pop_frame_result == K4A_WAIT_RESULT_TIMEOUT)
	//	{
	//		//  it should never hit timeout when K4A_WAIT_INFINITE is set
	//		Console::logError("[cam id = " + std::to_string(m_idCam) + "] Pop body frame result timeout!");
	//		m_isDataAvailable = false;

	//		return;
	//	}
	//	else
	//	{
	//		Console::logError("[cam id = " + std::to_string(m_idCam) + "] Pop body frame result failed!");
	//		return;
	//	}
	//}
	//else if (get_capture_result == K4A_WAIT_RESULT_TIMEOUT)
	//{
	//	// It should never hit time out when K4A_WAIT_INFINITE is set.
	//	Console::logError("[cam id = " + std::to_string(m_idCam) + "] Get depth frame time out!");
	//	return;
	//}
	//else
	//{
	//	Console::logError("[cam id = " + std::to_string(m_idCam) + "] Get depth capture returned error: " + std::to_string(get_capture_result));
	//	return;
	//}
}

// extract skeletons from body frame and parse them into default skeleton pool
//void PN2Tracker::extractSkeleton(k4abt_frame_t* body_frame)
//{
//
//	// set number of detected bodies in frame
//	m_properties->countDetectedSkeleton = k4abt_frame_get_num_bodies(*body_frame);
//
//	//Console::log(std::to_string(m_numBodies));
//
//	// skeleton loop
//	for (int indexSkeleton = 0; indexSkeleton < m_properties->countDetectedSkeleton; indexSkeleton++)
//	{
//
//		// get the skeleton and the id
//		k4abt_skeleton_t skeleton;
//		k4abt_frame_get_body_skeleton(*body_frame, indexSkeleton, &skeleton);
//		uint32_t id = k4abt_frame_get_body_id(*body_frame, indexSkeleton);
//
//		bool createNewSkeleton = true;
//
//		// update existing skeleton
//		for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
//		{
//
//			if (id == itPoolSkeletons->first)
//			{
//
//				// update all joints of existing skeleon with new data
//				m_skeletonPool[id].m_joints = parseSkeleton(&skeleton, id)->m_joints;
//
//				createNewSkeleton = false;
//
//				break;
//
//			}
//		}
//
//		// create new skeleton
//		if (createNewSkeleton)
//		{
//
//			// create new skeleton and add it to the skeleton pool
//			m_skeletonPool.insert({ id, *parseSkeleton(&skeleton, id) });
//			
//			//skeleton was added/removed, so UI updates
//			m_hasSkeletonPoolChanged = true;
//
//			Console::log("PN2Tracker::updateSkeleton(): [cam id = " + std::to_string(m_idCam) + "] Created new skeleton with id = " + std::to_string(id) + ".");
//
//		}
//	}
//}
//
////takes data from a k4a skeleton and pushes it into the list
//Skeleton* PN2Tracker::parseSkeleton(k4abt_skeleton_t* skeleton, int id)
//{
//
//	// skeleton data container
//	Skeleton* currSkeleton = new Skeleton(id);
//
//	// loop through all joints, get the position and rotation and pass them into the joint map
//	for (int jointIndex = 0; jointIndex < K4ABT_JOINT_COUNT; jointIndex++)
//	{
//
//		// get position and rotation
//		k4a_float3_t skeleton_position = skeleton->joints[jointIndex].position;
//		k4a_quaternion_t skeleton_rotation = skeleton->joints[jointIndex].orientation;
//
//		
//		// convert from k4a Vectors and quaternions into Eigen vector and quaternion with coordinate transformation
//		Vector4f pos	= m_offsetMatrix * Vector4f(skeleton_position.xyz.x, skeleton_position.xyz.y, skeleton_position.xyz.z, 1);
//		Quaternionf rot = Quaternionf(skeleton_rotation.wxyz.w, skeleton_rotation.wxyz.x, skeleton_rotation.wxyz.y, skeleton_rotation.wxyz.z);
//
//		// get joint confidence level from azure kinect body tracker API
//		Joint::JointConfidence confidence = (Joint::JointConfidence)skeleton->joints[jointIndex].confidence_level;
//
//		// map azure kinect skeleton joints to default skeleton joints and set confidence level
//		//rotations are converted from global to local
//		switch (jointIndex)
//		{
//
//			case 0:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::HIPS, Joint(pos, rot, confidence) });
//				break;
//
//			case 1:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::SPINE, Joint(pos, rot, confidence) });
//				break;
//
//			case 2:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::CHEST, Joint(pos, rot, confidence) });
//				break;
//				 
//			case 3:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::NECK, Joint(pos, rot, confidence) });
//				break;
//
//			case 4:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, 0, 0));
//				currSkeleton->m_joints.insert({ Joint::SHOULDER_L, Joint(pos, rot, confidence) });
//				break;
//
//			case 5:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, 0, 0));
//				currSkeleton->m_joints.insert({ Joint::ARM_L, Joint(pos, rot, confidence) });
//				break;
//
//			case 6:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, 0, 0));
//				currSkeleton->m_joints.insert({ Joint::FOREARM_L, Joint(pos, rot, confidence) });
//				break;
//
//			case 7:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(0, 180, 180));
//				currSkeleton->m_joints.insert({ Joint::HAND_L, Joint(pos, rot, confidence) });
//				break;
//
//			case 11:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(270, 0, 0));
//				currSkeleton->m_joints.insert({ Joint::SHOULDER_R, Joint(pos, rot, confidence) });
//				break;
//
//			case 12:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(270, 0, 0));
//				currSkeleton->m_joints.insert({ Joint::ARM_R, Joint(pos, rot, confidence) });
//				break;
//
//			case 13:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(270, 0, 0));
//				currSkeleton->m_joints.insert({ Joint::FOREARM_R, Joint(pos, rot, confidence) });
//				break;
//
//			case 14:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(0, 0, 0));
//				currSkeleton->m_joints.insert({ Joint::HAND_R, Joint(pos, rot, confidence) });
//				break;
//
//			case 18:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::UPLEG_L, Joint(pos, rot, confidence) });
//				break;
//
//			case 19:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::LEG_L, Joint(pos, rot, confidence) });
//				break;
//
//			case 20:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::FOOT_L, Joint(pos, rot, confidence) });
//				break;
//
//			case 21:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(0, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::TOE_L, Joint(pos, rot, confidence) });
//				break;
//
//			case 22:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(-90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::UPLEG_R, Joint(pos, rot, confidence) });
//				break;
//
//			case 23:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(-90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::LEG_R, Joint(pos, rot, confidence) });
//				break;
//
//			case 24:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(-90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::FOOT_R, Joint(pos, rot, confidence) });
//				break;
//
//			case 25:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(0, 90, 180));
//				currSkeleton->m_joints.insert({ Joint::TOE_R, Joint(pos, rot, confidence) });
//				break;
//
//			case 26:
//				 rot *= azureKinectEulerToQuaternion(Vector3f(90, -90, 0));
//				currSkeleton->m_joints.insert({ Joint::HEAD, Joint(pos, rot, confidence) });
//				break;
//
//			default:
//				break;
//		}
//	}
//
//	// set body heigt based on head position
//	currSkeleton->setHeight(currSkeleton->m_joints[Joint::HEAD].getJointPosition().y());
//
//
//	// return parsed default skeleton
//	return currSkeleton;
//
//}
//
//// erase all unused skeletons from pool
//void PN2Tracker::cleanSkeletonPool(k4abt_frame_t* bodyFrame)
//{
//
//	//all skeletons with ids in this list will be erased at the end of this method
//	std::list<int> idSkeletonsToErase;
//
//	// loop through all skeletons in pool
//	for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
//	{	
//
//		// get current skeleton id
//		int idCurrPoolSkeleton = itPoolSkeletons->first;
//		bool isK4aSkeletonInPool = false;
//
//		//loop thorugh all k4a skeletons in frame
//		for (int indexK4aSkeleton = 0; indexK4aSkeleton < m_properties->countDetectedSkeleton; indexK4aSkeleton++)
//		{
//			// current k4a skeleton id
//			int idCurrK4aSkeleton = k4abt_frame_get_body_id(*bodyFrame, indexK4aSkeleton);
//
//			// if k4a skeleton is in pool set isK4aSkeletonInPool to true
//			if (idCurrPoolSkeleton == idCurrK4aSkeleton)
//			{
//				isK4aSkeletonInPool = true;
//			}
//		}
//
//		if (!isK4aSkeletonInPool)
//		{
//			idSkeletonsToErase.push_back(idCurrPoolSkeleton);
//		}
//	}
//
//	for (auto itIndexIdSkeletonsToErase = idSkeletonsToErase.begin(); itIndexIdSkeletonsToErase != idSkeletonsToErase.end(); itIndexIdSkeletonsToErase++)
//	{
//
//		// erase skeleton with id
//		m_skeletonPool.erase(*itIndexIdSkeletonsToErase);
//
//		//skeleton was added/removed, so UI updates
//		m_hasSkeletonPoolChanged = true;
//
//		Console::log("PN2Tracker::cleanSkeletonList(): [cam id = " + std::to_string(m_idCam) + "] Removed skeleton with id = " + std::to_string(*itIndexIdSkeletonsToErase) + " from pool!");
//
//	}
//}