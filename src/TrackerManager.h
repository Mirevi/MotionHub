#pragma once

#include <list>
#include "Tracker.h"

#include <map>
#include <vector>

#include "AKTracker.h"

class TrackerManager
{

public:

	TrackerManager();

	enum TypeTracker
	{
		AKT,
		XST
	};

	void createTracker(TypeTracker type);

private:
	std::map<std::string, Tracker*> poolTracker;

};