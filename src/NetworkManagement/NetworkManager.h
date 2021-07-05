#pragma once

#include "ConfigDllExportNetworkManagement.h"

#include "MotionHubUtil/Skeleton.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/ConfigManager.h"

#include "defines.h"

#include "OSCSender.h"

#include <string>
#include <iostream>
#include <vector>
#include <regex>

/*!
 * \class NetworkManager
 *
 * \brief Manager for different Network Sender and skeleton sending
 *
 * \note Supported Protocols: OSC
 * 
 * \author Kester Evers and Eric Jansen
 */
class NetworkManagement_DLL_import_export NetworkManager
{

public:
	/*!
	 * default constructor 
	 */
	NetworkManager(ConfigManager* configManager);
	/*!
	 * sends skeleton pool to all network sender
	 * \param skeletonPool skeleton pool to send
	 */
	void sendSkeletonPool(std::map<int, Skeleton>* skeletonPool, int trackerID);

	/*!
	 * sends point collection to all network sender
	 * \param pointCollection point collection to send
	 */
	void sendPointPool(PointCollection* pointCollection, int trackerID);

	void createOSCSender(int ID);

	void removeNetworkSender(int ID);

	void setIPAddress(std::string ipAddress);

	/*!
	 * Checks if IP address is within IPv4 range 0.0.0.0 - 255.255.255.255 OR a valid IPv6 address
	 * 
	 * \param ipAddress IP address to validate
	 * \return true if valid, false if invalid
	 */
	bool isValidIPAddress(std::string ipAddress);

	std::string m_ipAddress = "127.0.0.1";



private:
	/*!
	 *  pool of all network sender
	 */
	std::map<int, NetworkSender*> m_poolSender;

	ConfigManager* m_configManager;
	
};