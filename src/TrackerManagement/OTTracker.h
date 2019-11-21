#pragma once
#include "Tracker.h"
#include "MotionHubUtil/Console.h"

#include "NatNetTypes.h"
#include "NatNetClient.h"

#include <atomic>
#include <iostream>
#include <map>
#include <chrono>
#include <algorithm> 


/*!
 * receives NatNet error mesages
 * 
 */
void __cdecl MessageHandler(int msgType, char* msg);


//Forward Declaration
class DataHandlerManager;



/*!
 * \class OTTracker
 *
 * \brief Manages OptiTrack Body Tracking via NatNet protocol
 *
 * \author Kester Evers and Eric Jansen
 */
class OTTracker : public Tracker
{
public:
	/*!
	 * default constructor
	 * 
	 */
	OTTracker();
	/*!
	 * constructor wit initialization
	 * 
	 * \param id Tracker ID
	 */
	OTTracker(int id);
	/*!
	 * default destructor
	 * 
	 */
	~OTTracker();
	/*!
	 * calls the start() method of the base class which sets m_tracking to true
	 */
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

	/*!
	 * reference to frame data
	 * 
	 */
	sFrameOfMocapData* m_refData;

	/*!
	 * the DataHandlerManager object for DataHandler callback
	 * 
	 */
	DataHandlerManager* m_dataHandlerManager;

	/*!
	 * this clients IP address
	 * 
	 */
	char szMyIPAddress[128] = "127.0.0.1";
	/*!
	 * the NatNet servers IP address
	 * 
	 */
	char szServerIPAddress[128] = "127.0.0.1";
	double holdInRoITime = 500;
	/*!
	 * the NatNet client
	 * 
	 */
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

