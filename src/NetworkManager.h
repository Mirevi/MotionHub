#pragma once


#include "Skeleton.h"

#include "OSCSender.h"

#include <string>
#include <iostream>
#include <vector>

//change if you add more Sender
class NetworkManager
{

public:
	NetworkManager();	

	void sendSkeletonPool(std::map<int, Skeleton>* skeletonPool);

private:
	std::vector<NetworkSender*> poolSender;
	
};