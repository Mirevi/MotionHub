#include "MotionHub.h"


NetworkManager* m_networkManager;
TrackerManager* m_trackerManager;


void sendSkeletonDelegate(std::map<int, Skeleton>* skeletonPool, int trackerID)
{

	m_networkManager->sendSkeletonPool(skeletonPool, trackerID);

}



int main(int argc, char** argv)
{

	m_networkManager = new NetworkManager();
	m_trackerManager = new TrackerManager();

	m_trackerManager->setDelegate(sendSkeletonDelegate);


	MotionHub* motionHub = new MotionHub(argc, argv, m_networkManager, m_trackerManager);

	return 0;

}