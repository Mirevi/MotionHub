#include "NetworkManager.h"
#include "defines.h"

NetworkManager::NetworkManager()
{
	poolSender.push_back(new OSCSender(BROADCAST, DEFAULT_PORT));
}

void NetworkManager::sendSkeletonPool(std::map<int, Skeleton>* skeletonPool)
{
	for (auto itSkeletonPool = skeletonPool->begin(); itSkeletonPool != skeletonPool->end(); itSkeletonPool++)
	{

		try
		{
			for (size_t i = 0; i < poolSender.size(); i++)
			{
				if (poolSender.at(i)->m_active)
				{
					poolSender.at(i)->sendSkeleton(&(itSkeletonPool->second), DEFAULT_URI.c_str() + itSkeletonPool->first);
				}
			}
		}
		catch (const std::exception&)
		{
			Console::logError("NetworkManager::sendSkeleton(): Skeleton could not be send! Pointer exception.");
		}
	}
}

