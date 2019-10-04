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

//# tracker_azureKinect_01
//# state in tracker

//insted of string for map key: use std::pair<type, int> with enum type

private:
	std::map<std::string, Tracker*> poolTracker;
	void update();
};