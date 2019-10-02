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

	MotionHub motionHub = MotionHub();

	// register tracker
	motionHub.m_trackerManager.createTracker(TrackerManager::AKT);

	// start tracking
	//motionHub.m_trackerManager.start();

	Vector3 vector3 = Vector3::one() ;
	Console::log(vector3.toString());

	return 0;

}