#pragma once

#define WIN32_LEAN_AND_MEAN

#include "Tracker.h"
#include "MotionHubUtil/Console.h"

#include <vector>
#include <sstream>
#include <iterator>

/*!
 * \class PNSTracker
 *
 * \brief Manages Azure Kinect Body Tracking
 *
 * \author Kester Evers and Eric Jansen
 */
class PNSTracker : public Tracker
{

public:
	
	/*!
	 * constructor with config
	 * \param idCam the cameras id number
	 */
	PNSTracker(int id);

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

private:

	SOCKET udpSocket;
	
	/*!
	 * initializes the camera, must only be called once in the beginning
	 * stop() resets all initialization
	 * \param configDevice configuration parameters, standart: DISABLE_ALL
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
	 *convertes k4a skeleton to default skeleton
	*\param skeleton the k4a skeleton to convert
	*\param id the skeletons id
	*\return returns the pointer of the default skeleton in the pool
	 */
	void parseSkeleton();
	/*!
	 * deletes all old skeletons from the skeleton pool
	 * \param bodyFrame the k4a frame with all skeleton data
	 */
	void cleanSkeletonPool();

};