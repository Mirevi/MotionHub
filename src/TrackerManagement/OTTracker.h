#pragma once
#include "Tracker.h"
#include "MotionHubUtil/Console.h"

#define min(a,b)            (((a) < (b)) ? (a) : (b))
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "RigidBodyCollection.h"
//Play Sound
#define NOMINMAX
//#include <windows.h>
//#include <Mmsystem.h>
//#include <mciapi.h>
#include <atomic>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>




//std::max
#include <algorithm> 

//TODO: Move functions into class. Didn't work with OTracker::DataHandler

void __cdecl MessageHandler(int msgType, char* msg);		            // receives NatNet error mesages


//Forward Declaration
class DataHandlerManager;

//class DataHandlerManager
//{
//public:
//	void static DataHandler(sFrameOfMocapData* data, void* pUserData);
//};




class OTTracker : public Tracker
{
public:
	OTTracker();
	OTTracker(int id);
	~OTTracker();

	void start() override;
	/*!
	 *  sets m_tracking to false
	 */
	void stop() override;
	/*!
	 * resets the Trackers init data
	 */
	void destroy() override;

private:

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



	sFrameOfMocapData* m_data;


	DataHandlerManager* m_dataHandlerManager;


	char szMyIPAddress[128] = "127.0.0.1";
	char szServerIPAddress[128] = "127.0.0.1";
	double holdInRoITime = 500;
	NatNetClient* m_client;
	int analogSamplesPerMocapFrame = 0;
	int iConnectionType = ConnectionType_Multicast;

	int createClient(int iConnectionType);
	//void DataHandler(sFrameOfMocapData* data, void* pUserData);

	void init() override {};
	/*!
	 * updade method used for tracker thread
	 */
	void update() override;
	/*!
	 * main tracking method
	 * captures one frame of body tracking data and saves all data in the skeleton pool
	 */
	void track() override;


	void extractSkeleton();
	Skeleton* parseSkeleton(sSkeletonData skeleton, int id);
	void cleanSkeletonPool();

};

