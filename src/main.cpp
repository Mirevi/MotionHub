#include "MotionHub.h"

int main(int argc, char** argv)
{

	MotionHub* motionHub = new MotionHub();

	// register tracker
	motionHub->getTrackerManager()->createTracker(TrackerManager::AKT);

	motionHub->getNetworkmanager();

	// start tracking
	motionHub->getTrackerManager()->start();

	return 0;
}