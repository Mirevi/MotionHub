#pragma once

#include "ConfigDllExportNetworkManagement.h"

#include "MotionHubUtil/Skeleton.h"
#include "MotionHubUtil/Console.h"
#include "defines.h"

#include "OSCSender.h"

#include <string>
#include <iostream>
#include <vector>

/*!
 * \class NetworkManager
 *
 * \brief Manager for different Network Sender and skeleton sending
 *
 * \note Supported Protocols: OSC
 * 
 * \author Kester Evers and Eric Jansen
 */
class NetworkManagement_DLL_import_export NetworkManager
{

public:
	/*!
	 * default constructor 
	 */
	NetworkManager();
	/*!
	 * sends skeleton pool to all network sender
	 * \param skeletonPool skeleton pool to send
	 */
	void sendSkeletonPool(std::map<int, Skeleton>* skeletonPool, int trackerID);


private:
	/*!
	 *  pool of all network sender
	 */
	std::vector<NetworkSender*> m_poolSender;
	
};