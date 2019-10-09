#include "OSCSender.h"

OSCSender::OSCSender()
{
	m_transmitSocket = new UdpTransmitSocket(IpEndpointName(m_address.c_str(), m_port));

	m_packetStream = new osc::OutboundPacketStream(buffer, OUTPUT_BUFFER_SIZE);

}

OSCSender::OSCSender(std::string address, int port) : NetworkSender(address, port)
{
	m_address;
	m_transmitSocket = new UdpTransmitSocket(IpEndpointName(m_address.c_str(), m_port));

	m_packetStream = new osc::OutboundPacketStream(buffer, OUTPUT_BUFFER_SIZE);

}

void OSCSender::sendSkeleton(Skeleton* skeleton, const char* uri)
{
	//first value in stream is user ID
	*m_packetStream << osc::BeginBundleImmediate << osc::BeginMessage(uri) << skeleton->getSid();

	Vector3 currJointPosition = Vector3::zero();
	Vector4 currJointRotation = Vector4::zero();
	

	//loop through all Joints to write pose data into the stream
	for (int jointsIndex = 0; jointsIndex < skeleton->m_joints.size(); jointsIndex++)
	{

		currJointPosition = skeleton->m_joints[(Joint::jointNames)jointsIndex].getJointPosition();
		currJointRotation = skeleton->m_joints[(Joint::jointNames)jointsIndex].getJointRotation();

		//if (jointsIndex == 0)
		//{
		//	Console::log("send Skeleton: " + std::to_string(skeleton->getSid()) + " on position " + currJointPosition.toString());
		//}

		*m_packetStream
			//add position data to stream
			<< (currJointPosition.m_xyz.x * (-1)) / 1000
			<< ((currJointPosition.m_xyz.y * (-1)) + 950) / 1000
			<< currJointPosition.m_xyz.z / 1000
			//add rotation data to stream
			<< currJointRotation.m_xyzw.x
			<< currJointRotation.m_xyzw.y
			<< currJointRotation.m_xyzw.z
			<< currJointRotation.m_xyzw.w;

	}
	
	//close the stream
	*m_packetStream << osc::EndMessage << osc::EndBundle;

	//send a packet with the datastream
	m_transmitSocket->Send(m_packetStream->Data(), m_packetStream->Size());

	// clear packet stream to prevent buffer overflow
	m_packetStream->Clear();

}