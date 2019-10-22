#pragma once

#include  "ConfigDllExportGestureManagement.h"

#include "MotionHubUtil/Skeleton.h"
#include "MotionHubUtil/Joint.h"
#include "MotionHubUtil/Console.h"
#include <map>

class GestureManagement_DLL_import_export GestureManager
{

public:
	GestureManager();
	void updateAllSkeletonPostures(std::map<int, Skeleton*>* poolSkeletons);

};