#pragma once

#include <list>
#include "Tracker.h"
#include "NetworkManager.h"

#include <map>
#include <vector>

#include "AKTracker.h"

class TrackerManager
{

public:
	TrackerManager();
	TrackerManager(NetworkManager* networkmanager);

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

	NetworkManager* m_networkManager;

	void update();

};