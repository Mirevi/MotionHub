#include "XSTracker.h"
#include "xs_streaming_protocol/xs_udpserver.h"
#include "xs_streaming_protocol/xs_streamer.h"
#include "DataHandlerManager.h"
#include <conio.h>
#include <xstypes/xstime.h>


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

	//tracker is enabled
	m_properties->isEnabled = true;





	//set default values for offsets
	setPositionOffset(Vector3f(configManager->getFloatFromStartupConfig("xPosXsens"),
		configManager->getFloatFromStartupConfig("yPosXsens"),
		configManager->getFloatFromStartupConfig("zPosXsens")
	));

	setRotationOffset(Vector3f(configManager->getFloatFromStartupConfig("xRotXsens"),
		configManager->getFloatFromStartupConfig("yRotXsens"),
		configManager->getFloatFromStartupConfig("zRotXsens")
	));

	setScaleOffset(Vector3f(configManager->getFloatFromStartupConfig("xSclXsens"),
		configManager->getFloatFromStartupConfig("ySclXsens"),
		configManager->getFloatFromStartupConfig("zSclXsens")
	));



	m_idCam = -1;



}

XSTracker::~XSTracker()
{



}

// start Xsens tracker
void XSTracker::start()
{

	// set tracking to true
	m_properties->isTracking = true;

	createClient(iConnectionType);


	//get reference to the frame data
	m_refData = m_dataHandlerManager->getData();

	// start tracking thread and detach the thread from method scope runtime
	m_trackingThread = new std::thread(&XSTracker::update, this);
	m_trackingThread->detach();

}

// stop tracking loop / thread
void XSTracker::stop()
{
	delete m_dataHandlerManager;
	//is not tracking, so the update loop exits after current loop
	m_properties->isTracking = false;

}

// shutdown and destroy xsens tracker
void XSTracker::destroy()
{
	// delete this object
	delete this;

}


void XSTracker::init()
{
	//no init code. The receiver is created on start, because it cannot be deactivated/paused
}


int XSTracker::createClient(int iConnectionType)
{

	// print xsens data
	Console::log("Das ist der erste Build-Versuch!");
	std::string hostDestinationAddress = "localhost";
	int port = 9763;

	std::cout << "Das ist der erste Build-Versuch: \nWaiting to receive packets from the client on port " << port << " ..." << std::endl << std::endl;

	//UdpServer udpServer(hostDestinationAddress, (uint16_t)port);

	////m_UdpServer = new UdpServer(hostDestinationAddress, (uint16_t)port);
	//m_UdpServer(hostDestinationAddress, (uint16_t)port);
	

	//while (!_kbhit())
	//	XsTime::msleep(10);

	// create NatNet client
	m_client = new NatNetClient(iConnectionType);

	// set the callback handlers
	m_client->SetVerbosityLevel(Verbosity_Warning);
	m_client->SetMessageCallback(MessageHandler);



	//create dummy object for MessageHandler
	m_dataHandlerManager = new DataHandlerManager(m_properties);

	//set callback with dummy object
	m_client->SetDataCallback(m_dataHandlerManager->DataHandler, m_client);	// this function will receive data from the server



	// print version info
	unsigned char ver[4];
	m_client->NatNetVersion(ver);

	Console::log("XSTracker::createClient(): Created NatNet Client (NatNet ver. " + std::to_string(ver[0]) + "." + std::to_string(ver[1]) + "." + std::to_string(ver[2]) + "." + std::to_string(ver[3]) + ")");

	// Init Client and connect to NatNet server
	// to use NatNet default port assignments
	int retCode = m_client->Initialize(szMyIPAddress, szServerIPAddress);

	// to use a different port for commands and/or data:
	if (retCode != ErrorCode_OK)
	{

		Console::log("OTTracker::createClient(): Unable to connect to server. Error code: " + std::to_string(retCode) + ". Exiting");
		return ErrorCode_Internal;

	}
	else
	{
		// get # of analog samples per mocap frame of data
		void* pResult;
		int ret = 0;
		int nBytes = 0;
		ret = m_client->SendMessageAndWait("AnalogSamplesPerMocapFrame", &pResult, &nBytes);

		if (ret == ErrorCode_OK)
		{
			analogSamplesPerMocapFrame = *((int*)pResult);
			Console::log("OTTracker::createClient(): Analog Samples Per Mocap Frame : " + std::to_string(analogSamplesPerMocapFrame) + ".");
		}

		// print server info
		sServerDescription ServerDescription;
		memset(&ServerDescription, 0, sizeof(ServerDescription));
		m_client->GetServerDescription(&ServerDescription);
		if (!ServerDescription.HostPresent)
		{
			Console::log("OTTracker::createClient(): Unable to connect to server. Host not present. Exiting.");
			return 1;
		}

	}

	return ErrorCode_OK;

}

// tracking loop
void XSTracker::update()
{

	// track while tracking is true
	while (m_properties->isTracking)
	{


		// get new data
		track();

		//send Skeleton Pool to NetworkManager
		m_networkManager->sendSkeletonPool(&m_skeletonPool, m_properties->id);

	}

	//clean skeleton pool after tracking
	clean();

}

// get new skeleton data and parse it into the default skeleton
void XSTracker::track()
{

	//Console::log("XSTracker::track()");
		//when frame data wasn't initialized, try again later
	if (m_refData == nullptr)
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		return;

	}

	//get skeleton data from frame data
	extractSkeleton();

	cleanSkeletonPool();

	//increase tracking cycle counter
	m_trackingCycles++;

	//new data is ready
	m_isDataAvailable = true;

}

void XSTracker::extractSkeleton()
{

	//when new data isn't available, skip this method run
	if (!m_dataHandlerManager->isDataAvailable())
	{

		return;

	}


	//get current skeleton number
	m_properties->countDetectedSkeleton = m_refData->nSkeletons;

	//loop through all Xsens skeletons
	for (int i = 0; i < m_refData->nSkeletons; i++)
	{
		//get current skeleton data
		sSkeletonData skData = m_refData->Skeletons[i];

		//true as long new skeleton will be added to the pool
		bool createNewSkeleton = true;

		//loop through all MMH skeletons
		for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
		{
			//when skeletons have the same ID, the skeleton is alredy in pool and no new skeleton has to be created
			if (skData.skeletonID == itPoolSkeletons->first)
			{

				//convert Xsens skeleton into MMH skeleton
				Skeleton* currSkeleton = parseSkeleton(skData, skData.skeletonID, &m_skeletonPool[skData.skeletonID]);

				if (currSkeleton != nullptr)
				{

					// update all joints of existing skeleon with new data
					m_skeletonPool[skData.skeletonID].m_joints = currSkeleton->m_joints;

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
			m_skeletonPool.insert({ skData.skeletonID, *parseSkeleton(skData, skData.skeletonID, new Skeleton()) });

			//skeleton was added/removed, so UI updates
			m_hasSkeletonPoolChanged = true;

			Console::log("DataHandlerManager::extractSkeleton(): Created new skeleton with id = " + std::to_string(skData.skeletonID) + ".");

		}

	}

}

//takes data from a k4a skeleton and pushes it into the list
Skeleton* XSTracker::parseSkeleton(sSkeletonData skeleton, int id, Skeleton* oldSkeletonData)
{

	//skeleton data container
	Skeleton* currSkeleton = new Skeleton(id);

	//loop through all joints
	for (int j = 0; j < skeleton.nRigidBodies; j++)
	{
		//temporary OptiTrack joint data object
		sRigidBodyData rbData = skeleton.RigidBodyData[j];

		// convert from k4a Vectors and quaternions into custom vectors
		Vector4f pos = m_offsetMatrix * Vector4f(rbData.x, rbData.y, rbData.z, 1.0f);
		Quaternionf rot = Quaternionf(rbData.qw, rbData.qx, -rbData.qy, -rbData.qz);

		//confidence values are not transmitted, default confidence is High
		Joint::JointConfidence confidence = Joint::JointConfidence::HIGH;


		//filter for corrupt position values  
		if (pos.x() > 100 || pos.y() > 100 || pos.z() > 100 ||
			pos.x() < -100 || pos.y() < -100 || pos.z() < -100)
		{

			return nullptr;

		}

		//filter for corrupt rotation values
		if (rot.x() < -1 || rot.y() < -1 || rot.z() < -1 || rot.w() < -1 ||
			rot.x() > 1 || rot.y() > 1 || rot.z() > 1 || rot.w() > 1)
		{

			return nullptr;

		}

		//map the Xsens poses to the MMH skeleton joints
		switch (j)
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

	//return skeleto with correct joint poses
	return currSkeleton;

}

// erase all unused skeletons from pool
void XSTracker::cleanSkeletonPool()
{

	//all skeletons with ids in this list will be erased at the end of this method
	std::list<int> idSkeletonsToErase;

	// loop through all skeletons in pool
	for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
	{

		// get current skeleton id
		int idCurrPoolSkeleton = itPoolSkeletons->first;

		bool isXSSkeletonInPool = false;

		//loop thorugh all XS skeletons in frame
		for (int itXSSkeletons = 0; itXSSkeletons < m_refData->nSkeletons; itXSSkeletons++)
		{

			// if XS skeleton is in pool set isXSSkeletonInPool to true
			if (idCurrPoolSkeleton == itXSSkeletons)
			{

				isXSSkeletonInPool = true;

			}

		}

		//if current skeleton isn't in current data frame, delete it from the pool later
		if (!isXSSkeletonInPool)
		{

			idSkeletonsToErase.push_back(idCurrPoolSkeleton);

		}

	}


	//loop through the removing list
	for (int itIndexIdSkeletonsToErase = idSkeletonsToErase.front(); itIndexIdSkeletonsToErase != idSkeletonsToErase.back(); itIndexIdSkeletonsToErase++)
	{

		//erase skeleton with id
		m_skeletonPool.erase(itIndexIdSkeletonsToErase);

		//skeleton was added/removed, so UI updates
		m_hasSkeletonPoolChanged = true;

		Console::log("XSTracker::cleanSkeletonList(): Removed skeleton with id = " + std::to_string(itIndexIdSkeletonsToErase) + " from pool!");

	}
}

Quaternionf XSTracker::convertXsensRotation(Quaternionf value)
{

	//converts the euler rotation to quaternion and multiplying it with input
	return eulerToQuaternion(Vector3f(0.0f, 180.0f, 0.0f)) * value;

}


std::string XSTracker::getTrackerType()
{

	return "Xsens";

}