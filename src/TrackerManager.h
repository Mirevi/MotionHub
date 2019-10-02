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

	bool m_tracking = false;

	void createTracker(TypeTracker type);
	void start();
	void stop();

private:
	std::map<std::string, Tracker*> poolTracker;
	void update();
};