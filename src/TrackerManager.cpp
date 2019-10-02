#include "TrackerManager.h"

TrackerManager::TrackerManager()
{

}

void TrackerManager::CreateTracker(TypeTracker type)
{

	switch (type)
	{

		case AKT:
			poolTracker.insert({ "NULL", new AKTracker() });
			break;

		case XST:
			//poolTracker.insert({ "NULL", new XSTracker() });
			break;

		default:
			break;

	}
}