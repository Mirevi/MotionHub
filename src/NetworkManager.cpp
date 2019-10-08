#include "NetworkManager.h"
#include "defines.h"

NetworkManager::NetworkManager()
{

	//m_oscSender = new OSCSender(BROADCAST, DEFAULT_PORT);
	poolSender[0] = new OSCSender(BROADCAST, DEFAULT_PORT);
}

void NetworkManager::sendSkeletonPool(std::map<int, Skeleton>* skeletonPool)
{

	for (auto itSkeletonPool = skeletonPool->begin(); itSkeletonPool != skeletonPool->end(); itSkeletonPool++)
	{

		try
		{
			for (size_t i = 0; i < NETWORK_SENDER_COUNT; i++)
			{
				poolSender[i]->sendSkeleton(&(itSkeletonPool->second), DEFAULT_URI + itSkeletonPool->first);
			}
		}
		catch (const std::exception&)
		{
			Console::logError("NetworkManager::sendSkeleton(): Skeleton could not be send! Pointer exception.");
		}
	}
}

