#pragma once

#include "Skeleton.h"
#include "NetworkManager.h" 

class NetworkSender
{

public:

	NetworkSender();

	const char* m_address;
	int m_port;

	virtual void sendSkeleton(Skeleton* skeleton, const char* uri);

};