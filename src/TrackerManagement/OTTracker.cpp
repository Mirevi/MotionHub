#include "OTTracker.h"
#include "DataHandlerManager.h"

#pragma once

OTTracker::OTTracker()
{

}


OTTracker::OTTracker(int id, NetworkManager* networkManager, ConfigManager* configManager)
{
	//assign id and name to properties
	m_properties->id = id;
	m_properties->name = "tracker_optiTrack_" + std::to_string(id);

	m_networkManager = networkManager;
	m_configManager = configManager;


	//default is enabled
	m_isEnabled = true;


	/*//set default values for offsets
	setPositionOffset(Vector3f(configManager->getFloatFromStartupConfig("xPosOptiTrack"), configManager->getFloatFromStartupConfig("yPosOptiTrack"), configManager->getFloatFromStartupConfig("zPosOptiTrack")));
	setRotationOffset(Vector3f(configManager->getFloatFromStartupConfig("xRotOptiTrack"), configManager->getFloatFromStartupConfig("yRotOptiTrack"), configManager->getFloatFromStartupConfig("zRotOptiTrack")));
	setScaleOffset(Vector3f(configManager->getFloatFromStartupConfig("xSclOptiTrack"), configManager->getFloatFromStartupConfig("ySclOptiTrack"), configManager->getFloatFromStartupConfig("zSclOptiTrack")));*/

	// read property values from config
	readOffsetFromConfig();
}


OTTracker::~OTTracker()
{



}



void OTTracker::start()
{
	//init NatNet client
	createClient(iConnectionType);

	//get reference to the frame data
	m_refData = m_dataHandlerManager->getData();

	// Clear point collection
	m_pointCollection.clear();

	//start tracking in a new thread
	Tracker::start();
}


void OTTracker::stop()
{
	//is not tracking, so the update loop exits 
	Tracker::stop();

	//resets the client
	m_client->Uninitialize();

	//dlete client and dataHandlerManager Objects
	delete m_client;
	delete m_dataHandlerManager;
}


int OTTracker::createClient(int iConnectionType)
{

	// create NatNet client
	m_client = new NatNetClient(iConnectionType);

	// set the callback handlers
	m_client->SetVerbosityLevel(Verbosity_Warning);
	m_client->SetMessageCallback(MessageHandler);

	//create dummy object for MessageHandler
	m_dataHandlerManager = new DataHandlerManager(this);

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
	m_countDetectedSkeleton = m_refData->nSkeletons;

	// lock skeleton pool mutex
	m_skeletonPoolLock.lock();

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

	// unlock skeleton pool mutex
	m_skeletonPoolLock.unlock();


	// lock point collection mutex
	m_pointCollectionLock.lock();

	if (m_refData->nRigidBodies + m_refData->nLabeledMarkers != receivedPointIDs.size()) {
		m_hasPointCollectionChanged = true;
	}
	
	// clear received points
	receivedPointIDs.clear();

	// RigidBodies
	for (int i = 0; i < m_refData->nRigidBodies; i++) {

		sRigidBodyData& rbData = m_refData->RigidBodies[i];

		// negate id for RigidBodies
		int id = -rbData.ID;

		// params 0x01 : bool, rigid body was successfully tracked in this frame
		bool isTrackingValid = rbData.params & 0x01;

		auto rbIterator = m_pointCollection.points.find(id);

		Vector3f position = Vector3f(rbData.x, rbData.y, rbData.z);
		Quaternionf rotation = Quaternionf(rbData.qw, rbData.qx, -rbData.qy, -rbData.qz);

		if (rbIterator != m_pointCollection.points.end()) {

			Point& point = rbIterator->second;

			point.setValid(isTrackingValid);

			point.setPosition(position);
			point.setRotation(rotation);

			point.setCustomInt(rbData.params);
			point.setCustomFloat(rbData.MeanError);
		}
		// Rigidbody not found -> add new Point
		else {
			Point point = Point(position, rotation, Point::Rigidbody);

			point.setValid(isTrackingValid);

			point.setCustomInt(rbData.params);
			point.setCustomFloat(rbData.MeanError);

			m_pointCollection.addPoint(id, point);

			m_hasPointCollectionChanged = true;
		}

		// Add ID to received points
		receivedPointIDs.insert(id);
	}

	// Markers
	for (int i = 0; i < m_refData->nLabeledMarkers; i++) {

		// params 0x01 : bool, marker was not visible (occluded) in this frame
		bool isOccluded = ((m_refData->LabeledMarkers[i].params & 0x01) != 0);

		sMarker& marker = m_refData->LabeledMarkers[i];

		int id = marker.ID;

		auto markerIterator = m_pointCollection.points.find(id);

		Vector3f position = Vector3f(marker.x, marker.y, marker.z);

		// Marker found -> update
		if (markerIterator != m_pointCollection.points.end()) {

			Point& point = markerIterator->second;

			point.setValid(!isOccluded);

			point.setPosition(position);

			point.setCustomFloat(marker.size);
			point.setCustomInt((int)marker.params);
		}
		// Marker not found -> add new
		else {

			Point point = Point(position, eulerToQuaternion(Vector3f(0, 0, 0)), Point::Marker);

			point.setValid (!isOccluded);

			point.setCustomFloat(marker.size);
			point.setCustomInt((int)marker.params);

			m_pointCollection.addPoint(id, point);

			m_hasPointCollectionChanged = true;
		}

		// Add ID to received points
		receivedPointIDs.insert(id);
	}

	if (m_hasPointCollectionChanged) {
		sDataDescriptions* pDataDefs = NULL;

		m_client->GetDataDescriptions(&pDataDefs);

		if (pDataDefs != NULL) {

			for (int i = 0; i < pDataDefs->nDataDescriptions; i++) {
				if (pDataDefs->arrDataDescriptions[i].type == Descriptor_MarkerSet) {

					// MarkerSet
					sMarkerSetDescription* pMS = pDataDefs->arrDataDescriptions[i].Data.MarkerSetDescription;
					
					for (int i = 0; i < pMS->nMarkers; i++)
					 printf(pMS->szMarkerNames[i]);
				}
				else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_RigidBody) {

					// RigidBody
					sRigidBodyDescription* pRB = pDataDefs->arrDataDescriptions[i].Data.RigidBodyDescription;
					//printf("RigidBody Name : %s\n", pRB->szName);
					//printf("RigidBody ID : %d\n", pRB->ID);
					//printf("RigidBody Parent ID : %d\n", pRB->parentID);
					//printf("Parent Offset : %3.2f,%3.2f,%3.2f\n", pRB->offsetx, pRB->offsety, pRB->offsetz);
				}
			}
		}
	}


	// Copy point iterator
	auto pointIterator = m_pointCollection.points.begin();

	// Loop while point iterator
	while (pointIterator != m_pointCollection.points.end()) {

		// Check if ID was not received
		if (receivedPointIDs.find(pointIterator->first) == receivedPointIDs.end()) {

			//m_pointCollection.points[pointIterator]
			if (pointIterator->second.isValid()) {
				m_pointCollection.points[pointIterator->first].setValid(false);
				++pointIterator;
			}
			else {
				m_pointCollection.points.erase(pointIterator++);

				m_hasPointCollectionChanged = true;
			}

		}
		// ID was received -> advance to next element
		else {
			++pointIterator;
		}
	}

	// unlock point collection mutex
	m_pointCollectionLock.unlock();
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

		// convert from optiTrack Vectors and quaternions into custom vectors
		Vector4f pos = applyOffset(Vector4f(rbData.x, rbData.y, rbData.z, 1.0f));
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
		rot = applyOffset(Quaternionf(rot.y(), rot.z(), rot.w(), rot.x()));

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

	// exit if no skeletons found
	if (idSkeletonsToErase.size() == 0) {
		return;
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

std::string OTTracker::getTrackerType()
{

	return "OptiTrack";

}
