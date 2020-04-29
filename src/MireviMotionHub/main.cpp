#include "MotionHub.h"


NetworkManager* networkManager;
TrackerManager* trackerManager;

void sendSkeletonDelegate(std::map<int, Skeleton>* skeletonPool, int trackerID)
{

	//send skeleton pool to Network Manager
	networkManager->sendSkeletonPool(skeletonPool, trackerID);


}


int main(int argc, char** argv)
{
	
	// print header with information in console
	Console::printHeader();

	//create manager to set funcPtr 
	networkManager = new NetworkManager;
	trackerManager = new TrackerManager;

	//pass funcPtr to tracker Manager, so sendSkeletonPool() can be called from tracker threads
	trackerManager->setSendSkeletonPtr(sendSkeletonDelegate);

	//create the MotionHub object 
	MotionHub* motionHub = new MotionHub(argc, argv, trackerManager, networkManager);

	return 0;

}