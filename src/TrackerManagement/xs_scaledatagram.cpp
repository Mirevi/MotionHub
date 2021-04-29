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

#include "xs_scaledatagram.h"

/*! \class ScaleDatagram
	Scale information (type 0x10)
	This packet contains scaling information about the character. This information is only useful for MVN Studio, since it only applies to a mesh of a specific size. However, there may be a way to put this information to use for other meshes.

	The data sent per item is:

	4 bytes segment ID, in the range 1-30
	4 bytes x–offset of segment relative to default position
	4 bytes y–offset of segment relative to default position
	4 bytes z–offset of segment relative to default position

	9x4 bytes matrix containing the rotation and scale of the segment relative to the default
	n bytes containing a null-terminated string with the name of the segment

	Total: unknown, minimum  bytes per segment

	The coordinates use a Z-Up, right-handed coordinate system.
*/

ScaleDatagram::ScaleDatagram()
	: Datagram()
{
	setType(SPMetaScaling);
}


/*! Destructor */
ScaleDatagram::~ScaleDatagram()
{
}

/*! Deserialize the scale packets
	The Scale packets are divided in parts, to avoid exceed the 1500 byte packet size limit. For a normal biped this results in a total of 5 packets.
	To deserialize the packets whe should follow this rules:
	- The first packet contains the null pose definition (T-pose);
	- The points will end up with a varying number per datagram with 0 segments (Points Definitation packets).
	- The last packet is the one that will end up in one datagram with 0 points.
*/
void ScaleDatagram::deserializeData(Streamer &inputStreamer)
{
	Streamer* streamer = &inputStreamer;
	// The first packet contains the null pose definition.

	// 4 bytes: the number of segments as an unsigned integer
	int32_t numberOfSegments = 0;
	streamer->read((int32_t)numberOfSegments);

	for (int i=0; i < numberOfSegments; i++)
	{
		NullPoseDefinition nullPosDef;

		// String: the name of the segment
		int32_t stringSize = 0;
		streamer->read((int32_t)stringSize);

		std::string str;
		streamer->read(str, stringSize);
		nullPosDef.segmentName = str;

		// 3-component vector: the position of the origin of the segment in the null pose
		for (int k = 0; k < 3; k++)
			streamer->read((float)nullPosDef.pos[k]);

		m_tPose.push_back(nullPosDef);
	}

/* The 0 segments format is used as identifier for "only update, don't discard"
	The points will end up with a varying number per datagram with 0 segments (Points Definitation packets).
*/
	if (numberOfSegments == 0)
	{
		// 4 bytes containing an unsigned integer: the number of points
		int32_t numberOfPoints = 0;
		streamer->read((int32_t)numberOfPoints);
		for (int i=0; i < numberOfPoints; i++)
		{
			PointDefinition pointDef;

			// 2 bytes: the id of the segment containing the point
			streamer->read((int16_t)pointDef.segmentId);

			// 2 bytes: the point id of the point within the segment
			streamer->read((int16_t)pointDef.pointId);

			// The last packet is the one that will end up in one datagram with 0 points.
			if (pointDef.pointId < 0)
				break;

			// String: the name of the segment
			int32_t stringSize = 0;
			streamer->read((int32_t)stringSize);
			std::string str;
			streamer->read(str, stringSize);
			pointDef.segmentName = str;

			// 4 bytes: unsigned integer containing flags describing the point’s characteristics
			streamer->read((int32_t)pointDef.characteristicOfPoint);

			// 3-component vector: the position of the point relative to the segment origin in the null pose
			for (int k = 0; k < 3; k++)
				streamer->read((float)pointDef.pos[k]);

			m_pointDefinitions.push_back(pointDef);
		}
	}
}



/*! Print Data datagram in a formatted way
*/
void ScaleDatagram::printData() const
{
	for (int i=0; i < m_tPose.size(); i++)
	{
		// For HTC Vive, only show the relevant details
		if (m_tPose.at(i).segmentName.find("HTC Vive:") != std::string::npos)
			printViveSegmentData(m_tPose.at(i));
		else
			printSegmentData(m_tPose.at(i));
	}

	for (int i=0; i < m_pointDefinitions.size(); i++)
	{
		// For HTC Vive, only show the relevant details
		if (m_pointDefinitions.at(i).segmentName.find("HTC Vive:") != std::string::npos)
			printVivePointData(m_pointDefinitions.at(i));
		else
			printPointData(m_pointDefinitions.at(i));
	}
}

/*! Print Segment data in a formatted way
*/
void ScaleDatagram::printSegmentData(NullPoseDefinition const& s) const
{
	// String: the name of the segment
	std::cout << "Segment: " << s.segmentName;

	// 3-component vector: the position of the origin of the segment in the null pose
	std::cout << "\tPosition: " << "(";
	std::cout << "x: " << s.pos[0] << ", ";
	std::cout << "y: " << s.pos[1] << ", ";
	std::cout << "z: " << s.pos[2] << ")" << std::endl;
}

/*! Print Vive Segment data in a formatted way
*/
void ScaleDatagram::printViveSegmentData(NullPoseDefinition const& s) const
{
	// String: the name of the segment
	std::cout << "Tracker: " << s.segmentName;

	// 3-component vector: the position of the origin of the segment in the null pose
	std::cout << "\tPosition: " << "(";
	std::cout << "x: " << s.pos[0] << ", ";
	std::cout << "y: " << s.pos[1] << ", ";
	std::cout << "z: " << s.pos[2] << ")" << std::endl;
}

/*! Print Point data in a formatted way
*/
void ScaleDatagram::printPointData(PointDefinition const& p) const
{
	// 2 bytes: the id of the segment containing the point
	std::cout << "Segment ID containing the point: " << p.segmentId << std::endl;

	// 2 bytes: the point id of the point within the segment
	std::cout << "Point ID within the segment: " << p.pointId << std::endl;

	// a string containing the name of the segment
	std::cout << "Segment Name: " << p.segmentName << std::endl;

	// 4 bytes: unsigned integer containing flags describing the point’s characteristics
	std::cout << "Characteristics of point: " << p.characteristicOfPoint << std::endl;

	// 3-compxonent vector: the position of the point relative to the segment origin in the null pose
	std::cout << "Position: " << "(";
	std::cout << "x: " << p.pos[0] << ", ";
	std::cout << "y: " << p.pos[1] << ", ";
	std::cout << "z: " << p.pos[2] << ")" << std::endl;
}


/*! Print Vive Point data in a formatted way
*/
void ScaleDatagram::printVivePointData(PointDefinition const& p) const
{
	// a string containing the name of the segment
	std::cout << "Segment name: " << p.segmentName << std::endl;

	// 3-compxonent vector: the position of the point relative to the segment origin in the null pose
	std::cout << "Position: " << "(";
	std::cout << "x: " << p.pos[0] << ", ";
	std::cout << "y: " << p.pos[1] << ", ";
	std::cout << "z: " << p.pos[2] << ")" << std::endl;
}
