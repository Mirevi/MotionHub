#pragma once

#include <ws2tcpip.h>

#include "Tracker.h"

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
	
	SOCKET inSocket;

	// use to hold the client information (port / ip address)
	sockaddr_in client;
	int clientMemSize;

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
	void extractSkeleton(char rawData[]);
	/*!
	 * gets the current skeleton data the udp data package
	 * convertes data to default skeleton
	 */
	Skeleton parseSkeleton(std::vector<std::string> dataValues, int skeletonId);
	/*!
	 * deletes all old skeletons from the skeleton pool
	 */
	void cleanSkeletonPool();

};