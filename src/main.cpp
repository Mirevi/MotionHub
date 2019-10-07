#include "MotionHub.h"

#include "Vector3.h"

//#include <osgDB/ReadFile>
//#include <osgViewer/Viewer>
//#include <osgGA/TrackballManipulator>
//#include <osg/PositionAttitudeTransform>
//#include <osg/PolygonMode>
//#include "KeyboardEventHandler.h"

int main(int argc, char** argv)
{

	MotionHub* motionHub = new MotionHub();

	// register tracker
	motionHub->getTrackerManager()->createTracker(TrackerManager::AKT);
	
	//Idea by PL	TODO:
	//TODO: SingleAzureKinect -> Besser Enum namen + createTracker gibt Pointer auf AKTracker zurück
	//AKTracker* aktracker = motionHub->getTrackerManager()->createTracker(TrackerManager::SingleAzureKinect); //Schön! 
	//if(akTracker == nullprt)
	//{
	//	Error!
	//}
	//AKTracker* aktracker = motionHub.m_trackerManager.createTracker(TrackerManager::AKT);
	//motionHub.m_trackerManager.registerTracker(aktracker);

	// start tracking
	motionHub->getTrackerManager()->start();

	return 0;
}