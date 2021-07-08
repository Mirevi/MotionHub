#pragma once

#include "Tracker.h"
#include "xs_udpserver.h"
#include <xstypes/xsquaternion.h>
#include <xstypes/xseuler.h>
#include <map>
#include <math.h>
#include "bvh11.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <fenv.h> 
#include "xs_scaledatagram.h"
#include <windows.h>
#include <mutex>
#include <chrono>
#include <thread>
#include "XSBvhWriter.h"





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

	std::string getTrackerType() override;



private:



	/*!
	 * the UDP Server
	 *
	 */
	UdpServer* m_UdpServer;

	/*!
	xsens datagram
	*/
	std::vector<QuaternionDatagram::Kinematics>* m_kinematics;

	/*!
	xsens datagram with Avatar ID
	*/
	ParserManager::QuaternionDataWithId* m_quaternianDataWithId;

	/*!
	list of current Avatars
	*/
	std::map<int, int> m_avatarList;

	/*!
	skeleton clean after x frames without new data
	*/
	int m_cleanSkeletonCountDown;


	/*!
	 * tracking loop
	 */
	 //void update() override;
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
	 * \param quaternionDataWithId input Xsens skeleton
	 * \param id the skeletons ID
	 * \return converted default skeleton
	 */
	Skeleton* XSTracker::parseSkeleton(ParserManager::QuaternionDataWithId* quaternianDataWithId, int id, Skeleton* oldSkeletonData);
	/*!
	 * deletes all old skeletons from the skeleton pool
	 */
	void cleanSkeletonPool();


	/*!
	* rotate Axis
	*/

	Quaternionf XSTracker::rotateAxis(const double& a, const double& xx, const double& yy, const double& zz);


	/*
	* One BVH stream for each skeleton
	*/

	std::vector<XSBvhWriter*>* bvhWriters;


	/*
	* One BVH stream for each skeleton
	*/

	bool isRecordBvh = false;

	


};