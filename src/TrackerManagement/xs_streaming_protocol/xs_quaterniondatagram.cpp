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

#include "xs_quaterniondatagram.h"
#include <xstypes/xsmath.h>

/*! \class QuaternionDatagram
  \brief a Position & Quaternion orientation pose datagram (type 02)

  Information about each segment is sent as follows.

  4 bytes segment ID, in the range 1-30
  4 bytes x–coordinate of sensor position
  4 bytes y–coordinate of sensor position
  4 bytes z–coordinate of sensor position
  4 bytes q1 rotation – sensor rotation quaternion component 1 (re)
  4 bytes q2 rotation – sensor rotation quaternion component 1 (i)
  4 bytes q3 rotation – sensor rotation quaternion component 1 (j)
  4 bytes q4 rotation – sensor rotation quaternion component 1 (k)

  Total: 32 bytes per segment

  The coordinates use a Z-Up, right-handed coordinate system.
 */



 /*! Constructor */
QuaternionDatagram::QuaternionDatagram()
	: Datagram()
{
	setType(SPPoseQuaternion);

}

/*! Destructor */
QuaternionDatagram::~QuaternionDatagram()
{

}

/*! Deserialize the data from \a arr
	\sa serializeData
*/
void QuaternionDatagram::deserializeData(Streamer& inputStreamer)
{
	Streamer* streamer = &inputStreamer;

	for (int i = 0; i < dataCount(); i++)
	{
		Kinematics kin;

		// 4 byte
		streamer->read(kin.segmentId);

		// Store the Sensor Position in a Vector -> 12 byte	(3 x 4 byte)
		// The coordinates use a Z-Up, right-handed coordinate system.
		for (int k = 0; k < 3; k++)
			streamer->read(kin.position[k]);

		// Store the Quaternion Rotation in a vector -> 16 byte	(4 x 4 byte)
		for (int k = 0; k < 4; k++)
			streamer->read(kin.orientation[k]);

		// trasform in degrees
		for (int k = 0; k < 4; k++)
			kin.orientation[k] = XsMath_rad2deg(kin.orientation[k]);

		m_data.push_back(kin);
	}
}

/*! Print Data datagram in a formatted way
*/
void QuaternionDatagram::printData() const
{
	const int bodySegmentMax = bodySegmentCount();
	const int propSegmentMax = bodySegmentMax + propCount();
	const int leftHandSegmentMax = propSegmentMax + fingerTrackingSegmentCount() / 2;
	//const int rightHandSegmentMax = leftHandSegmentMax + fingerTrackingSegmentCount()/2;

	for (int i = 0; i < m_data.size(); i++)
	{
		if (i < bodySegmentMax)
			std::cout << "Body Segment (ID): " << m_data.at(i).segmentId << std::endl;
		else if (i < propSegmentMax)
			std::cout << "Prop Segment (ID): " << m_data.at(i).segmentId << std::endl;
		else if (i < leftHandSegmentMax)
			std::cout << "Left Hand Segment (ID): " << m_data.at(i).segmentId - propSegmentMax << std::endl;
		else
			std::cout << "Right Hand Segment (ID): " << m_data.at(i).segmentId - leftHandSegmentMax << std::endl;

		// Position
		std::cout << "Segment Position: " << "(";
		std::cout << "x: " << m_data.at(i).position[0] << ", ";
		std::cout << "y: " << m_data.at(i).position[1] << ", ";
		std::cout << "z: " << m_data.at(i).position[2] << ")" << std::endl;

		// Quaternion Orientation
		std::cout << "Quaternion Orientation: " << "(";
		std::cout << "re: " << m_data.at(i).orientation[0] << ", ";
		std::cout << "i: " << m_data.at(i).orientation[1] << ", ";
		std::cout << "j: " << m_data.at(i).orientation[2] << ", ";
		std::cout << "k: " << m_data.at(i).orientation[3] << ")" << std::endl << std::endl;
	}

}

/*! Get Data datagram in a formatted way
*/
std::vector<QuaternionDatagram::Kinematics>* QuaternionDatagram::getData()
{
	return &m_data;
}
