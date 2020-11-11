/*! \file
	\section FileCopyright Copyright Notice
	This is free and unencumbered software released into the public domain.

	Anyone is free to copy, modify, publish, use, compile, sell, or
	distribute this software, either in source code form or as a compiled
	binary, for any purpose, commercial or non-commercial, and by any
	means.

	In jurisdictions that recognize copyright laws, the author or authors
	of this software dedicate any and all copyright interest in the
	software to the public domain. We make this dedication for the benefit
	of the public at large and to the detriment of our heirs and
	successors. We intend this dedication to be an overt act of
	relinquishment in perpetuity of all present and future rights to this
	software under copyright law.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#include "xs_datagram.h"
#include <xstypes/xstimestamp.h>

/*! \class Datagram

	Deserialize the data. The header data is interpreted by this class.

	\param[in] arr The data array. This is the data part of the datagram, stripped from the header.
	\param[in] precision The floating point precision. By default this is set to single precision.

	\see MVN Studio real-time network streaming protocol specification
*/

const float Datagram::EULERPOSITIONSCALE = 100.0;

Datagram::Datagram()
	: m_header("MXTP00")
	, m_avatarId(0)
	, m_dgramCounter(0x80)
	, m_dataCount(0)
	, m_frameTime(0)
	, m_sampleCounter(0)

	, m_bodySegmentCount(23)
	, m_propCount(0)
	, m_fingerTrackingSegmentCount(0)

	, m_dataSize(0)
{
	initMap(m_packetsName);
}

/*! Destructor */
Datagram::~Datagram()
{
}

/*! The data dimension of the datagram */
int Datagram::getDataSize() const
{
	return m_dataSize;
}

/*! The datagrams message type */
int Datagram::messageType(const XsByteArray &array)
{
	if (array[0] == '{')
		return SPUnknown;

	try
	{
		std::stringstream tp;
		// extract the 5th and 6th digits that represent the code of the packet
		tp << array[4] << array[5];
		std::string type;
		tp >> type;
		// convert to hex
		int sp = std::stoi(type,nullptr,16);

		return sp;
	}
	catch (...)
	{
		return SPUnknown;
	}
}

/*! The datagrams message type */
int Datagram::messageType() const
{
	std::string header = m_header;
	std::string type = header.substr(4, header.size()-4);
	int sp = std::stoi (type,nullptr,16);

	return sp;
}

/*! Set the type of the message */
void Datagram::setType(StreamingProtocol proto)
{
	std::stringstream hexSS;
	hexSS.width(2);
	hexSS.fill('0');
	hexSS << std::hex << proto;
	m_header = "MXTP" +  hexSS.str();
}

/*! Deserializes the datagram from given byte array \a arr.
*/
bool Datagram::deserialize(const XsByteArray& arr)
{
	Streamer streamer(arr);
	std::string messageType;

	// extract only the byte for the header (24 bytes)
	streamer.read(messageType,6);		// 6 bytes
	streamer.read(m_sampleCounter);		// 4 bytes
	streamer.read(m_dgramCounter);		// 1 byte
	streamer.read(m_dataCount);			// 1 byte
	streamer.read(m_frameTime);			// 4 bytes
	streamer.read(m_avatarId);			// 1 byte

	streamer.read(m_bodySegmentCount);				// 1 byte, introduced in MVN 2019 / 2018.3 beta
	streamer.read(m_propCount);						// 1 byte, introduced in MVN 2019 / 2018.3 beta
	streamer.read(m_fingerTrackingSegmentCount);	// 1 byte, introduced in MVN 2019 / 2018.3 beta

	uint8_t reserved;
	streamer.read(reserved);			// 1 byte
	streamer.read(reserved);			// 1 byte

	streamer.read(m_dataSize);			// 2 bytes, introduced in MVN 2018

	// deserialize the data part of the Packet
	deserializeData(streamer);

	return true;
}

void Datagram::printHeader() const
{
	std::cout	<< "\n************* " << decode(static_cast<StreamingProtocol>(messageType()))
				<< "(" << (int)avatarId() << ")"
				<< " [" << (int)datagramCounter() << "] "
				<< getDataSize() << " bytes *************"

				<< "\nNumber of items: " << (int)dataCount()
				<< "\tSample counter: " << sampleCounter()
				<< "\tFrame Time: " <<  frameTime()

				<< "\nBody Segment Count: " << bodySegmentCount()
				<< "\tProp Count: " << propCount()
				<< "\tFingerTS Count: " << fingerTrackingSegmentCount() << std::endl;
}

/*! Return the avatar ID

  MVN Studio now supports multiple avatars in one viewport. This byte specifies to which avatar the data belongs. In a single-avatar setup this value will always be 0. In multi-avatar cases, they will usually be incremental. However, especially during live streaming, one of the avatars may disconnect and stop sending data while others will continue so the receiver should be able to handle this.
  Each avatar will send its own full packet.

  \sa setAvatarId
*/
uint8_t Datagram::avatarId() const
{
	return m_avatarId;
}

/*! Return the amount of data items in this datagram

  The number of items is stored as an 8-bit unsigned integer value. This number indicates the number of segments or points that are contained in the packet. Note that this number is not necessarily equal to the total number of sensors that were being captured at the sampling instance if the motion capture data was split up into several datagrams. This number may instead be used to verify that the entire UDP datagram has been fully received by calculating the expected size of the datagram and comparing it to the actual size of the datagram.
*/
uint8_t Datagram::dataCount() const
{
	return m_dataCount;
}

/*! Set the number of items in this packet */
void Datagram::setDataCount(uint8_t c)
{
	m_dataCount = c;
}

/*! The sample counter is a 32-bit unsigned integer value which is incremented by one each time a new set of motion sensor data is sampled and sent away. Note that the sample counter is not to be interpreted as a time code, since the sender may skip frames. */
int32_t Datagram::sampleCounter() const
{
	return m_sampleCounter;
}

/*! Return the datagram counter

The size of a UDP datagram is usually limited by the MTU (maximum transmission unit, approx. 1500 bytes) of the underlying Ethernet network.
In nearly all cases the entire motion data that was collected at one sampling instance will fit into a single UDP datagram.
However, if the amount of motion data becomes too large then the data is split up into several datagrams.

If motion data is split up into several datagrams then the datagrams receive index numbers starting at zero.

The datagram counter is a 7-bit unsigned integer value which stores this index number.
The most significant bit of the datagram counter byte is used to signal that this datagram is the last one belonging to that sampling instance.
For example, if motion data is split up into three datagrams then their datagram counters will have the values 0, 1 and 0x82 (hexadecimal).
If all data fits into one UDP datagram (the usual case) then the datagram counter will be equal to 0x80 (hexadecimal).

The sample counter mentioned above can be used to identify which datagrams belong to the same sampling instance because they must all carry
the same sample counter value but different datagram counters.
This also means that the combination of sample counter and datagram counter values is unique for each UDP datagram containing (part of the) motion data.

*/
uint8_t Datagram::datagramCounter() const
{
	return m_dgramCounter;
}

/*! The frametime associated with this datagram

  MVN Studio contains a clock which starts running at the start of a recording. The clock measures the elapsed time in milliseconds. Whenever new captured data is sampled the current value of the clock is sampled as well and is stored inside the datagram(s) as a 32-bit unsigned integer value representing a time code.
*/
int32_t Datagram::frameTime() const
{
	return m_frameTime;
}

/*! Map the StreamingProtocol names to a user friendly version
*/
void Datagram::initMap(std::map<int, std::string> &map)
{
	map[SPPoseEuler] = "Position + Orientation (Euler)";
	map[SPPoseQuaternion] = "Position + Orientation (Quaternion)";
	map[SPPosePositions] = "Virtual Optical Marker Set";
	map[SPJackProcessSimulate] = "Siemens Tecnomatix";
	map[SPPoseUnity3D] = "Unity 3D";

	map[SPMetaMoreMeta] = "Character Meta Data";
	map[SPMetaScaling] = "Scaling Data";

	map[SPJointAngles] = "Joint Angles";
	map[SPLinearSegmentKinematics] = "Linear Segment Kinematics";
	map[SPAngularSegmentKinematics] = "Angular Segment Kinematics";
	map[SPTrackerKinematics] = "Tracker Kinematics";
	map[SPCenterOfMass] = "Center of Mass";
	map[SPTimeCode] = "Time Code";
}

/*! Convert the StreamingProtocol to user frindly string name
*/
std::string Datagram::decode(StreamingProtocol proto) const
{
	if(m_packetsName.find(proto) != m_packetsName.end())
	{
		//element found;
		return m_packetsName.find(proto)->second;
	}
	return std::string();
}

void Datagram::convertFromYupToZup(float *vector) const
{
	// Covert the coordinates to Z-Up
	float x = vector[0];
	float y = vector[1];
	float z = vector[2];

	vector[0] = z;
	vector[1] = x;
	vector[2] = y;
}
