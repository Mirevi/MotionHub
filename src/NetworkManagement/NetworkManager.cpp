#include "NetworkManager.h"

NetworkManager::NetworkManager()
{

	poolSender.push_back(new OSCSender(BROADCAST, DEFAULT_PORT));

}

void NetworkManager::sendSkeletonPool(std::map<int, Skeleton*>* skeletonPool)
{
	for (auto itSkeletonPool = skeletonPool->begin(); itSkeletonPool != skeletonPool->end(); itSkeletonPool++)
	{

		for (auto itSenderPool = poolSender.begin(); itSenderPool != poolSender.end(); itSenderPool++)
		{

			if ((*itSenderPool)->isActive())
			{
				(*itSenderPool)->sendSkeleton(itSkeletonPool->second, DEFAULT_URI);
			}
		}
	}
}