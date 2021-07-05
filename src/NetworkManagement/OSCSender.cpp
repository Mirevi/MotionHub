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

			Joint::JointNames currType = (Joint::JointNames)jointsIndex;


			// get and assign joint position, rotation and confidence
			currJointPosition	= skeleton->m_joints[currType].getJointPosition();
			currJointRotation	= skeleton->m_joints[currType].getJointRotation();
			currJointConfidence	= skeleton->m_joints[currType].getJointConfidence();

			//Console::log("OSCSender::sendSkeleton(): type = " + Joint::toString(currType) + ", rotation = " + toString(currJointRotation));

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

			//Console::log("OSCSender::sendSkeleton(): currJointPosition: " + toString(currJointPosition));
			//Console::log("OSCSender::sendSkeleton(): currJointRotation: " + toString(currJointRotation));


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

void OSCSender::sendPoints(PointCollection* pointCollection, const char* uri, int trackerID)
{
	// exit if point collection is null
	if (pointCollection == nullptr) {
		return;
	}

	// begin packet stream & add tracker ID as first value
	*m_packetStream << osc::BeginBundleImmediate << osc::BeginMessage(uri);

	// create new var's for id, position, rotation 
	int id;
	Point point;

	Vector3f position;
	Quaternionf rotation;

	int type;

	int customInt;
	float customFloat;

	// add point count to message stream
	*m_packetStream << (int)pointCollection->points.size();

	// loop through all Points to write data into the stream
	for (const std::pair<int, Point>& item : pointCollection->points) {

		id = item.first;
		point = item.second;

		position = point.getPosition();
		rotation = point.getRotation();

		type = point.getType();

		customInt = point.getCustomInt();
		customFloat = point.getCustomFloat();

		// Add data to OSC packet stream:
		*m_packetStream

			// Add position data to stream
			<< position.x()
			<< position.y()
			<< position.z()

			// Add rotation data to stream
			<< rotation.x()
			<< rotation.y()
			<< rotation.z()
			<< rotation.w()

			// Add id to stream
			<< id

			// Add type to stream
			<< type

			// Add custom data to stream
			<< customInt
			<< customFloat;
	}

	// close the stream
	*m_packetStream << osc::EndMessage << osc::EndBundle;

	// send packet with data
	m_transmitSocket->Send(m_packetStream->Data(), m_packetStream->Size());

	// clear packet stream to prevent buffer overflow
	m_packetStream->Clear();
}

void OSCSender::setIPAddress(std::string address) {

	// override IP address
	m_address = address;

	// destroy old udp transmit socket
	delete m_transmitSocket;

	// create new udp transmit socket sending on address and port
	m_transmitSocket = new UdpTransmitSocket(IpEndpointName(m_address.c_str(), m_port));
}
