#include "TrackerManager.h"

TrackerManager::TrackerManager()
{

}

void TrackerManager::CreateTracker(Tracker::TypeTracker type)
{

	switch (type)
	{
	case Tracker::AKTracker:
		poolTracker.insert({ "NULL", new AKTracker() });
		break;
	case Tracker::XSTracker:
		//poolTracker.insert({ "NULL", new XSTracker() });

		break;
	default:
		break;
	}

}