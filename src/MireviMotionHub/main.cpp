#include "MotionHub.h"

int main(int argc, char** argv)
{

	MotionHub* motionHub = new MotionHub();

	// create tracker
	motionHub->getTrackerManager()->createTracker(TrackerManager::AKT);

	// start tracking
	motionHub->startTracking();

	return 0;

}