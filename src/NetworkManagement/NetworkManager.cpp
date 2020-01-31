#include "NetworkManager.h"

// default constructor
NetworkManager::NetworkManager()
{
	
	Console::log("NetworkManager::NetworkManager(): Created network manager.");

	// create new osc sender and add to sender pool
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

			// sender loop
			for (auto itSenderPool = m_poolSender.begin(); itSenderPool != m_poolSender.end(); itSenderPool++)
			{

				// check if current sender is active
				if ((*itSenderPool)->isActive())
				{

					// send skeleton pool
					(*itSenderPool)->sendSkeleton(&(itSkeletonPool->second), DEFAULT_URI, trackerID);
				}
			}
		}
	}
}