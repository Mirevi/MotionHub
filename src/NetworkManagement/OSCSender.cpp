#include "OSCSender.h"

// default constructor
OSCSender::OSCSender()
{

	// create new udp transmit socket sending on address and port
	m_transmitSocket = new UdpTransmitSocket(IpEndpointName(m_address.c_str(), m_port));
	// create new osc outbound packet stream
	m_packetStream = new osc::OutboundPacketStream(buffer, OUTPUT_BUFFER_SIZE);


}

OSCSender::OSCSender(std::string address, int port) : NetworkSender(address, port)
{

	// create new udp transmit socket sending on address and port
	m_transmitSocket = new UdpTransmitSocket(IpEndpointName(m_address.c_str(), m_port));
	// create new osc outbound packet stream
	m_packetStream = new osc::OutboundPacketStream(buffer, OUTPUT_BUFFER_SIZE);



}

// send single skeleton
void OSCSender::sendSkeleton(Skeleton* skeleton, const char* uri, int trackerID)
{

	// check if skeleton exists
	if (skeleton != nullptr)
	{

		int ID = (trackerID * 1000 + skeleton->getSid());



		//first value in stream is user ID
		*m_packetStream << osc::BeginBundleImmediate << osc::BeginMessage(uri) << ID;

		// create new var's for joint position, rotation and confidence
		Vector4f currJointPosition;
		Quaternionf currJointRotation;
		Joint::JointConfidence currJointConfidence = Joint::JointConfidence::NONE;

		//loop through all Joints to write pose data into the stream
		for (int jointsIndex = 0; jointsIndex < skeleton->m_joints.size(); jointsIndex++)
		{

			// get and assign joint position, rotation and confidence
			currJointPosition	= skeleton->m_joints[(Joint::JointNames)jointsIndex].getJointPosition();
			currJointRotation	= skeleton->m_joints[(Joint::JointNames)jointsIndex].getJointRotation();
			currJointConfidence	= skeleton->m_joints[(Joint::JointNames)jointsIndex].getJointConfidence();


			// ADD DATA TO OSC PACKET STREAM:
			*m_packetStream			 
			
				// add position data to stream
				<< currJointPosition.x()
				<< currJointPosition.y()
				<< currJointPosition.z()


				// add rotation data to stream
				<< currJointRotation.x()
				<< currJointRotation.y()
				<< currJointRotation.z()
				<< currJointRotation.w()

				// add joint confidence level to stream
				<< (int)currJointConfidence;

		}

		// add skeleton posture
		*m_packetStream << (int)skeleton->getPosture();

		// close the stream
		*m_packetStream << osc::EndMessage << osc::EndBundle;

		// send packet with data
		m_transmitSocket->Send(m_packetStream->Data(), m_packetStream->Size());

		// clear packet stream to prevent buffer overflow
		m_packetStream->Clear();

	}
}