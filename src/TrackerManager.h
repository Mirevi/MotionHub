#pragma once

#include <list>
#include "Tracker.h"

#include <map>
#include <vector>


class TrackerManager
{

public:
	void CreateTracker(Tracker::TypeTracker type);
	TrackerManager();

private:
	std::map<std::string, Tracker*> poolTracker;

};