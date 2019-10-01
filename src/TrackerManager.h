#pragma once

#include <list>
#include "Tracker.h"

class TrackerManager
{

public:
	enum TypeTracker
	{
		AKTracker,
		XSTracker
	};

private:
	std::list<Tracker> tracker;

	Tracker CreateTracker(TypeTracker type);

};