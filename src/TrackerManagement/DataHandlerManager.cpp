#include "DataHandlerManager.h"


std::map<int, Skeleton*>* DataHandlerManager::m_refSkeletonPool;
Tracker::Properties* DataHandlerManager::m_properties;

sFrameOfMocapData* DataHandlerManager::m_data;


DataHandlerManager::DataHandlerManager(std::map<int, Skeleton*>* skeletonPool, Tracker::Properties* properties, OTTracker* tracker)
{



	m_refSkeletonPool = skeletonPool;
	m_properties = properties;

	m_tracker = tracker;

}

// receives data from the server
void DataHandlerManager::DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	

	if (!m_properties->isTracking)
	{

		return;
	}


	m_data = data;

	NatNetClient* pClient = (NatNetClient*)pUserData;


	bool bIsRecording = ((m_data->params & 0x01) != 0);
	bool bTrackedModelsChanged = ((m_data->params & 0x02) != 0);
	if (bIsRecording)
		printf("RECORDING\n");
	if (bTrackedModelsChanged)
		printf("Models Changed.\n");
	// timecode - for systems with an eSync and SMPTE timecode generator - decode to values
	int hour, minute, second, frame, subframe;
	bool bValid = pClient->DecodeTimecode(m_data->Timecode, m_data->TimecodeSubframe,
		&hour, &minute, &second, &frame, &subframe);
	// decode to friendly string
	char szTimecode[128] = "";
	pClient->TimecodeStringify(m_data->Timecode, m_data->TimecodeSubframe, szTimecode, 128);


	extractSkeleton();

}



void DataHandlerManager::extractSkeleton()
{

	m_properties->countDetectedSkeleton = m_data->nSkeletons;

	//loop through all skeletons
	for (int i = 0; i < m_data->nSkeletons; i++)
	{
		sSkeletonData skData = m_data->Skeletons[i];
		//Console::log("Skeleton ID= " + std::to_string(skData.skeletonID) + ", Bone count= " + std::to_string(skData.nRigidBodies));




		bool createNewSkeleton = true;

		for (auto itPoolSkeletons = m_refSkeletonPool->begin(); itPoolSkeletons != m_refSkeletonPool->end(); itPoolSkeletons++)
		{
			if (skData.skeletonID == itPoolSkeletons->first)
			{

				// update all joints of existing skeleon with new data
				(*m_refSkeletonPool)[skData.skeletonID]->m_joints = parseSkeleton(skData, skData.skeletonID)->m_joints;

				
				createNewSkeleton = false;

				break;

			}
		}


		// create new skeleton
		if (createNewSkeleton)
		{

			// create new skeleton and add it to the skeleton pool
			m_refSkeletonPool->insert(std::pair<int, Skeleton*>(skData.skeletonID, parseSkeleton(skData, skData.skeletonID)));



			//skeleton was added/removed, so UI updates

			//m_tracker->setSkeletonPoolChanged(true);



			Console::log("DataHandlerManager::extractSkeleton(): Created new skeleton with id = " + std::to_string(skData.skeletonID) + ".");

		}
	} 
}


Skeleton* DataHandlerManager::parseSkeleton(sSkeletonData skeleton, int id)
{


	// skeleton data container
	Skeleton* currSkeleton = new Skeleton(id);



	//loop through all joints
	for (int j = 0; j < skeleton.nRigidBodies; j++)
	{
		sRigidBodyData rbData = skeleton.RigidBodyData[j];
		//printf("Bone %d\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
		//	rbData.ID, rbData.x, rbData.y, rbData.z, rbData.qx, rbData.qy, rbData.qz, rbData.qw);


		//insert data now in skeleton pool


				// convert from k4a Vectors and quaternions into custom vectors
		Vector3 pos = Vector3(rbData.x, rbData.y, rbData.z);
		Vector4 rot = Vector4(rbData.qx, rbData.qy, rbData.qz, rbData.qw);


		Joint::JointConfidence confidence = Joint::JointConfidence::HIGH;

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
	currSkeleton->setHeight(currSkeleton->m_joints[Joint::HEAD].getJointPosition().m_xyz.y);



	return currSkeleton;

}


void DataHandlerManager::cleanSkeletonPool()
{

	//all skeletons with ids in this list will be erased at the end of this method
	std::list<int> idSkeletonsToErase;

	// loop through all skeletons in pool
	for (auto itPoolSkeletons = m_refSkeletonPool->begin(); itPoolSkeletons != m_refSkeletonPool->end(); itPoolSkeletons++)
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
		m_refSkeletonPool->erase(itIndexIdSkeletonsToErase);

		//skeleton was added/removed, so UI updates
		//m_tracker->setSkeletonPoolChanged(true);


		Console::log("OTTracker::cleanSkeletonList(): Removed skeleton with id = " + std::to_string(itIndexIdSkeletonsToErase) + " from pool!");

		Console::log("OTTracker::cleanSkeletonList(): Skeleton pool count = " + std::to_string(m_refSkeletonPool->size()) + ".");
	}
}