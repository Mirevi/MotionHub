#include "NetworkManager.h"

// default constructor
NetworkManager::NetworkManager(ConfigManager* configManager)
{
	
	m_configManager = configManager;

	//m_ipAddress = m_configManager->getStringFromStartupConfig("ipAddress");
	m_configManager->readString("ipAddress", m_ipAddress);

	Console::log("NetworkManager::NetworkManager(): Created network manager");

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



void NetworkManager::createOSCSender(int ID)
{

	m_poolSender.insert({ID, new OSCSender(m_ipAddress, DEFAULT_PORT) });

}

void NetworkManager::removeNetworkSender(int ID)
{

	m_poolSender.erase(ID);

	Console::log("NetworkManager::removeNetworkSender(): erased Sender " + toString(ID) + ". remaining Sender: " + toString((int)m_poolSender.size()));

}

bool NetworkManager::isValidIPAddress(std::string ipAddress)
{
	// Check if IP Address is valid IPv4 Address
	std::regex ipV4RegEx = std::regex(IPV4_REGEX);
	if (std::regex_match(ipAddress, ipV4RegEx)) {
		return true;
	}

	// Check if IP Address is valid IPv6 Address
	std::regex ipV6RegEx = std::regex(IPV6_REGEX);
	return std::regex_match(ipAddress, ipV6RegEx);
}
