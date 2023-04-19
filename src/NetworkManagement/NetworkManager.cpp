#include "NetworkManager.h"

// default constructor
NetworkManager::NetworkManager(ConfigManager* configManager)
{
	
	m_configManager = configManager;

	m_ipAddresses = std::vector<std::string>(1);

	//check if adresses in config are real IPs and store them in m_ipAddresses
	std::string adr;
	m_configManager->readString("ipAddress", adr);

	if (isValidIPAddress(adr))
	{
		m_ipAddresses[0] = adr;
	}

	
	if (m_configManager->readString("ipAddress2", adr))
	{
		if (isValidIPAddress(adr))
		{
			m_ipAddresses.push_back(adr);
		}
	}
	else
	{
		Console::log("NetworkManager::NetworkManager(): No value for ipAddress2 in config");
	}

	Console::log("NetworkManager::NetworkManager(): Created network manager");

}

// send skeleton pool with all active sender
void NetworkManager::sendSkeletonPool(std::map<int, Skeleton>* skeletonPool, int trackerID, int frameIndex)
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
				m_poolSender[trackerID]->sendSkeleton(&(itSkeletonPool->second), DEFAULT_URI, trackerID, frameIndex);

			}
		}
	}
}



void NetworkManager::sendPointPool(PointCollection* pointCollection, int trackerID)
{
	// check if point collection exists
	if (pointCollection != nullptr)
	{
		// check if current sender is active
		if (m_poolSender[trackerID]->isActive())
		{
			// send point collection
			m_poolSender[trackerID]->sendPoints(pointCollection, POINT_URI, trackerID);
		}
	}
}

void NetworkManager::createOSCSender(int ID)
{

	m_poolSender.insert({ID, new OSCSender(m_ipAddresses[0], DEFAULT_PORT) });

	if (m_ipAddresses[1] != "")
	{
		m_poolSender.insert({ ID, new OSCSender(m_ipAddresses[1], DEFAULT_PORT) });
	}
}

void NetworkManager::removeNetworkSender(int ID)
{

	m_poolSender.erase(ID);

	Console::log("NetworkManager::removeNetworkSender(): erased Sender " + toString(ID) + ". remaining Sender: " + toString((int)m_poolSender.size()));

}

void NetworkManager::setIPAddress(int idx,  std::string ipAddress)
{
	// Override IP address
	m_ipAddresses[idx] = ipAddress;
	
	// Update IP address for each NetworkSender DOES NOT WORK WITH MULTI ADDRESS
	//for (const std::pair<const int, NetworkSender*> &entry: m_poolSender)
	//{
	//	entry.second->setIPAddress(ipAddress);
	//}
}

bool NetworkManager::isValidIPAddress(std::string ipAddress)
{
	// Check if IP address is valid IPv4 address
	std::regex ipV4RegEx = std::regex(IPV4_REGEX);
	if (std::regex_match(ipAddress, ipV4RegEx)) {
		return true;
	}

	// Check if IP address is valid IPv6 address
	std::regex ipV6RegEx = std::regex(IPV6_REGEX, std::regex::icase);
	return std::regex_match(ipAddress, ipV6RegEx);
}
