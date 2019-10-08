#pragma once

#include "NetworkSender.h"

#include "UdpSocket.h"
#include "OscOutboundPacketStream.h"

#define OUTPUT_BUFFER_SIZE 1024

class OSCSender : public NetworkSender
{

private:
	UdpTransmitSocket* m_transmitSocket = nullptr;
	osc::OutboundPacketStream* m_packetStream = nullptr;

public:

	OSCSender();
	OSCSender(const char* address, int port);

	void sendSkeleton(Skeleton* skeleton, const char* uri) override;

};