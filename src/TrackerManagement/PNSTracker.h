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
 * \brief Manages Perception Neuron Studio Tracking
 *
 * \author Eric Jansen
 */
class PNSTracker : public Tracker
{

public:
	
	/*!
	 * constructor with config
	 * \param id
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
	 * stops and closes udpSocket
	 */
	void destroy() override;

private:
	
	SOCKET udpSocket;

	/*!
	 * initializes the udpSocket, must only be called once in the beginning
	 * stop() resets all initialization
	 */
	void init() override;
	/*!
	 * tracking loop 
	 */
	void update() override;
	/*!
	 * starts one tracking cylce by getting the udp data package from socket
	 * calls updateSkeletons(...)
	 */
	void track() override;
	/*!
	 * pushes new skeleton into the pool or updates existing one
	 */
	void extractSkeleton();
	/*!
	 * gets the current skeleton data the udp data package
	 * convertes data to default skeleton
	 */
	void parseSkeleton();
	/*!
	 * deletes all old skeletons from the skeleton pool
	 */
	void cleanSkeletonPool();

};