#pragma once


#include "Skeleton.h"

#include "OSCSender.h"

#include <string>
#include <iostream>
#include <list>

//change if you add more Sender
class NetworkManager
{

public:
	NetworkManager();	

	void sendSkeletonPool(std::map<int, Skeleton>* skeletonPool);

private:
	std::list<NetworkSender*> poolSender;
	
};