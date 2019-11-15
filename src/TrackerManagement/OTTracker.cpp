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

	m_dataHandlerManager = new DataHandlerManager(&m_skeletonPool, m_properties);
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

}

void OTTracker::update()
{

}

void OTTracker::track()
{


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