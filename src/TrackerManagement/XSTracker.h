#pragma once

#include "Tracker.h"
#include "MotionHubUtil/Console.h"

#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "xs_streaming_protocol/xs_udpserver.h"

#include <atomic>
#include <iostream>
#include <map>
#include <chrono>
#include <algorithm>
#include "functional"



/*!
 * receives NatNet error mesages
 *
 */
void __cdecl MessageHandler(int msgType, char* msg);


//Forward Declaration
class DataHandlerManager;


 /*!
  * \class XSTracker
  *
  * \brief Manages Xsens Body Tracking
  *
  */

class XSTracker : public Tracker
{

public:

	/*!
	 * constructor with config
	 * \param idCam the cameras id number
	 */
	XSTracker(int id, NetworkManager* networkManager, ConfigManager* configManager);

	/*!
 * default destructor
 *
 */
	~XSTracker();
	/*!
	 * calls the start() method of the base class which sets m_tracking to true
	 */
	void start() override;
	/*!
	* executes the stop() method of the base class which sets m_tracking to false
	*/
	void stop() override;
	/*!
	 * stops and closes tracker and camera
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

	UdpServer* m_UdpServer;

	int analogSamplesPerMocapFrame = 0;
	/*!
	 * connection type
	 *
	 */
	int iConnectionType = ConnectionType_Multicast;

	/*!
	 * creates xsens client
	 * \param iConnectionType connection type
	 * \return Error Code
	 */
	int createClient(int iConnectionType);


	/*!
	xsens datagram
	*/
	std::vector<QuaternionDatagram::Kinematics> m_data;

	/*!
	 * empty override method for Tracker::init()
	 * this tracker does initialization in  createClient()
	 */
	void init() override;
	/*!
	 * tracking loop
	 */
	void update() override;
	/*!
	 * starts tracking by getting the capture result and body frame
	 * calls updateSkeletons(...)
	 */
	void track() override;
	/*!
	 * gets the current skeleton data from the current body frame
	 * pushes new skeleton into the pool or updates existing one
	 * \param body_frame
	 */
	void extractSkeleton();

	/*!
	 * converts Xsens skeleton to default skeleton type
	 *
	 * \param skeleton input Xsens skeleto
	 * \param id the skeletons ID
	 * \return converted default skeleton
	 */
	Skeleton* parseSkeleton(sSkeletonData skeleton, int id, Skeleton* oldSkeletonData);
	/*!
	 * deletes all old skeletons from the skeleton pool
	 * \param bodyFrame the k4a frame with all skeleton data
	 */
	void cleanSkeletonPool();


	/*!
	test update2
	*/
	void update2(const std::vector<QuaternionDatagram::Kinematics>& m_data);

	/*
	prints Datagram
	*/
	void XSTracker::printDatagram(const std::vector<QuaternionDatagram::Kinematics>& data);

	/*!
	 * rotates the input quaternion at the y-axis with 180 degree
	 *
	 * \param value input quaternion
	 * \return rotated output quaternion
	 */
	Quaternionf convertXsensRotation(Quaternionf value);

};