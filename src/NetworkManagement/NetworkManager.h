#pragma once

#include "ConfigDllExportNetworkManagement.h"

#include "MotionHubUtil/Skeleton.h"
#include "MotionHubUtil/Console.h"
#include "defines.h"

#include "OSCSender.h"

#include <string>
#include <iostream>
#include <list>

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
	NetworkManager();
	void sendSkeletonPool(std::map<int, Skeleton*>* skeletonPool);

private:
	std::list<NetworkSender*> poolSender;
	
};