#pragma once

#include "Skeleton.h"
#include "defines.h"

class NetworkSender
{

private:
	bool m_active = true;

public:
	NetworkSender();
	NetworkSender(std::string address, int port);	
	bool isActive();
	void setActive(bool active);
	virtual void sendSkeleton(Skeleton* skeleton, const char* uri) = 0;

	
protected: //Is only accessable by derived classes
	std::string m_address;
	int m_port;

};