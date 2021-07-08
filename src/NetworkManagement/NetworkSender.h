#pragma once

#include <iostream>
#include "MotionHubUtil/Skeleton.h"

/*!
 * \class NetworkSender
 *
 * \brief Prototype class for implemetation - holds virtual methods
 *
 * \author Kester Evers and Eric Jansen
 */
class NetworkSender
{
	
private:
	/*!
	 * activity state of current NetworkSender
	 * 
	 */
	bool m_active = true;

public:
	/*!
	 * default constructor
	 * 
	 */
	NetworkSender();
	/*!
	 * standard constructor
	 * 
	 * \param address the receivers address
	 * \param port the receivers target port
	 * 
	 */
	NetworkSender(std::string address, int port);	
	/*!
	 * getter for activity state
	 * 
	 * \return m_active
	 */
	bool isActive();
	/*!
	 * setter for activity state
	 * 
	 * \param active new activity state
	 */
	void setActive(bool active);
	/*!
	 * sends skeleton data
	 * 
	 * \param skeleton input skeleton data
	 * \param uri messages title
	 */
	virtual void sendSkeleton(Skeleton* skeleton, const char* uri, int trackerID, int frameIndex) = 0;

	/*!
	 * overrides the receivers address
	 *
	 * \param address the receivers address
	 */
	virtual void setIPAddress(std::string address) = 0;
protected:

	/*!
	 * the receivers address
	 * 
	 */
	std::string m_address;
	/*!
	 * the receivers target port
	 * 
	 */
	int m_port;

};