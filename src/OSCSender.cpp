#include "OSCSender.h"

OSCSender::OSCSender()
{
	
	NetworkSender::NetworkSender();

	m_transmitSocket = new UdpTransmitSocket(IpEndpointName(m_address, m_port));

	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

}

OSCSender::OSCSender(const char* address, int port)
{

	m_address = address;
	m_port = port;

	m_transmitSocket = new UdpTransmitSocket(IpEndpointName(m_address, m_port));

	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

}

void OSCSender::sendSkeleton(Skeleton* skeleton, const char* uri)
{

	//first value in stream is user ID
	*m_packetStream << osc::BeginBundleImmediate << osc::BeginMessage(uri) << skeleton->getSid();

	//loop through all Joints to write pose data into the stream
	for (int jointsIndex = 0; jointsIndex < 26; jointsIndex++)
	{
		//add position data to stream
		m_packetStream << (skeleton->m_joints[(Joint::jointNames)jointsIndex].getJointPosition.m_xyz.x * (-1)) / 1000
			<< ((skeleton->m_joints[(Joint::jointNames)jointsIndex].getJointPosition.m_xyz.y * (-1)) + 950) / 1000
			<< skeleton->m_joints[(Joint::jointNames)jointsIndex].getJointPosition.m_xyz.z / 1000

			//add rotation data to stream
			<< skeleton->m_joints[(Joint::jointNames)jointsIndex].getJointRotation.m_xyzw.x
			<< skeleton->m_joints[(Joint::jointNames)jointsIndex].getJointRotation.m_xyzw.y
			<< skeleton->m_joints[(Joint::jointNames)jointsIndex].getJointRotation.m_xyzw.z
			<< skeleton->m_joints[(Joint::jointNames)jointsIndex].getJointRotation.m_xyzw.w;

	}

	//close the stream
	*m_packetStream << osc::EndMessage << osc::EndBundle;

	//send a packet with the datastream
	m_transmitSocket->Send(m_packetStream->Data(), m_packetStream->Size());

	// clear packet stream to prevent buffer overflow
	m_packetStream->Clear();

}