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


	//set the offset values
	setPositionOffset(Vector3f(0.0f, 0.1f, 0.0f));
	setRotationOffset(Vector3f(0.0f, 0.0f, 0.0f));
	setScaleOffset(Vector3f(1.0f, 1.0f, 1.0f));

	m_idCam = -1;




}

OTTracker::~OTTracker()
{



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

void OTTracker::stop()
{
	//is not tracking, so the update loop exits 
	m_properties->isTracking = false;

	//resets the client
	m_client->Uninitialize();

	//dlete client and dataHandlerManager Objects
	delete m_client;
	delete m_dataHandlerManager;

}

void OTTracker::destroy()
{

	// delete this object
	delete this;

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

	Console::log("OTTracker::createClient(): Created NatNet Client (NatNet ver. " + std::to_string(ver[0]) + "." + std::to_string(ver[1]) + "." + std::to_string(ver[2]) + "." + std::to_string(ver[3]) + ")");

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



void OTTracker::init()
{

	//no init code. The receiver is created on start, because it cannot be deactivated/paused

}

void OTTracker::update()
{
	

	// track while tracking is true
	while (m_properties->isTracking)
	{

		// if no new data is procressed
		if (!m_isDataAvailable)
		{


			// get new data
			track();

			m_sendSkeletonDelegate(&m_skeletonPool, m_properties->id);


		}


	}

	//clean skeleton pool after tracking
	clean();

	
}

void OTTracker::track()
{

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



void OTTracker::extractSkeleton()
{

	//when new data isn't available, skip this method run
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

Skeleton* OTTracker::parseSkeleton(sSkeletonData skeleton, int id, Skeleton* oldSkeletonData)
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
		if ( pos.x() >  100 || pos.y() >  100 || pos.z() >  100 ||
			 pos.x() < -100 || pos.y() < -100 || pos.z() < -100 )
		{

			return nullptr;

		}

		//filter for corrupt rotation values
		if ( rot.x() < -1 || rot.y() < -1 || rot.z() < -1 || rot.w() < -1 || 
			 rot.x() >  1 || rot.y() >  1 || rot.z() >  1 || rot.w() >  1 )
		{

			return nullptr;

		}

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
	currSkeleton->setHeight(currSkeleton->m_joints[Joint::HEAD].getJointPosition().y());

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

	}
}



Quaternionf OTTracker::convertOptiTrackRotation(Quaternionf value)
{

	//converts the euler rotation to quaternion and multiplying it with input
	return eulerToQuaternion(Vector3f(0.0f, 180.0f, 0.0f)) * value;

}



// MessageHandler receives NatNet error/debug messages
void MessageHandler(int msgType, char* msg)
{

	//the NatNet messages are muted. If you need the messages, output msg

}