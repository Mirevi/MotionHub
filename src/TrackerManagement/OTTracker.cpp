#include "OTTracker.h"


std::atomic<int> g_rigidBodyDroneX;
std::atomic<int> g_rigidBodyDroneY;
std::atomic<int> g_rigidBodyDroneZ;
RigidBodyCollection g_rigidBodies;



#include "DataHandlerManager.h"


OTTracker::OTTracker(int id)
{

	//create new Properties object
	m_properties = new Properties();

	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_optiTrack_" + std::to_string(id);

	m_properties->isEnabled = true;
}





int OTTracker::createClient(int iConnectionType)
{
	//OBSCURE: Why 
	// release previous server
	//if (theClient)
	//{
	//	theClient->Uninitialize();
	//	delete theClient;
	//}
	// create NatNet client
	m_client = new NatNetClient(iConnectionType);
	// set the callback handlers

	m_client->SetVerbosityLevel(Verbosity_Warning);
	m_client->SetMessageCallback(MessageHandler);

	m_dataHandlerManager = new DataHandlerManager(m_properties);

	m_client->SetDataCallback(m_dataHandlerManager->DataHandler, m_client);	// this function will receive data from the server
	// [optional] use old multicast group
	//theClient->SetMulticastAddress("224.0.0.1");
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
		printf("[SampleClient] Server application info:\n");
		printf("Application: %s (ver. %d.%d.%d.%d)\n", ServerDescription.szHostApp, ServerDescription.HostAppVersion[0],
			ServerDescription.HostAppVersion[1], ServerDescription.HostAppVersion[2], ServerDescription.HostAppVersion[3]);
		printf("NatNet Version: %d.%d.%d.%d\n", ServerDescription.NatNetVersion[0], ServerDescription.NatNetVersion[1],
			ServerDescription.NatNetVersion[2], ServerDescription.NatNetVersion[3]);
		printf("Client IP:%s\n", szMyIPAddress);
		printf("Server IP:%s\n", szServerIPAddress);
		printf("Server Name:%s\n\n", ServerDescription.szHostComputerName);
	}

	return ErrorCode_OK;
}


void OTTracker::start()
{

	// set tracking to true
	m_properties->isTracking = true;

	createClient(iConnectionType);

	m_data = m_dataHandlerManager->getData();



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

			track(); // get new data

		}
	}

	//clean skeleton pool after tracking
	clean();

	Console::log("OTTracker::track(): Stopped tracking thread.");
	

}

void OTTracker::track()
{
	
	if (m_isDataAvailable)
	{
		return;

	}

	if (m_data == nullptr)
	{
		Console::logError("OTTracker::track(): m_data was nullptr!");

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		return;
	}

	extractSkeleton();

	m_isDataAvailable = true;

}

// stop tracking loop / thread
void OTTracker::stop()
{
	//is not tracking, so the update loop exits 
	m_properties->isTracking = false;

	Console::log("Deleting Object...");

	//theClient->Uninitialize();

	//theClient->~NatNetClient();

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
	//m_client->Uninitialize();
	//delete m_client;
	//delete m_dataHandlerManager;
}




void RigidBodyCollection::AppendRigidBodyData(sRigidBodyData const * const rigidBodyData, size_t numRigidBodies)
{
	for (size_t i = 0; i < numRigidBodies; ++i)
	{
		const sRigidBodyData& rb = rigidBodyData[i];
		mXYZCoord[i + mNumRigidBodies] = std::make_tuple(rb.x, rb.y, rb.z);
		mXYZWQuats[i + mNumRigidBodies] = std::make_tuple(rb.qx, rb.qy, rb.qz, rb.qw);
		mIds[i + mNumRigidBodies] = rb.ID;
	}
	mNumRigidBodies += numRigidBodies;
}


RigidBodyCollection::RigidBodyCollection() : mNumRigidBodies(0)
{
	;
}

// MessageHandler receives NatNet error/debug messages
void MessageHandler(int msgType, char* msg)
{
	printf("\n%s\n", msg);
}





void OTTracker::extractSkeleton()
{

	m_properties->countDetectedSkeleton = m_data->nSkeletons;

	//loop through all skeletons
	for (int i = 0; i < m_data->nSkeletons; i++)
	{
		sSkeletonData skData = m_data->Skeletons[i];
		//Console::log("Skeleton ID= " + std::to_string(skData.skeletonID) + ", Bone count= " + std::to_string(skData.nRigidBodies));




		bool createNewSkeleton = true;

		for (auto itPoolSkeletons = m_skeletonPool.begin(); itPoolSkeletons != m_skeletonPool.end(); itPoolSkeletons++)
		{
			if (skData.skeletonID == itPoolSkeletons->first)
			{

				// update all joints of existing skeleon with new data
				(m_skeletonPool)[skData.skeletonID]->m_joints = parseSkeleton(skData, skData.skeletonID)->m_joints;


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

			//m_tracker->setSkeletonPoolChanged(true);



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
		sRigidBodyData rbData = skeleton.RigidBodyData[j];
		//printf("Bone %d\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
		//	rbData.ID, rbData.x, rbData.y, rbData.z, rbData.qx, rbData.qy, rbData.qz, rbData.qw);

		//Console::log("OTTracker::parseSkeleton(): jointID: " + std::to_string(rbData.ID));
		//Console::log("OTTracker::parseSkeleton(): joint position: " + std::to_string(rbData.x) + ", " + std::to_string(rbData.y) + ", " + std::to_string(rbData.z));
		//Console::log("OTTracker::parseSkeleton(): joint rotation: " + std::to_string(rbData.qx) + ", " + std::to_string(rbData.qx) + ", " + std::to_string(rbData.qz) + ", " + std::to_string(rbData.qw));


		//insert data now in skeleton pool


				// convert from k4a Vectors and quaternions into custom vectors
		Vector3 pos = Vector3(rbData.x, rbData.y, rbData.z);
		Vector4 rot = Vector4(rbData.qx, rbData.qy, rbData.qz, rbData.qw);


		Joint::JointConfidence confidence = Joint::JointConfidence::HIGH;

		switch (j)
		{

		case 0:
			currSkeleton->m_joints.insert({ Joint::HIPS, Joint(pos, rot, confidence) });

			Console::log("OTTracker::parseSkeleton(): joint position: " + std::to_string(rbData.x) + ", " + std::to_string(rbData.y) + ", " + std::to_string(rbData.z));

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
			currSkeleton->m_joints.insert({ Joint::SHOULDER_R, Joint(pos, rot, confidence) });
			break;

		case 6:
			currSkeleton->m_joints.insert({ Joint::FOREARM_R, Joint(pos, rot, confidence) });
			break;

		case 7:
			currSkeleton->m_joints.insert({ Joint::HAND_R, Joint(pos, rot, confidence) });
			break;

		case 11:
			currSkeleton->m_joints.insert({ Joint::FOREARM_L, Joint(pos, rot, confidence) });
			break;

		case 12:
			currSkeleton->m_joints.insert({ Joint::HAND_L, Joint(pos, rot, confidence) });
			break;

		case 13:
			currSkeleton->m_joints.insert({ Joint::UPLEG_R, Joint(pos, rot, confidence) });
			break;

		case 14:
			currSkeleton->m_joints.insert({ Joint::LEG_R, Joint(pos, rot, confidence) });
			break;

		case 18:
			currSkeleton->m_joints.insert({ Joint::FOOT_L, Joint(pos, rot, confidence) });
			break;

		case 19:
			currSkeleton->m_joints.insert({ Joint::TOE_R, Joint(pos, rot, confidence) });
			break;

		case 20:
			currSkeleton->m_joints.insert({ Joint::TOE_L, Joint(pos, rot, confidence) });
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
	currSkeleton->setHeight(currSkeleton->m_joints[Joint::HEAD].getJointPosition().m_xyz.y);



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
		for (int itOTSkeletons = 0; itOTSkeletons < m_data->nSkeletons; itOTSkeletons++)
		{

			// if OT skeleton is in pool set isOTSkeletonInPool to true
			if (idCurrPoolSkeleton == itOTSkeletons)
			{
				isOTSkeletonInPool = true;
			}
		}

		if (!isOTSkeletonInPool)
		{
			idSkeletonsToErase.push_back(idCurrPoolSkeleton);
		}
	}



	for (int itIndexIdSkeletonsToErase = idSkeletonsToErase.front(); itIndexIdSkeletonsToErase != idSkeletonsToErase.back(); itIndexIdSkeletonsToErase++)
	{

		// erase skeleton with id
		m_skeletonPool.erase(itIndexIdSkeletonsToErase);

		//skeleton was added/removed, so UI updates
		//m_tracker->setSkeletonPoolChanged(true);


		Console::log("OTTracker::cleanSkeletonList(): Removed skeleton with id = " + std::to_string(itIndexIdSkeletonsToErase) + " from pool!");

		Console::log("OTTracker::cleanSkeletonList(): Skeleton pool count = " + std::to_string(m_skeletonPool.size()) + ".");
	}
}