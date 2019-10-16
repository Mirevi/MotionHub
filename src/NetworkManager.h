#pragma once


#include "Skeleton.h"

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
class NetworkManager
{

public:
	NetworkManager();
	void sendSkeletonPool(std::map<int, Skeleton>* skeletonPool);

private:
	std::list<NetworkSender*> poolSender;
	
};