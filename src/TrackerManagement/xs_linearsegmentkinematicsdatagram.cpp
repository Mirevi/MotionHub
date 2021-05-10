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

#include "xs_linearsegmentkinematicsdatagram.h"

/*! \class LinearSegmentKinematicsDatagram
	\brief a Linear Segment Kinematics datagram (type 0x21)

	Information about each segment is sent as follows.

	4 bytes segment ID, in the range 1-30
	4 bytes x–coordinate of segment position
	4 bytes y–coordinate of segment position
	4 bytes z–coordinate of segment position
	4 bytes x–coordinate of segment velocity
	4 bytes y–coordinate of segment velocity
	4 bytes z–coordinate of segment velocity
	4 bytes x–coordinate of segment acceleration
	4 bytes y–coordinate of segment acceleration
	4 bytes z–coordinate of segment acceleration

	Total: 40 bytes per segment

	The coordinates use a Z-Up, right-handed coordinate system.
	*/

/*! Constructor */
LinearSegmentKinematicsDatagram::LinearSegmentKinematicsDatagram()
	: Datagram()
{
	setType(SPLinearSegmentKinematics);
}

/*! Destructor */
LinearSegmentKinematicsDatagram::~LinearSegmentKinematicsDatagram()
{
}

/*! Deserialize the data from \a arr
	\sa serializeData
*/
void LinearSegmentKinematicsDatagram::deserializeData(Streamer &inputStreamer)
{
	Streamer* streamer = &inputStreamer;

	for (int i = 0; i < dataCount(); i++)
	{
		Kinematics kin;

		// Store the segement Id -> 4 byte
		streamer->read(kin.segmentId);

		// Store the Segment Position in a Vector -> 12 byte	(3 x 4 byte)
		for (int k = 0; k < 3; k++)
			streamer->read(kin.pos[k]);

		// Store the Segment Velocity in a Vector -> 12 byte	(3 x 4 byte)
		for (int k = 0; k < 3; k++)
			streamer->read(kin.velocity[k]);

		// Store the Segmetn Acceleration in a Vector -> 12 byte	(3 x 4 byte)
		for (int k = 0; k < 3; k++)
			streamer->read(kin.acceleration[k]);

		m_data.push_back(kin);
	}
}

/*! Print Data datagram in a formatted way
*/
void LinearSegmentKinematicsDatagram::printData() const
{
	for (int i = 0; i < m_data.size(); i++)
	{
		std::cout << "Segment ID: " << m_data.at(i).segmentId << std::endl;
		// Segment Position
		std::cout << "Segment Position: " << "(";
		std::cout << "x: " << m_data.at(i).pos[0] << ", ";
		std::cout << "y: " << m_data.at(i).pos[1] << ", ";
		std::cout << "z: " << m_data.at(i).pos[2] << ")"<< std::endl;

		// Segment Velocity
		std::cout << "Segment Velocity: " << "(";
		std::cout << "x: " << m_data.at(i).velocity[0] << ", ";
		std::cout << "y: " << m_data.at(i).velocity[1] << ", ";
		std::cout << "z: " << m_data.at(i).velocity[2] << ")"<< std::endl;

		// Segment Acceleration
		std::cout << "Segment Acceleration: " << "(";
		std::cout << "x: " << m_data.at(i).acceleration[0] << ", ";
		std::cout << "y: " << m_data.at(i).acceleration[1] << ", ";
		std::cout << "z: " << m_data.at(i).acceleration[2] << ")"<< std::endl << std::endl;
	}
}
