#pragma once

#include "ConfigDllExportNetworkManagement.h"

#include "MotionHubUtil/Skeleton.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/ConfigManager.h"
#include "MotionHubUtil/Landmark.h"

#include "defines.h"

#include "OSCSender.h"

#include <string>
#include <iostream>
#include <vector>

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
	 * sends skeleton pool with a specified sender
	 * \param skeletonPool skeleton pool to send
	 */
	void sendSkeletonPool(std::map<int, Skeleton>* skeletonPool, int trackerId);

	/*!
	 * sends the imgage landmarks with a specified sender
	 * \param imgageLandmarks imgage landmarks to send
	 * \param senderId the id of the sender
	 */
	void sendImageLandmarks(std::vector<Landmark>* imgageLandmarks, int senderId);

	void createOSCSender(int ID);

	void removeNetworkSender(int ID);



	std::string m_ipAddress;



private:
	/*!
	 *  pool of all network sender
	 */
	std::map<int, NetworkSender*> m_poolSender;

	ConfigManager* m_configManager;
	
};