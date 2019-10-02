#include "MotionHub.h"

//#include <osgDB/ReadFile>
//#include <osgViewer/Viewer>
//#include <osgGA/TrackballManipulator>
//#include <osg/PositionAttitudeTransform>
//#include <osg/PolygonMode>
//#include "KeyboardEventHandler.h"

int main(int argc, char** argv)
{

	MotionHub motionHub = MotionHub();

	// register tracker
	motionHub.m_trackerManager.createTracker(TrackerManager::AKT);

	// start tracking
	motionHub.m_trackerManager.start();

	return 0;

}