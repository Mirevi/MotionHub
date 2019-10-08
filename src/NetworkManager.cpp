#include "NetworkManager.h"

NetworkManager::NetworkManager()
{

	m_oscSender = new OSCSender(LOCALHOST, DEFAULT_PORT);

}

void NetworkManager::sendSkeletonPool(std::map<int, Skeleton>* skeletonPool)
{

	for (auto itSkeletonPool = skeletonPool->begin(); itSkeletonPool != skeletonPool->end(); itSkeletonPool++)
	{

		m_oscSender->sendSkeleton(&(itSkeletonPool->second), DEFAULT_URI + itSkeletonPool->first);

	}
}