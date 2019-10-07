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

	void start();
	void stop();

	enum TypeTracker 
	{
		AKT,	// Azure Kinect Tracker
		XST		// XSense Tracker
	};;

	void createTracker(TypeTracker type);

private:
	bool m_tracking = false;
	std::map<std::pair<TypeTracker, int>, Tracker*> poolTracker;

	void update();

};