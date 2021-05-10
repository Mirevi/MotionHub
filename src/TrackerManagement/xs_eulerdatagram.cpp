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

#include "xs_eulerdatagram.h"
#include <xstypes/xsquaternion.h>
#include <xstypes/xseuler.h>

/*! \class EulerDatagram
	\brief a Position & Euler orientation pose datagram (type 01)

	Segment data Euler (type 01)
	Information about each segment is sent as follows.

	4 bytes segment ID, in the range 1-30
	4 bytes x of segment position
	4 bytes y of segment position
	4 bytes z of segment position
	4 bytes x of segment rotation
	4 bytes y of segment rotation
	4 bytes z of segment rotation

	Total: 28 bytes per segment

	The coordinates use a Y-Up, right-handed coordinate system.
*/

/*! Constructor */
EulerDatagram::EulerDatagram()
	: Datagram()
{
	setType(SPPoseEuler);
}

/*! Destructor */
EulerDatagram::~EulerDatagram()
{
}

/*! Deserialize the data from \a arr
	\sa serializeData
*/
void EulerDatagram::deserializeData(Streamer &inputStreamer)
{
	Streamer* streamer = &inputStreamer;

	for (int i = 0; i < dataCount(); i++)
	{
		Kinematics kin;
		float orientation[4];
		float rotation[3];
		XsQuaternion quaternion;
		XsQuaternion tempQuat;

		// Store the segement Id -> 4 byte
		streamer->read(kin.segmentId);

		// Store the position in a Vector. The coordinates use a Y-Up
		for (int k = 0; k < 3; k++)
			streamer->read((float)kin.pos[k]);

		for (int k = 0; k < 3; k++)
			kin.pos[k] /= EULERPOSITIONSCALE;

		// Covert the coordinates to Z-Up
		convertFromYupToZup(kin.pos);

		// The rotation is based to the coordinates Y-Up
		for (int k = 0; k < 3; k++)
			streamer->read((float)rotation[k]);

		// create Euler vector based from the rotation cordinates
		XsEuler euler(rotation[0], rotation[1], rotation[2]);

		// convert the Euler vector to quaternion
		quaternion.fromEulerAngles(euler);

		// create a quaternion with the inverted components (x,y,z)
		tempQuat = XsQuaternion(quaternion[0], quaternion[3], quaternion[1], quaternion[2]);

		// covert from Euler to Quaternion again
		euler.fromQuaternion(tempQuat);

		kin.rotation[0] = euler[0];
		kin.rotation[1] = euler[1];
		kin.rotation[2] = euler[2];

		m_data.push_back(kin);
	}
}

/*! Print Data datagram in a formatted way
*/
void EulerDatagram::printData() const
{
	for (int i = 0; i < m_data.size(); i++)
	{
		std::cout << "Segment ID: " << m_data.at(i).segmentId << std::endl;

		// Position
		std::cout << "Segment Position: " << "(";
		std::cout << "x: " << m_data.at(i).pos[0] << ", ";
		std::cout << "y: " << m_data.at(i).pos[1] << ", ";
		std::cout << "z: " << m_data.at(i).pos[2] << ")"<< std::endl;

		// Orientation
		std::cout << "Segment rotation: " << "(";
		std::cout << "x: " << m_data.at(i).rotation[0] << ", ";
		std::cout << "y: " << m_data.at(i).rotation[1] << ", ";
		std::cout << "z: " << m_data.at(i).rotation[2] << ")"<< std::endl << std::endl;
	}
}
