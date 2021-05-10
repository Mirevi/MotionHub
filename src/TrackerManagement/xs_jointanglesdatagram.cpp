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

#include "xs_jointanglesdatagram.h"

/*! \class JointAnglesDatagram
	\brief a Joint Angle datagram (type 0x20)

	Information about each joint is sent as follows.

	4 bytes parent connection identifier: 256 * segment ID + point ID
	4 bytes child connection identifer: 256 * segment ID + point ID
	4 bytes x rotation
	4 bytes y rotation
	4 bytes z rotation

	Total: 20 bytes per joint

	The coordinates use a Z-Up, right-handed coordinate system.
*/

/*! Constructor */
JointAnglesDatagram::JointAnglesDatagram()
	: Datagram()
{
	setType(SPJointAngles);
}

/*! Destructor */
JointAnglesDatagram::~JointAnglesDatagram()
{
}

/*! Deserialize the data from \a arr
	\sa serializeData
*/
void JointAnglesDatagram::deserializeData(Streamer &inputStreamer)
{
	Streamer* streamer = &inputStreamer;

	for (int i = 0; i < dataCount(); i++)
	{
		Joint joint;

		// Parent Connection ID  -> 4 byte
		streamer->read(joint.parent);

		// Child Connection ID -> 4 byte
		streamer->read(joint.child);

		// Store the Rotation in a Vector -> 12 byte	(3 x 4 byte)
		for (int k = 0; k < 3; k++)
			streamer->read(joint.rotation[k]);

		m_data.push_back(joint);
	}
}

/*! Print Data datagram in a formatted way
*/
void JointAnglesDatagram::printData() const
{
	for (int i = 0; i < m_data.size(); i++)
	{
		std::cout << "Parent Connection ID (256 * segment ID + point ID): " << m_data.at(i).parent << std::endl;
		std::cout << "Child Connection ID (256 * segment ID + point ID): " << m_data.at(i).child << std::endl;
		// Rotation
		std::cout << "Rotation: " << "(";
		std::cout << "x: " << m_data.at(i).rotation[0] << ", ";
		std::cout << "y: " << m_data.at(i).rotation[1] << ", ";
		std::cout << "z: " << m_data.at(i).rotation[2] << ")"<< std::endl << std::endl;
	}
}
