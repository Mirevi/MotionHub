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
	OTTracker(int id, NetworkManager* networkManager, ConfigManager* configManager);
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
	 * sets m_tracking to false, so track() runs for the last time and exits,
	 * after that, the tracking thread is stopped
	 */
	void stop() override;
	/*!
	 * resets the Trackers init data
	 */
	void destroy() override;

	std::string getTrackerType() override;


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
	/*!
	 * the NatNet client
	 * 
	 */
	NatNetClient* m_client;

	int analogSamplesPerMocapFrame = 0;
	/*!
	 * connection type
	 * 
	 */
	int iConnectionType = ConnectionType_Multicast;




	/*!
	 * creates a NatNet Receiver client
	 * mainly code from the NatNet SDK
	 * 
	 * \param iConnectionType connection type
	 * \return Error Code
	 */
	int createClient(int iConnectionType);

	/*!
	 * empty override method for Tracker::init()
	 * this tracker does initialization in  createClient()
	 */
	void init() override;
	/*!
	 * updade method used for tracker thread
	 */
	void update() override;
	/*!
	 * main tracking method
	 * captures one frame of body tracking data and saves all data in the skeleton pool
	 */
	void track() override;

	/*!
	 * gets skeleton data from frame and inserts that data into the skeleton pool
	 * 
	 */
	void extractSkeleton();

	/*!
	 * converts OptiTrack skeleton to default skeleton type
	 * 
	 * \param skeleton input OptiTrack skeleto
	 * \param id the skeletons ID
	 * \return converted default skeleton
	 */
	Skeleton* parseSkeleton(sSkeletonData skeleton, int id, Skeleton* oldSkeletonData);

	/*!
	 * removes skeletons from pool which are not captured in current frames anymore
	 * 
	 */
	void cleanSkeletonPool();


	/*!
	 * rotates the input quaternion at the y-axis with 180 degree
	 * 
	 * \param value input quaternion
	 * \return rotated output quaternion
	 */
	Quaternionf convertOptiTrackRotation(Quaternionf value);

};

