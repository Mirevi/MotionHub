#include "NetworkManager.h"

// default constructor
NetworkManager::NetworkManager(ConfigManager* configManager)
{
	
	m_configManager = configManager;

	m_ipAddress = m_configManager->getStringFromStartupConfig("ipAddress");

	Console::log("NetworkManager::NetworkManager(): Created network manager.");

}

// send skeleton pool with all active sender
void NetworkManager::sendSkeletonPool(std::map<int, Skeleton>* skeletonPool, int trackerId)
{

	// check if skeleton pool exists
	if (skeletonPool != nullptr)
	{

		// skeleton loop
		for (auto itSkeletonPool = skeletonPool->begin(); itSkeletonPool != skeletonPool->end(); itSkeletonPool++)
		{
		
			// check if current sender is active
			if (m_poolSender[trackerId]->isActive())
			{

				// send skeleton pool
				m_poolSender[trackerId]->sendSkeleton(&(itSkeletonPool->second), DEFAULT_SKELETON_URI, trackerId);

			}
		}
	}
}

void NetworkManager::sendImageLandmarks(std::vector<Landmark>* imageLandmarks, int senderId)
{
	if (imageLandmarks != nullptr && m_poolSender[senderId]->isActive()) {
		
		m_poolSender[senderId]->sendImageLandmarks(imageLandmarks, DEFAULT_LANDMARK_URI);

	}
}

void NetworkManager::createOSCSender(int ID)
{

	m_poolSender.insert({ID, new OSCSender(m_ipAddress, DEFAULT_PORT) });

}

void NetworkManager::removeNetworkSender(int ID)
{

	m_poolSender.erase(ID);

	Console::log("NetworkManager::removeNetworkSender(): erased Sender " + toString(ID) + ". remaining Sender: " + toString((int)m_poolSender.size()));

}