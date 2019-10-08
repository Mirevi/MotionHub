#pragma once


#include "Skeleton.h"

#include "OSCSender.h"

#include <string>
#include <iostream>

//change if you add more Sender
#define NETWORK_SENDER_COUNT 1

class NetworkManager
{

public:

	NetworkManager();	

	void sendSkeletonPool(std::map<int, Skeleton>* skeletonPool);



private:
	//OSCSender* m_oscSender;
	NetworkSender* poolSender[NETWORK_SENDER_COUNT];
	
};