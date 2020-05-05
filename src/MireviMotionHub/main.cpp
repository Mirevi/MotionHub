#include "MotionHub.h"







int main(int argc, char** argv)
{
	
	// print header with information in console
	Console::printHeader();



	//pass funcPtr to tracker Manager, so sendSkeletonPool() can be called from tracker threads
	//trackerManager->setSendSkeletonPtr(sendSkeletonDelegate);

	//create the MotionHub object 
	MotionHub* motionHub = new MotionHub(argc, argv);

	return 0;

}