#pragma once

#include "Skeleton.h"
#include "Joint.h"
#include "Console.h"
#include <map>

class GestureManager
{

public:
	GestureManager();
	void updateAllSkeletonPostures(std::map<int, Skeleton>* poolSkeletons);

};