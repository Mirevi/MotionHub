#include "Tracker.h"

Tracker::Tracker(TypeTracker type)
{

	tracking = false;

}

void Tracker::start()
{
	tracking = true;
	track();
}

void Tracker::stop()
{
	tracking = false;
}