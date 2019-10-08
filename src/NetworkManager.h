#pragma once

#define LOCALHOST "127.0.0.1"
#define DEFAULT_PORT 7000
#define DEFAULT_URI "/mh/skeleton/"

#include "Skeleton.h"

#include "OSCSender.h"

#include <string>
#include <iostream>

class NetworkManager
{

public:

	NetworkManager();	

	void sendSkeletonPool(std::map<int, Skeleton>* skeletonPool);

private:
	OSCSender* m_oscSender;

};