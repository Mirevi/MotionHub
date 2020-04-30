#include "NetworkManager.h"

// default constructor
NetworkManager::NetworkManager()
{
	
	Console::log("NetworkManager::NetworkManager(): Created network manager.");


	// create new osc sender and add to sender pool
	m_poolSender.push_back(new OSCSender(LOCALHOST, DEFAULT_PORT));
	m_poolSender.push_back(new OSCSender(LOCALHOST, DEFAULT_PORT));
	m_poolSender.push_back(new OSCSender(LOCALHOST, DEFAULT_PORT));
	m_poolSender.push_back(new OSCSender(LOCALHOST, DEFAULT_PORT));



}

// send skeleton pool with all active sender
void NetworkManager::sendSkeletonPool(std::map<int, Skeleton>* skeletonPool, int trackerID)
{

	// check if skeleton pool exists
	if (skeletonPool != nullptr)
	{

		// skeleton loop
		for (auto itSkeletonPool = skeletonPool->begin(); itSkeletonPool != skeletonPool->end(); itSkeletonPool++)
		{
		
			// check if current sender is active
			if (m_poolSender[trackerID]->isActive())
			{

				// send skeleton pool
				m_poolSender[trackerID]->sendSkeleton(&(itSkeletonPool->second), DEFAULT_URI, trackerID);

			}
		}
	}
}

