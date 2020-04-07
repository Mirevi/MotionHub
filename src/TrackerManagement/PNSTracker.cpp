#include "PNSTracker.h"

// default constructor
PNSTracker::PNSTracker(int id)
{

	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_perceptionNeuronStudio_" + std::to_string(id);

	//tracker is enabled
	m_properties->isEnabled = true;

	//set default values for offsets
	setPositionOffset(Vector3f(0.0f, 0.0f, 0.0f));
	setRotationOffset(Vector3f(0.0f, 0.0f, 0.0f));
	setScaleOffset(Vector3f(0.01f, 0.01f, 0.01f));

	// initialize udp server
	init();

}

// start azure kinect tracker
void PNSTracker::start()
{

	// set tracking to true
	m_properties->isTracking = true;

	// start tracking thread and detach the thread from method scope runtime
	m_trackingThread = new std::thread(&PNSTracker::update, this);
	m_trackingThread->detach();

}

// stop tracking loop / thread
void PNSTracker::stop()
{

	// Close socket
	closesocket(inSocket);

	// Shutdown winsock
	WSACleanup();

	//is not tracking, so the update loop exits after current loop
	m_properties->isTracking = false;

}

// shutdown and destroy azure kinect tracker
void PNSTracker::destroy()
{

	// delete this object
	delete this;

}

void PNSTracker::init()
{

	// INITIALIZE WINSOCK

	// structure to store the WinSock version
	// this is filled in on the call to WSAStartup()
	WSADATA data;

	// to start WinSock, the required version must be passed to WSAStartup()
	// this server is going to use WinSock version 2 so i create a word that will store 2 and 2 in hex i.e. 0x0202
	WORD version = MAKEWORD(2, 2);

	// start WinSock
	int errCodeSocket = WSAStartup(version, &data);

	if (errCodeSocket != 0)
	{
		Console::logError("PNSTracker::init(): Error starting WinSock = " + errCodeSocket);
		return;
	}

	// SOCKET CREATION AND BINDING

	// create a socket, notice that it is a user datagram socket (UDP)
	inSocket = socket(AF_INET, SOCK_DGRAM, 0);

	// Create a server hint structure for the server
	sockaddr_in socketSettings;
	socketSettings.sin_addr.S_un.S_addr = ADDR_ANY; // use any IP address available on the machine -> CHANGE TO LOCALHOST?
	socketSettings.sin_family = AF_INET; // address format is IPv4
	socketSettings.sin_port = htons(7001); // convert from little to big endian 54000

	// try and bind the socket to the IP and port
	if (bind(inSocket, (sockaddr*)&socketSettings, sizeof(socketSettings)) == SOCKET_ERROR)
	{
		Console::logError("PNSTracker::init(): Error binding WinSock = " + WSAGetLastError());
		return;
	}

	clientMemSize = sizeof(client);

}

// tracking loop
void PNSTracker::update()
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
void PNSTracker::track()
{

	// OPTIMAL SIZE??
	char rawData[1024 * 3];

	// clear the raw data variable
	ZeroMemory(rawData, 1024 * 3); 

	// wait and get raw data
	int errCodeSocket = recvfrom(inSocket, rawData, 1024 * 3, 0, (sockaddr*)&client, &clientMemSize);
	if (errCodeSocket == SOCKET_ERROR)
	{
		Console::logError("PNSTracker::track(): " + WSAGetLastError());
		return;
	}

	// extract skeletons from raw data and parse them into skeleton pool
	extractSkeleton(rawData);

	// clean up skeleton pool - remove inactive skeletons
	//cleanSkeletonPool();

	// increase tracking cycles count
	m_trackingCycles++;

	// set data available to true
	m_isDataAvailable = true;

}

// extract skeletons from udp data package and psuh them into skeleton pool
void PNSTracker::extractSkeleton(char rawData[])
{

	// PROCESS RAW DATA

	// replace "chr" with "000" in rawData
	rawData[2] = '0';
	rawData[3] = '0';
	rawData[4] = '0';

	// convert processed data to string
	std::string data(rawData);

	// split string into vector
	std::istringstream issData(data);
	std::vector<std::string> dataValues((std::istream_iterator<std::string>(issData)), std::istream_iterator<std::string>());

	// display the received data
	// Console::log("PNSTracker::track(): Data = " + data);
	// Console::log("PNSTracker::track(): Count data values = " + std::to_string(dataValues.size())); // dataValues.size() -> 357 values in total with header and footer
	// Console::log("PNSTracker::track(): Euler rotation hips = (" + dataValues.at(5) + ", " + dataValues.at(6) + ", " + dataValues.at(7) + ")");

	// calculate the count of all detected skeletons
	// m_properties->countDetectedSkeleton = dataValues.size() / 357;

	// get skeleton id from data values at index 1 in vector
	int skeletonId = std::stoi(dataValues.at(1));

	int sizeSkeletonPool = m_skeletonPool.size();

	if (sizeSkeletonPool == 1)
	{

		// update existing skeleon with new data
		m_skeletonPool[0].m_joints = parseSkeleton(dataValues, skeletonId).m_joints;

		// skeleton was added/removed, so UI updates
		m_hasSkeletonPoolChanged = true;

	}
	else if (sizeSkeletonPool == 0)
	{

		// create new skeleton and add it to the skeleton pool
		m_skeletonPool.insert({ skeletonId, parseSkeleton(dataValues, skeletonId) });

		// TMP -> currently there is only one tracking suit for testing available at MIREVI so that the detected count never exeeds 1
		m_properties->countDetectedSkeleton = 1;

		// skeleton was added/removed, so UI updates
		m_hasSkeletonPoolChanged = true;

		Console::log("PNSTracker::updateSkeleton(): Created new skeleton with id = " + std::to_string(skeletonId) + ".");

	}

	m_trackingCycles++;

}

// takes udp data package and returns default skeleton
Skeleton PNSTracker::parseSkeleton(std::vector<std::string> dataValues, int skeletonId)
{

	// remove header and footer from dataValues
	dataValues.erase(dataValues.begin());
	dataValues.erase(dataValues.begin());
	dataValues.pop_back();

	// Console::log("PNSTracker::track(): Count joints = " + std::to_string(dataValues.size() / 6)); // dataValues.size() -> 357 values in total with header and footer
	
	// skeleton data container
	Skeleton currSkeleton(skeletonId);

	for (int indexJoint = 0; indexJoint < 59; indexJoint++)
	{

		// parse position and apply offset matrix
		Vector4f jointPos = m_offsetMatrix * Vector4f(std::stof(dataValues.at(indexJoint * 6 + 0)),
													  std::stof(dataValues.at(indexJoint * 6 + 1)),
													  std::stof(dataValues.at(indexJoint * 6 + 2)), 1);

		// parse rotation
		Vector3f eulerJointRot(std::stof(dataValues.at(indexJoint * 6 + 3)),
							   std::stof(dataValues.at(indexJoint * 6 + 4)),
							   std::stof(dataValues.at(indexJoint * 6 + 5)));
		
		// convert rotation to quaternion
		Quaternionf jointRot = eulerToQuaternion(eulerJointRot);

		// set joint confidence to default HIGH
		Joint::JointConfidence jointConfidence = Joint::JointConfidence::HIGH;
		
		switch (indexJoint)
		{

			case 0:
				currSkeleton.m_joints.insert({ Joint::HIPS, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 1:
				currSkeleton.m_joints.insert({ Joint::UPLEG_R, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 2:
				currSkeleton.m_joints.insert({ Joint::LEG_R, Joint(jointPos, jointRot, jointConfidence) });
				break;
				 
			case 3:
				currSkeleton.m_joints.insert({ Joint::FOOT_R, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 4:
				currSkeleton.m_joints.insert({ Joint::UPLEG_L, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 5:
				currSkeleton.m_joints.insert({ Joint::LEG_L, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 6:
				currSkeleton.m_joints.insert({ Joint::FOOT_L, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 7:
				currSkeleton.m_joints.insert({ Joint::SPINE, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 9:
				currSkeleton.m_joints.insert({ Joint::CHEST, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 11:
				currSkeleton.m_joints.insert({ Joint::NECK, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 12:
				currSkeleton.m_joints.insert({ Joint::HEAD, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 13:
				currSkeleton.m_joints.insert({ Joint::SHOULDER_R, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 14:
				currSkeleton.m_joints.insert({ Joint::ARM_R, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 15:
				currSkeleton.m_joints.insert({ Joint::FOREARM_R, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 16:
				currSkeleton.m_joints.insert({ Joint::HAND_R, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 36:
				currSkeleton.m_joints.insert({ Joint::SHOULDER_L, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 37:
				currSkeleton.m_joints.insert({ Joint::ARM_L, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 38:
				currSkeleton.m_joints.insert({ Joint::FOREARM_L, Joint(jointPos, jointRot, jointConfidence) });
				break;

			case 39:
				currSkeleton.m_joints.insert({ Joint::ARM_L, Joint(jointPos, jointRot, jointConfidence) });
				break;

			default:
				break;
		}
	}

	// set body heigt based on head position
	currSkeleton.setHeight(currSkeleton.m_joints[Joint::HEAD].getJointPosition().y());

	// return parsed skeleton
	return currSkeleton;

}