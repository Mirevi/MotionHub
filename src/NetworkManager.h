#pragma once

#include "Skeleton.h"

#include <string>
#include <iostream>

class NetworkManager
{

public:
	NetworkManager();	
	void sendSkeleton(Skeleton* skeleton, std::string uri);

};