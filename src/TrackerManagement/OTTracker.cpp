#include "OTTracker.h"




//################################# NETNATSDK ###################################################################
//###############################################################################################################
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "RigidBodyCollection.h"
//Play Sound
#define NOMINMAX
#include <windows.h>
#include <Mmsystem.h>
#include <mciapi.h>
#include <atomic>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>


//std::max
#include <algorithm> 
RigidBodyCollection::RigidBodyCollection()
	:mNumRigidBodies(0)
{
	;
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
std::atomic<int> g_rigidBodyDroneX;
std::atomic<int> g_rigidBodyDroneY;
std::atomic<int> g_rigidBodyDroneZ;
RigidBodyCollection g_rigidBodies;
//GLfloat g_RoIRadius = 0.1;
//GLfloat target_1_X, target_1_Y, target_1_Z;
//GLfloat target_2_X, target_2_Y, target_2_Z;
//GLfloat target_3_X, target_3_Y, target_3_Z;
//GLfloat target_4_X, target_4_Y, target_4_Z;
//GLfloat target_5_X, target_5_Y, target_5_Z;
double g_timestampStart,
g_timestampTarget1,
g_timestampTarget2,
g_timestampTarget3,
g_timestampTarget4,
g_timestampTarget5,
g_timestampPressButtonOnce;
double g_pressButtonOnceDelay = 300;
std::vector<std::string> g_resultsIDs;
std::vector<double> g_resultsTimes;
std::map<std::string, double> g_resultsMapForOrder;
int g_lastRoIID;
bool g_isCalibrationFinished = true;
bool g_isFirstLoop = true;
bool g_startTracking = false;
bool g_onceInTarget1 = false;
bool g_passedTarget1 = false;
bool g_onceInTarget2 = false;
bool g_passedTarget2 = false;
bool g_onceInTarget3 = false;
bool g_passedTarget3 = false;
bool g_onceInTarget4 = false;
bool g_passedTarget4 = false;
bool g_onceInTarget5 = false;
bool g_passedTarget5 = false;
bool writeFileOnce = false;
std::string g_kuerzel;
void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData);		// receives data from the server
void __cdecl MessageHandler(int msgType, char* msg);		            // receives NatNet error mesages
char szMyIPAddress[128] = "127.0.0.1";
char szServerIPAddress[128] = "127.0.0.1";
double holdInRoITime = 500;
NatNetClient* theClient;
int analogSamplesPerMocapFrame = 0;
int iConnectionType = ConnectionType_Multicast;



double getCurrentTimeInMillisec()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
//int CreateClient(int iConnectionType)
//{
//	// release previous server
//	if (theClient)
//	{
//		theClient->Uninitialize();
//		delete theClient;
//	}
//	// create NatNet client
//	theClient = new NatNetClient(iConnectionType);
//	// set the callback handlers
//	theClient->SetVerbosityLevel(Verbosity_Warning);
//	theClient->SetMessageCallback(MessageHandler);
//	theClient->SetDataCallback(DataHandler, theClient);	// this function will receive data from the server
//	// [optional] use old multicast group
//	//theClient->SetMulticastAddress("224.0.0.1");
//	// print version info
//	unsigned char ver[4];
//	theClient->NatNetVersion(ver);
//	printf("NatNet Sample Client (NatNet ver. %d.%d.%d.%d)\n", ver[0], ver[1], ver[2], ver[3]);
//	// Init Client and connect to NatNet server
//	// to use NatNet default port assignments
//	int retCode = theClient->Initialize(szMyIPAddress, szServerIPAddress);
//	// to use a different port for commands and/or data:
//	//int retCode = theClient->Initialize(szMyIPAddress, szServerIPAddress, MyServersCommandPort, MyServersDataPort);
//	if (retCode != ErrorCode_OK)
//	{
//		printf("Unable to connect to server.  Error code: %d. Exiting", retCode);
//		return ErrorCode_Internal;
//	}
//	else
//	{
//		// get # of analog samples per mocap frame of data
//		void* pResult;
//		int ret = 0;
//		int nBytes = 0;
//		ret = theClient->SendMessageAndWait("AnalogSamplesPerMocapFrame", &pResult, &nBytes);
//		if (ret == ErrorCode_OK)
//		{
//			analogSamplesPerMocapFrame = *((int*)pResult);
//			printf("Analog Samples Per Mocap Frame : %d", analogSamplesPerMocapFrame);
//		}
//		// print server info
//		sServerDescription ServerDescription;
//		memset(&ServerDescription, 0, sizeof(ServerDescription));
//		theClient->GetServerDescription(&ServerDescription);
//		if (!ServerDescription.HostPresent)
//		{
//			printf("Unable to connect to server. Host not present. Exiting.");
//			return 1;
//		}
//		printf("[SampleClient] Server application info:\n");
//		printf("Application: %s (ver. %d.%d.%d.%d)\n", ServerDescription.szHostApp, ServerDescription.HostAppVersion[0],
//			ServerDescription.HostAppVersion[1], ServerDescription.HostAppVersion[2], ServerDescription.HostAppVersion[3]);
//		printf("NatNet Version: %d.%d.%d.%d\n", ServerDescription.NatNetVersion[0], ServerDescription.NatNetVersion[1],
//			ServerDescription.NatNetVersion[2], ServerDescription.NatNetVersion[3]);
//		printf("Client IP:%s\n", szMyIPAddress);
//		printf("Server IP:%s\n", szServerIPAddress);
//		printf("Server Name:%s\n\n", ServerDescription.szHostComputerName);
//	}
//	return ErrorCode_OK;
//}



//void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData)
//{
//	NatNetClient* pClient = (NatNetClient*)pUserData;
//	//if (fp)
//	//	_WriteFrame(fp, data);
//	int i = 0;
//	//printf("FrameID : %d\n", data->iFrame);
//	//printf("Timestamp :  %3.2lf\n", data->fTimestamp);
//	//printf("Latency :  %3.2lf\n", data->fLatency);
//	// FrameOfMocapData params
//	bool bIsRecording = ((data->params & 0x01) != 0);
//	bool bTrackedModelsChanged = ((data->params & 0x02) != 0);
//	if (bIsRecording)
//		printf("RECORDING\n");
//	if (bTrackedModelsChanged)
//		printf("Models Changed.\n");
//	// timecode - for systems with an eSync and SMPTE timecode generator - decode to values
//	int hour, minute, second, frame, subframe;
//	bool bValid = pClient->DecodeTimecode(data->Timecode, data->TimecodeSubframe, &hour, &minute, &second, &frame, &subframe);
//	// decode to friendly string
//	char szTimecode[128] = "";
//	pClient->TimecodeStringify(data->Timecode, data->TimecodeSubframe, szTimecode, 128);
//	//printf("Timecode : %s\n", szTimecode);
//	// Other Markers
//	//printf("Other Markers [Count=%d]\n", data->nOtherMarkers);
//	//for (i = 0; i < data->nOtherMarkers; i++)
//	//{
//	//	printf("Other Marker %d : %3.2f\t%3.2f\t%3.2f\n",
//	//		i,
//	//		data->OtherMarkers[i][0],
//	//		data->OtherMarkers[i][1],
//	//		data->OtherMarkers[i][2]);
//	//}
//	// Rigid Bodies
//	//printf("Rigid Bodies [Count=%d]\n", data->nRigidBodies);
//	for (i = 0; i < data->nRigidBodies; i++)
//	{
//		// params
//		// 0x01 : bool, rigid body was successfully tracked in this frame
//		bool bTrackingValid = data->RigidBodies[i].params & 0x01;
//		//printf("Rigid Body [ID=%d  Error=%3.2f  Valid=%d]\n", data->RigidBodies[i].ID, data->RigidBodies[i].MeanError, bTrackingValid);
//		//printf("\tx\ty\tz\tqx\tqy\tqz\tqw\n");
//		//printf("\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
//		//	data->RigidBodies[i].x,
//		//	data->RigidBodies[i].y,
//		//	data->RigidBodies[i].z,
//		//	data->RigidBodies[i].qx,
//		//	data->RigidBodies[i].qy,
//		//	data->RigidBodies[i].qz,
//		//	data->RigidBodies[i].qw);
//		g_rigidBodyDroneX = data->RigidBodies[i].x;
//		g_rigidBodyDroneY = data->RigidBodies[i].y;
//		g_rigidBodyDroneZ = data->RigidBodies[i].z;
//		int rbcount = min(RigidBodyCollection::MAX_RIGIDBODY_COUNT, data->nRigidBodies);
//		g_rigidBodies.SetRigidBodyData(data->RigidBodies, rbcount);
//		//printf("\tRigid body markers [Count=%d]\n", data->RigidBodies[i].nMarkers);
//		//for (int iMarker = 0; iMarker < data->RigidBodies[i].nMarkers; iMarker++)
//		//{
//		//	printf("\t\t");
//		//	if (data->RigidBodies[i].MarkerIDs)
//		//		printf("MarkerID:%d", data->RigidBodies[i].MarkerIDs[iMarker]);
//		//	if (data->RigidBodies[i].MarkerSizes)
//		//		printf("\tMarkerSize:%3.2f", data->RigidBodies[i].MarkerSizes[iMarker]);
//		//	if (data->RigidBodies[i].Markers)
//		//		printf("\tMarkerPos:%3.2f,%3.2f,%3.2f\n",
//		//			data->RigidBodies[i].Markers[iMarker][0],
//		//			data->RigidBodies[i].Markers[iMarker][1],
//		//			data->RigidBodies[i].Markers[iMarker][2]);
//		//}
//	}
//	// skeletons
//	//printf("Skeletons [Count=%d]\n", data->nSkeletons);
//	//for (i = 0; i < data->nSkeletons; i++)
//	//{
//	//	sSkeletonData skData = data->Skeletons[i];
//	//	printf("Skeleton [ID=%d  Bone count=%d]\n", skData.skeletonID, skData.nRigidBodies);
//	//	for (int j = 0; j < skData.nRigidBodies; j++)
//	//	{
//	//		sRigidBodyData rbData = skData.RigidBodyData[j];
//	//		printf("Bone %d\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n",
//	//			rbData.ID, rbData.x, rbData.y, rbData.z, rbData.qx, rbData.qy, rbData.qz, rbData.qw);
//	//		printf("\tRigid body markers [Count=%d]\n", rbData.nMarkers);
//	//		for (int iMarker = 0; iMarker < rbData.nMarkers; iMarker++)
//	//		{
//	//			printf("\t\t");
//	//			if (rbData.MarkerIDs)
//	//				printf("MarkerID:%d", rbData.MarkerIDs[iMarker]);
//	//			if (rbData.MarkerSizes)
//	//				printf("\tMarkerSize:%3.2f", rbData.MarkerSizes[iMarker]);
//	//			if (rbData.Markers)
//	//				printf("\tMarkerPos:%3.2f,%3.2f,%3.2f\n",
//	//					data->RigidBodies[i].Markers[iMarker][0],
//	//					data->RigidBodies[i].Markers[iMarker][1],
//	//					data->RigidBodies[i].Markers[iMarker][2]);
//	//		}
//	//	}
//	//}
//	//// labeled markers
//	//bool bOccluded;     // marker was not visible (occluded) in this frame
//	//bool bPCSolved;     // reported position provided by point cloud solve
//	//bool bModelSolved;  // reported position provided by model solve
//	//printf("Labeled Markers [Count=%d]\n", data->nLabeledMarkers);
//	//for (i = 0; i < data->nLabeledMarkers; i++)
//	//{
//	//	bOccluded = ((data->LabeledMarkers[i].params & 0x01) != 0);
//	//	bPCSolved = ((data->LabeledMarkers[i].params & 0x02) != 0);
//	//	bModelSolved = ((data->LabeledMarkers[i].params & 0x04) != 0);
//	//	sMarker marker = data->LabeledMarkers[i];
//	//	int modelID, markerID;
//	//	theClient->DecodeID(marker.ID, &modelID, &markerID);
//	//	printf("Labeled Marker [ModelID=%d, MarkerID=%d, Occluded=%d, PCSolved=%d, ModelSolved=%d] [size=%3.2f] [pos=%3.2f,%3.2f,%3.2f]\n",
//	//		modelID, markerID, bOccluded, bPCSolved, bModelSolved, marker.size, marker.x, marker.y, marker.z);
//	//}
//	//// force plates
//	//if (data->nForcePlates == 0)
//	//{
//	//	printf("No Plates\n");
//	//}
//	//printf("Force Plate [Count=%d]\n", data->nForcePlates);
//	//for (int iPlate = 0; iPlate < data->nForcePlates; iPlate++)
//	//{
//	//	printf("Force Plate %d\n", data->ForcePlates[iPlate].ID);
//	//	for (int iChannel = 0; iChannel < data->ForcePlates[iPlate].nChannels; iChannel++)
//	//	{
//	//		printf("\tChannel %d:\t", iChannel);
//	//		if (data->ForcePlates[iPlate].ChannelData[iChannel].nFrames == 0)
//	//		{
//	//			printf("\tEmpty Frame\n");
//	//		}
//	//		else if (data->ForcePlates[iPlate].ChannelData[iChannel].nFrames != analogSamplesPerMocapFrame)
//	//		{
//	//			printf("\tPartial Frame [Expected:%d   Actual:%d]\n", analogSamplesPerMocapFrame, data->ForcePlates[iPlate].ChannelData[iChannel].nFrames);
//	//		}
//	//		for (int iSample = 0; iSample < data->ForcePlates[iPlate].ChannelData[iChannel].nFrames; iSample++)
//	//			printf("%3.2f\t", data->ForcePlates[iPlate].ChannelData[iChannel].Values[iSample]);
//	//		printf("\n");
//	//	}
//	//}
//}
// MessageHandler receives NatNet error/debug messages
void __cdecl MessageHandler(int msgType, char* msg)
{
	printf("\n%s\n", msg);
}


//dummy method
//int trapdate(int argc, char ** argv)
//{
//
//	//################################### NETNATSDK ###################################
//	//################################### NETNATSDK ###################################
//	//################################### NETNATSDK ###################################
//	CreateClient(iConnectionType);
//	std::cout << "In calibration mode" << std::endl;
//	g_timestampPressButtonOnce = getCurrentTimeInMillisec();
//	g_kuerzel = "NONE";
//	//################################### NETNATSDK ###################################
//	//################################### NETNATSDK ###################################
//	//################################### NETNATSDK ###################################
//
//
//
//
//
//
//
//
//	//CTCon Usage of Keys:
//	// System starts in "L" Mode ("Los") -> Tracking is enabled
//	// Key "M" allows calibration of a new tracking area with 5 areas
//	// After Mode "M", it is possible to press "L" for tracking
//	// P prints the positions of the current saved regions of interest
//	//GLfloat x, y, z;
//	//std::tie(x, y, z) = g_rigidBodies.GetCoordinates(0);
//	//std::cout << "###In Main While Tuple: " << x << " " << y << " " << z << std::endl;
//	double curentTime;
//	//Load RoIPosition file
//	BUTTON_STATE keyboardButtonStateKeyO = g_interactionManager->getKeyboardMouseConnection()->getButtonState(osgGA::GUIEventAdapter::KEY_O);
//	if (g_isFirstLoop || keyboardButtonStateKeyO == PUSH)
//	{
//		char cNum[10];
//		std::string line;
//		/*ifstream myfile("CTcon_RoIPositions.txt");
//		if (myfile.is_open())
//		{
//			myfile >> target_1_X;
//			myfile >> target_1_Y;
//			myfile >> target_1_Z;
//			myfile >> target_2_X;
//			myfile >> target_2_Y;
//			myfile >> target_2_Z;
//			myfile >> target_3_X;
//			myfile >> target_3_Y;
//			myfile >> target_3_Z;
//			myfile >> target_4_X;
//			myfile >> target_4_Y;
//			myfile >> target_4_Z;
//			myfile >> target_5_X;
//			myfile >> target_5_Y;
//			myfile >> target_5_Z;
//			myfile.close();
//			std::cout << "Load successfully RoIPosition File" << std::endl;
//		}
//		else cout << "Unable to open file";*/
//	}
//	//if (!g_isCalibrationFinished)
//	//{
//	//	BUTTON_STATE keyboardButtonStateKey1 = g_interactionManager->getKeyboardMouseConnection()->getButtonState(osgGA::GUIEventAdapter::KEY_1);
//	//	if (keyboardButtonStateKey1 == PUSH)
//	//	{
//	//		std::tie(target_1_X, target_1_Y, target_1_Z) = g_rigidBodies.GetCoordinates(0);
//	//		std::cout << "### SAVED target_1: " << target_1_X << " " << target_1_Y << " " << target_1_Z << std::endl;
//	//		g_timestampPressButtonOnce = getCurrentTimeInMillisec();
//	//	}
//	//	BUTTON_STATE keyboardButtonStateKey2 = g_interactionManager->getKeyboardMouseConnection()->getButtonState(osgGA::GUIEventAdapter::KEY_2);
//	//	if (keyboardButtonStateKey2 == PUSH)
//	//	{
//	//		std::tie(target_2_X, target_2_Y, target_2_Z) = g_rigidBodies.GetCoordinates(0);
//	//		std::cout << "### SAVED target_2: " << target_2_X << " " << target_2_Y << " " << target_2_Z << std::endl;
//	//	}
//	//	BUTTON_STATE keyboardButtonStateKey3 = g_interactionManager->getKeyboardMouseConnection()->getButtonState(osgGA::GUIEventAdapter::KEY_3);
//	//	if (keyboardButtonStateKey3 == PUSH)
//	//	{
//	//		std::tie(target_3_X, target_3_Y, target_3_Z) = g_rigidBodies.GetCoordinates(0);
//	//		std::cout << "### SAVED target_3: " << target_3_X << " " << target_3_Y << " " << target_3_Z << std::endl;
//	//	}
//	//	BUTTON_STATE keyboardButtonStateKey4 = g_interactionManager->getKeyboardMouseConnection()->getButtonState(osgGA::GUIEventAdapter::KEY_4);
//	//	if (keyboardButtonStateKey4 == PUSH)
//	//	{
//	//		std::tie(target_4_X, target_4_Y, target_4_Z) = g_rigidBodies.GetCoordinates(0);
//	//		std::cout << "### SAVED target_4: " << target_4_X << " " << target_4_Y << " " << target_4_Z << std::endl;
//	//	}
//	//	BUTTON_STATE keyboardButtonStateKey5 = g_interactionManager->getKeyboardMouseConnection()->getButtonState(osgGA::GUIEventAdapter::KEY_5);
//	//	if (keyboardButtonStateKey5 == PUSH)
//	//	{
//	//		std::tie(target_5_X, target_5_Y, target_5_Z) = g_rigidBodies.GetCoordinates(0);
//	//		std::cout << "### SAVED target_5: " << target_5_X << " " << target_5_Y << " " << target_5_Z << std::endl;
//	//	}
//	//	BUTTON_STATE keyboardButtonStateKeyL = g_interactionManager->getKeyboardMouseConnection()->getButtonState(osgGA::GUIEventAdapter::KEY_L);
//	//	if (keyboardButtonStateKeyL == PUSH)
//	//	{
//	//		g_isCalibrationFinished = true;
//	//		std::ofstream myfile;
//	//		myfile.open("CTcon_RoIPositions.txt");
//	//		myfile << target_1_X << "\n" << target_1_Y << "\n" << target_1_Z << "\n";
//	//		myfile << target_2_X << "\n" << target_2_Y << "\n" << target_2_Z << "\n";
//	//		myfile << target_3_X << "\n" << target_3_Y << "\n" << target_3_Z << "\n";
//	//		myfile << target_4_X << "\n" << target_4_Y << "\n" << target_4_Z << "\n";
//	//		myfile << target_5_X << "\n" << target_5_Y << "\n" << target_5_Z << "\n";
//	//		myfile.close();
//	//		std::cout << "Writing csv finished" << std::endl;
//	//		std::cout << "### Calibration Done - File written - Drone must in RoI 1 - Starting Tracking " << std::endl;
//	//	}
//	//}
//	//else { //isCalibrationFinished
//	//	BUTTON_STATE keyboardButtonStateKeyM = g_interactionManager->getKeyboardMouseConnection()->getButtonState(osgGA::GUIEventAdapter::KEY_M);
//	//	if (keyboardButtonStateKeyM == PUSH)
//	//	{
//	//		g_isCalibrationFinished = false;
//	//		std::cout << "In calibration mode" << std::endl;
//	//	}
//	//	int currentRoIID = doesRoICollide(x, y, z);
//	//	//std::cout << "currentRoIID: " << currentRoIID << std::endl;
//	//	//std::cout << "g_lastRoIID: " << g_lastRoIID << std::endl;
//	//	if (g_lastRoIID == 1 && currentRoIID == -1 && !g_startTracking)
//	//	{
//	//		//Start
//	//		std::cout << "Start" << std::endl;
//	//		g_startTracking = true;
//	//		g_onceInTarget1 = true;
//	//		g_passedTarget1 = true;
//	//		g_timestampStart = getCurrentTimeInMillisec();
//	//	}
//	//	//################ Target2 ######################
//	//	if (g_startTracking && !g_passedTarget2 && g_onceInTarget2 && currentRoIID == 2 && (holdInRoITime < std::difftime(getCurrentTimeInMillisec(), g_timestampTarget2)))
//	//	{
//	//		std::cout << "Target 2 scanned" << std::endl;
//	//		g_resultsIDs.push_back("A");
//	//		g_resultsTimes.push_back(std::difftime(getCurrentTimeInMillisec(), g_timestampStart));
//	//		g_resultsMapForOrder.insert(std::pair<std::string, double>("A", std::difftime(getCurrentTimeInMillisec(), g_timestampStart)));
//	//		g_passedTarget2 = true;
//	//		if (g_resultsIDs.size() < 4)
//	//		{
//	//			PlaySound("CTcon_target_passed.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
//	//		}
//	//		if (g_resultsIDs.size() == 4)
//	//		{
//	//			PlaySound("CTcon_finish.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
//	//		}
//	//	}
//	//	if (g_startTracking && currentRoIID == 2)
//	//	{
//	//		if (!g_onceInTarget2) { //Falls noch nicht drin war, wird der false sein, daher ! fragen
//	//			g_timestampTarget2 = getCurrentTimeInMillisec();
//	//			std::cout << "##### HIT 2 ##################################" << std::endl;
//	//		}
//	//		g_onceInTarget2 = true;
//	//	}
//	//	//################ Target3 ######################
//	//	if (g_startTracking && !g_passedTarget3 && g_onceInTarget3 && currentRoIID == 3 && (holdInRoITime < std::difftime(getCurrentTimeInMillisec(), g_timestampTarget3)))
//	//	{
//	//		std::cout << "Target 3 scanned" << std::endl;
//	//		g_resultsIDs.push_back("B");
//	//		g_resultsTimes.push_back(std::difftime(getCurrentTimeInMillisec(), g_timestampStart));
//	//		g_resultsMapForOrder.insert(std::pair<std::string, double>("B", std::difftime(getCurrentTimeInMillisec(), g_timestampStart)));
//	//		g_passedTarget3 = true;
//	//		if (g_resultsIDs.size() < 4)
//	//		{
//	//			PlaySound("CTcon_target_passed.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
//	//		}
//	//		if (g_resultsIDs.size() == 4)
//	//		{
//	//			PlaySound("CTcon_finish.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
//	//		}
//	//	}
//	//	if (g_startTracking && currentRoIID == 3)
//	//	{
//	//		if (!g_onceInTarget3) { //Falls noch nicht drin war, wird der false sein, daher ! fragen
//	//			g_timestampTarget3 = getCurrentTimeInMillisec();
//	//			std::cout << "##### HIT 3 ##################################" << std::endl;
//	//		}
//	//		g_onceInTarget3 = true;
//	//	}
//	//	//################ Target4 ######################
//	//	if (g_startTracking && !g_passedTarget4 && g_onceInTarget4 && currentRoIID == 4 && (holdInRoITime < std::difftime(getCurrentTimeInMillisec(), g_timestampTarget4)))
//	//	{
//	//		std::cout << "Target 4 scanned" << std::endl;
//	//		g_resultsIDs.push_back("C");
//	//		g_resultsTimes.push_back(std::difftime(getCurrentTimeInMillisec(), g_timestampStart));
//	//		g_resultsMapForOrder.insert(std::pair<std::string, double>("C", std::difftime(getCurrentTimeInMillisec(), g_timestampStart)));
//	//		g_passedTarget4 = true;
//	//		if (g_resultsIDs.size() < 4)
//	//		{
//	//			PlaySound("CTcon_target_passed.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
//	//		}
//	//		if (g_resultsIDs.size() == 4)
//	//		{
//	//			PlaySound("CTcon_finish.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
//	//		}
//	//	}
//	//	if (g_startTracking && currentRoIID == 4)
//	//	{
//	//		if (!g_onceInTarget4) { //Falls noch nicht drin war, wird der false sein, daher ! fragen
//	//			g_timestampTarget4 = getCurrentTimeInMillisec();
//	//			std::cout << "##### HIT 4 ##################################" << std::endl;
//	//		}
//	//		g_onceInTarget4 = true;
//	//	}
//	//	//################ Target5 ######################
//	//	if (g_startTracking && !g_passedTarget5 && g_onceInTarget5 && currentRoIID == 5 && (holdInRoITime < std::difftime(getCurrentTimeInMillisec(), g_timestampTarget5)))
//	//	{
//	//		std::cout << "Target 5 scanned" << std::endl;
//	//		g_resultsIDs.push_back("D");
//	//		g_resultsTimes.push_back(std::difftime(getCurrentTimeInMillisec(), g_timestampStart));
//	//		g_resultsMapForOrder.insert(std::pair<std::string, double>("D", std::difftime(getCurrentTimeInMillisec(), g_timestampStart)));
//	//		g_passedTarget5 = true;
//	//		if (g_resultsIDs.size() < 4)
//	//		{
//	//			PlaySound("CTcon_target_passed.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
//	//		}
//	//		if (g_resultsIDs.size() == 4)
//	//		{
//	//			PlaySound("CTcon_finish.wav", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
//	//		}
//	//	}
//	//	if (g_startTracking && currentRoIID == 5)
//	//	{
//	//		if (!g_onceInTarget5) { //Falls noch nicht drin war, wird der false sein, daher ! fragen
//	//			g_timestampTarget5 = getCurrentTimeInMillisec();
//	//			std::cout << "##### HIT 5 ##################################" << std::endl;
//	//		}
//	//		g_onceInTarget5 = true;
//	//	}
//	//	g_lastRoIID = currentRoIID;
//	//}
//	if (g_resultsIDs.size() == 4 && !writeFileOnce)
//	{
//		std::cout << "Writing data" << std::endl;
//		//Write results csv
//		std::ofstream myfile;
//		myfile.open("CTcon_results.csv", ios::ate | ios::app);
//		//Write data
//			//Write Kuerzel
//		myfile << g_kuerzel << ", ";
//		//Write Time
//		time_t t = time(0);
//		struct tm * now = localtime(&t);
//		std::stringstream finalFileSavePath;
//		myfile << now->tm_year + 1900 << "-"
//			<< now->tm_mon + 1 << "-"
//			<< now->tm_mday << ", "
//			<< now->tm_hour << "-"
//			<< now->tm_min << "-"
//			<< now->tm_sec << ", ";
//		//Write RoIid and time
//		for (int i = 0; i < g_resultsIDs.size(); i++)
//		{
//			if (i != 4)
//			{
//				myfile << g_resultsIDs.at(i) << "; ";
//			}
//			else
//			{
//				myfile << g_resultsIDs.at(i) << ", ";
//			}
//		}
//		myfile << g_resultsMapForOrder.at("A") << ", ";
//		myfile << g_resultsMapForOrder.at("B") << ", ";
//		myfile << g_resultsMapForOrder.at("C") << ", ";
//		myfile << g_resultsMapForOrder.at("D") << ", ";
//		myfile << std::max({ g_resultsTimes.at(0), g_resultsTimes.at(1), g_resultsTimes.at(2), g_resultsTimes.at(3) }) << "\n";
//		//myfile << g_resultsTimes.at(i) << "\n";
//		myfile.close();
//		std::cout << "Writing successfull" << std::endl;
//		writeFileOnce = true;
//		std::cout << "---> Total time was " << std::max({ g_resultsTimes.at(0), g_resultsTimes.at(1), g_resultsTimes.at(2), g_resultsTimes.at(3) }) << std::endl;
//	}
//	BUTTON_STATE keyboardButtonStateKeyU = g_interactionManager->getKeyboardMouseConnection()->getButtonState(osgGA::GUIEventAdapter::KEY_U);
//	if (g_isFirstLoop || keyboardButtonStateKeyU == PUSH)
//	{
//		//std::cout << "U Time" << std::difftime(getCurrentTimeInMillisec(), g_timestampPressButtonOnce) << std::endl;
//			//printf("Kuerzel: ");
//			//fgets(g_kuerzel, 100, stdin);
//			//printf("Ihre Eingabe: %s\n", g_kuerzel);
//			//g_timestampPressButtonOnce = getCurrentTimeInMillisec();
//		ifstream myfile("CTcon_Kuerzel.txt");
//		if (myfile.is_open())
//		{
//			myfile >> g_kuerzel;
//			myfile.close();
//			std::cout << "Load successfully initials-file" << std::endl;
//		}
//	}
//	//Print all RoIs
//	BUTTON_STATE keyboardButtonStateKeyP = g_interactionManager->getKeyboardMouseConnection()->getButtonState(osgGA::GUIEventAdapter::KEY_P);
//	if (keyboardButtonStateKeyP == PUSH)
//	{
//		std::cout << "### target_1: " << target_1_X << " " << target_1_Y << " " << target_1_Z << std::endl;
//		std::cout << "### target_2: " << target_2_X << " " << target_2_Y << " " << target_2_Z << std::endl;
//		std::cout << "### target_3: " << target_3_X << " " << target_3_Y << " " << target_3_Z << std::endl;
//		std::cout << "### target_4: " << target_4_X << " " << target_4_Y << " " << target_4_Z << std::endl;
//		std::cout << "### target_5: " << target_5_X << " " << target_5_Y << " " << target_5_Z << std::endl;
//	}
//	g_isFirstLoop = false; //Need for loading RoIPositionFile once
//	//g_timestampTarget2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//	//g_timestampTarget2,
//	//	g_timestampTarget2Leave,
//	//g_lastRoIID = currentRoIID;
//	//double timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//	//std::cout << "###### m_rsPipe->wait_for_frames()  " << std::fixed << timestamp << std::endl;
//	//HWM:
//	//Zeiten zählen, wenn aus erstem RoI rausgeflogen wird (Reset ist durch Drone nicht möglich! -> Bool)
//	//Die Zeiten zwischen den RoIs und jeweils die ID muss geschrieben werden -> Am besten dazu durch ein scanf oder ähnliches ein Kürzel einlesen
//	//Alle Zeiten und Differenzen + Gesamtzeit speichern
//	//Fluglinien abspeichern
//	//Fluglinien alle samt einlesen und anzeigen können
//}