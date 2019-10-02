#include "Tracker.h"

Tracker::Tracker()
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

void Tracker::track()
{



}