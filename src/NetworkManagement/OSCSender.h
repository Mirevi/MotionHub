#pragma once

#include "NetworkSender.h"

#include "UdpSocket.h"
#include "OscOutboundPacketStream.h"

#include "defines.h"
#include "MotionHubUtil/Vector3.h"
#include "MotionHubUtil/Vector4.h"
#include "MotionHubUtil/Skeleton.h"
#include "MotionHubUtil/Console.h"
#include "MotionHubUtil/Landmark.h"

#include <vector>

/*!
 * \class OSCSender
 *
 * \brief Implements OSC Protocol
 *
 * \author Kester Evers and Eric Jansen
 */
class OSCSender : public NetworkSender
{

private:

	char buffer[OUTPUT_BUFFER_SIZE];
	UdpTransmitSocket* m_transmitSocket = nullptr;
	osc::OutboundPacketStream* m_packetStream = nullptr;

public:

	/*!
	 * default constructor
	 * 
	 */
	OSCSender();
	/*!
	 * standard constructor
	 *
	 * \param address the receivers address
	 * \param port the receivers target port
	 *
	 */
	OSCSender(std::string address, int port);

	/*!
	 * sends skeleton data via OSC protocol
	 *
	 * \param skeleton input skeleton data
	 * \param uri messages title
	 */
	void sendSkeleton(Skeleton* skeleton, const char* uri, int trackerID) override;
	/*!
	 * sends image landmarks data via OSC protocol
	 *
	 * \param imageLandmarks input image landmarks data
	 * \param uri messages title
	 */
	void sendImageLandmarks(std::vector<Landmark>* imageLandmarks, const char* uri) override;

};