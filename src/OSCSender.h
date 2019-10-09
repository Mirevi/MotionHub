#pragma once

#include "NetworkSender.h"

#include "UdpSocket.h"
#include "OscOutboundPacketStream.h"

#include "defines.h"
#include "Vector3.h"
#include "Vector4.h"

class OSCSender : public NetworkSender
{

private:
	char buffer[OUTPUT_BUFFER_SIZE];
	UdpTransmitSocket* m_transmitSocket = nullptr;
	osc::OutboundPacketStream* m_packetStream = nullptr;

public:

	OSCSender();
	OSCSender(std::string address, int port);

	void sendSkeleton(Skeleton* skeleton, const char* uri) override;

};