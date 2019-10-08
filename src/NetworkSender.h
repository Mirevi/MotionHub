#pragma once

#include "Skeleton.h"
#include "defines.h"

class NetworkSender
{

public:

	NetworkSender();

	const char* m_address;
	int m_port;
	bool active = true;

	virtual void sendSkeleton(Skeleton* skeleton, const char* uri);

};