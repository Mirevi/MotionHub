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

#include "xs_trackerkinematicsdatagram.h"

/*! \class TrackerKinematicsDatagram
	\brief Tracker kinematics datagram (type 0x23)

	Information about each tracker is sent as follows.

	4 bytes associated segment ID
	4 bytes q1 rotation – sensor rotation quaternion component 1 (re)
	4 bytes q2 rotation – sensor rotation quaternion component 2 (i)
	4 bytes q3 rotation – sensor rotation quaternion component 3 (j)
	4 bytes q4 rotation – sensor rotation quaternion component 4 (k)
	4 bytes x–coordinate of sensor free acc
	4 bytes y–coordinate of sensor free acc
	4 bytes z–coordinate of sensor free acc
	4 bytes x–coordinate of sensor acc
	4 bytes y–coordinate of sensor acc
	4 bytes z–coordinate of sensor acc
	4 bytes x–coordinate of sensor gyr
	4 bytes y–coordinate of sensor gyr
	4 bytes z–coordinate of sensor gyr
	4 bytes x–coordinate of sensor mag
	4 bytes y–coordinate of sensor mag
	4 bytes z–coordinate of sensor mag

	Total: 68 bytes per sensor

	The coordinates use a Z-Up, right-handed coordinate system.
*/

/*! Constructor */
TrackerKinematicsDatagram::TrackerKinematicsDatagram()
	: Datagram()
{
	setType(SPTrackerKinematics);
}


/*! Destructor */
TrackerKinematicsDatagram::~TrackerKinematicsDatagram()
{
}

/*! Deserialize the data from \a arr
	\sa serializeData
*/
void TrackerKinematicsDatagram::deserializeData(Streamer &inputStreamer)
{
	Streamer* streamer = &inputStreamer;

	for (int i = 0; i < dataCount(); i++)
	{
		Kinematics kin;

		// 4 byte
		streamer->read(kin.segmentId);

		// Store the Sensor rotation in a Vector -> 16 byte	(4 x 4 byte)
		for (int k = 0; k < 4; k++)
			streamer->read((float)kin.sens_rot[k]);

		// Store the Sensor free acceleration in a Vector -> 12 byte	(3 x 4 byte)
		for (int k = 0; k < 3; k++)
			streamer->read((float)kin.sen_freeAcc[k]);

		// Store the  Sensor Acceleration in a Vector -> 12 byte	(3 x 4 byte)
		for (int k = 0; k < 3; k++)
			streamer->read((float)kin.sen_acc[k]);

		// Store the Sensor gyroscope in a Vector -> 12 byte	(3 x 4 byte)
		for (int k = 0; k < 3; k++)
			streamer->read((float)kin.sen_gyr[k]);

		// Store the Sensor magnetometer a Vector -> 12 byte	(3 x 4 byte)
		for (int k = 0; k < 3; k++)
			streamer->read((float)kin.sen_mag[k]);

		m_data.push_back(kin);
	}
}

/*! Print Data datagram in a formatted way
*/
void TrackerKinematicsDatagram::printData() const
{
	for (int i = 0; i < m_data.size(); i++)
	{
		std::cout << "Segment ID: " << m_data.at(i).segmentId << std::endl;
		// Sensor rotation quaternion
		std::cout << "Sensor Rotation: " << "(";
		std::cout << "re: " << m_data.at(i).sens_rot[0] << ", ";
		std::cout << "i: " << m_data.at(i).sens_rot[1] << ", ";
		std::cout << "j: " << m_data.at(i).sens_rot[1] << ", ";
		std::cout << "k: " << m_data.at(i).sens_rot[2] << ")"<< std::endl;

		// Sensor free acceleration
		std::cout << "Sensor free acceleration: " << "(";
		std::cout << "x: " << m_data.at(i).sen_freeAcc[0] << ", ";
		std::cout << "y: " << m_data.at(i).sen_freeAcc[1] << ", ";
		std::cout << "z: " << m_data.at(i).sen_freeAcc[2] << ")"<< std::endl;

		// Sensor Acceleration
		std::cout << "Sensor Acceleration: " << "(";
		std::cout << "x: " << m_data.at(i).sen_acc[0] << ", ";
		std::cout << "y: " << m_data.at(i).sen_acc[1] << ", ";
		std::cout << "z: " << m_data.at(i).sen_acc[2] << ")"<< std::endl;

		// Sensor gyroscope
		std::cout << "Sensor gyroscope: " << "(";
		std::cout << "x: " << m_data.at(i).sen_gyr[0] << ", ";
		std::cout << "y: " << m_data.at(i).sen_gyr[1] << ", ";
		std::cout << "z: " << m_data.at(i).sen_gyr[2] << ")"<< std::endl;

		// Sensor magnetometer
		std::cout << "Sensor magnetometer: " << "(";
		std::cout << "x: " << m_data.at(i).sen_mag[0] << ", ";
		std::cout << "y: " << m_data.at(i).sen_mag[1] << ", ";
		std::cout << "z: " << m_data.at(i).sen_mag[2] << ")"<< std::endl << std::endl;
	}
}
