#include "OTTracker.h"
#include "DataHandlerManager.h"


OTTracker::OTTracker()
{

}

OTTracker::OTTracker(int id)
{

	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_optiTrack_" + std::to_string(id);

	//default is enabled
	m_properties->isEnabled = true;
}


int OTTracker::createClient(int iConnectionType)
{

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
	printf("NatNet Sample Client (NatNet ver. %d.%d.%d.%d)\n", ver[0], ver[1], ver[2], ver[3]);
	// Init Client and connect to NatNet server
	// to use NatNet default port assignments
	int retCode = m_client->Initialize(szMyIPAddress, szServerIPAddress);
	// to use a different port for commands and/or data:
	//int retCode = theClient->Initialize(szMyIPAddress, szServerIPAddress, MyServersCommandPort, MyServersDataPort);
	if (retCode != ErrorCode_OK)
	{
		printf("Unable to connect to server.  Error code: %d. Exiting", retCode);
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
			printf("Analog Samples Per Mocap Frame : %d", analogSamplesPerMocapFrame);
		}
		// print server info
		sServerDescription ServerDescription;
		memset(&ServerDescription, 0, sizeof(ServerDescription));
		m_client->GetServerDescription(&ServerDescription);
		if (!ServerDescription.HostPresent)
		{
			printf("Unable to connect to server. Host not present. Exiting.");
			return 1;
		}

		//TODO: bring Console outputs to work!
		//Console::log("[SampleClient] Server application info:");
		//Console::log("Application: " + std::string(ServerDescription.szHostApp) + " (ver. " + std::string(reinterpret_cast<char*>(ServerDescription.HostAppVersion[1])) + "." + std::string(reinterpret_cast<char*>(ServerDescription.HostAppVersion[2])) + "." + std::string(reinterpret_cast<char*>(ServerDescription.HostAppVersion[3])) + ")");
		//Console::log("NatNet Version: " + std::string(reinterpret_cast<char*>(ServerDescription.NatNetVersion[1])) + "." + std::string(reinterpret_cast<char*>(ServerDescription.NatNetVersion[2])) + "." + std::string(reinterpret_cast<char*>(ServerDescription.NatNetVersion[3])));
		//Console::log("Client IP: " + szMyIPAddress);
		//Console::log("Server IP: " + std::string(reinterpret_cast<char*>(szServerIPAddress)));
		//Console::log("Server Name: " + std::string(reinterpret_cast<char*>(ServerDescription.szHostComputerName)));
	}

	return ErrorCode_OK;
}


void OTTracker::start()
{

	// set tracking to true
	m_properties->isTracking = true;

	//init NatNet client
	createClient(iConnectionType);

	//get reference to the frame data
	m_refData = m_dataHandlerManager->getData();


	//start update() in new thread
	m_trackingThread = new std::thread(&OTTracker::update, this);
	m_trackingThread->detach();

}

void OTTracker::update()
{
	
	Console::log("OTTracker::track(): Started tracking thread.");

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

	Console::log("OTTracker::track(): Stopped tracking thread.");
	
}

void OTTracker::track()
{

	//when new data wasn't fetched yet, dont update
	if (m_isDataAvailable)
	{
		
		return;

	}

	//when frame data wasn't initialized, try again later
	if (m_refData == nullptr)
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		return;

	}

	//get skeleton data from frame data
	extractSkeleton();

	//new data is ready
	m_isDataAvailable = true;

}

// stop tracking loop / thread
void OTTracker::stop()
{
	//is not tracking, so the update loop exits 
	m_properties->isTracking = false;

	delete m_dataHandlerManager;
	delete m_client;

	Console::log("Deleted Object");

}

// shutdown and destroy azure kinect tracker
void OTTracker::destroy()
{

	Console::log("OTTracker::destroy(): Destroyed tracker.");

	// delete this object
	delete this;

}

OTTracker::~OTTracker()
{



}


void OTTracker::extractSkeleton()
{

	//when new data isn't available, don't go on
	if (!m_dataHandlerManager->isDataAvailable())
	{
		return;
	}

	//get current skeleton number
	m_properties->countDetectedSkeleton = m_refData->nSkeletons;

	//loop through all OptiTrack skeletons
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

				//convert OptiTrack skeleton into MMH skeleton
				Skeleton* currSkeleton = parseSkeleton(skData, skData.skeletonID);

				// update all joints of existing skeleon with new data
				(m_skeletonPool)[skData.skeletonID]->m_joints = currSkeleton->m_joints;

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
			m_skeletonPool.insert(std::pair<int, Skeleton*>(skData.skeletonID, parseSkeleton(skData, skData.skeletonID)));



			//skeleton was added/removed, so UI updates
			m_hasSkeletonPoolChanged = true;



			Console::log("DataHandlerManager::extractSkeleton(): Created new skeleton with id = " + std::to_string(skData.skeletonID) + ".");

		}
	}
}


Skeleton* OTTracker::parseSkeleton(sSkeletonData skeleton, int id)
{


	// skeleton data container
	Skeleton* currSkeleton = new Skeleton(id);

	//loop through all joints
	for (int j = 0; j < skeleton.nRigidBodies; j++)
	{
		//temporary OptiTrack joint data object
		sRigidBodyData rbData = skeleton.RigidBodyData[j];


		// convert from k4a Vectors and quaternions into custom vectors
		Vector3 pos = Vector3(-rbData.x, rbData.y, rbData.z);
		Vector4 rot = Vector4(rbData.qx, rbData.qy, rbData.qz, rbData.qw);

		//confidence values are not transmitted, default confidence is High
		Joint::JointConfidence confidence = Joint::JointConfidence::HIGH;


		//map the OptiTRack poses to the MMH skeleton joints
		switch (j)
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
			currSkeleton->m_joints.insert({ Joint::HEAD, Joint(pos, rot, confidence) });
			break;

		case 5:
			currSkeleton->m_joints.insert({ Joint::SHOULDER_L, Joint(pos, rot, confidence) });
			break;

		case 6:
			currSkeleton->m_joints.insert({ Joint::ARM_L, Joint(pos, rot, confidence) });
			break;

		case 7:
			currSkeleton->m_joints.insert({ Joint::FOREARM_L, Joint(pos, rot, confidence) });
			break;

		case 8:
			currSkeleton->m_joints.insert({ Joint::HAND_L, Joint(pos, rot, confidence) });
			break;

		case 9:
			currSkeleton->m_joints.insert({ Joint::SHOULDER_R, Joint(pos, rot, confidence) });
			break;

		case 10:
			currSkeleton->m_joints.insert({ Joint::ARM_R, Joint(pos, rot, confidence) });
			break;

		case 11:
			currSkeleton->m_joints.insert({ Joint::FOREARM_R, Joint(pos, rot, confidence) });
			break;

		case 12:
			currSkeleton->m_joints.insert({ Joint::HAND_R, Joint(pos, rot, confidence) });
			break;

		case 13:
			currSkeleton->m_joints.insert({ Joint::UPLEG_L, Joint(pos, rot, confidence) });
			break;

		case 14:
			currSkeleton->m_joints.insert({ Joint::LEG_L, Joint(pos, rot, confidence) });
			break;

		case 15:
			currSkeleton->m_joints.insert({ Joint::FOOT_L, Joint(pos, rot, confidence) });
			break;

		case 16:
			currSkeleton->m_joints.insert({ Joint::UPLEG_R, Joint(pos, rot, confidence) });
			break;

		case 17:
			currSkeleton->m_joints.insert({ Joint::LEG_R, Joint(pos, rot, confidence) });
			break;

		case 18:
			currSkeleton->m_joints.insert({ Joint::FOOT_R, Joint(pos, rot, confidence) });
			break;

		case 19:
			currSkeleton->m_joints.insert({ Joint::TOE_L, Joint(pos, rot, confidence) });
			break;

		case 20:
			currSkeleton->m_joints.insert({ Joint::TOE_R, Joint(pos, rot, confidence) });
			break;

		default:
			break;
		}
	}



	// set body heigt based on head position
	currSkeleton->setHeight(currSkeleton->m_joints[Joint::HEAD].getJointPosition().m_xyz.y);


	//return skeleto with correct joint poses
	return currSkeleton;

}


void OTTracker::cleanSkeletonPool()
{

	//all skeletons with ids in this list will be erased at the end of this method
	std::list<int> idSkeletonsToErase;

	// loop through all skeletons in pool
	for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
	{

		// get current skeleton id
		int idCurrPoolSkeleton = itPoolSkeletons->first;
		bool isOTSkeletonInPool = false;

		//loop thorugh all OT skeletons in frame
		for (int itOTSkeletons = 0; itOTSkeletons < m_refData->nSkeletons; itOTSkeletons++)
		{

			// if OT skeleton is in pool set isOTSkeletonInPool to true
			if (idCurrPoolSkeleton == itOTSkeletons)
			{
				isOTSkeletonInPool = true;
			}
		}

		//if current skeleton isn't in current data frame, delete it from the pool later
		if (!isOTSkeletonInPool)
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


		Console::log("OTTracker::cleanSkeletonList(): Removed skeleton with id = " + std::to_string(itIndexIdSkeletonsToErase) + " from pool!");

		Console::log("OTTracker::cleanSkeletonList(): Skeleton pool count = " + std::to_string(m_skeletonPool.size()) + ".");
	}
}



// MessageHandler receives NatNet error/debug messages
void MessageHandler(int msgType, char* msg)
{
	Console::log("MessageHandler(): " + std::string(msg));
}