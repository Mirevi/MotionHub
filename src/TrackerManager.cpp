#include "TrackerManager.h"

TrackerManager::TrackerManager()
{

	createTracker(AKT);
	poolTracker.begin()->second->start();

}

void TrackerManager::createTracker(TypeTracker type)
{

	switch (type)
	{

		case AKT:
			poolTracker.insert({ "NULL", new AKTracker() });
			Console::log('i', "Created tracker = AKTracker.");
			break;

		case XST:
			//poolTracker.insert({ "NULL", new XSTracker() });
			break;

		default:
			break;

	}
}