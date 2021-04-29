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

#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <memory>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include "xs_streamer.h"

enum StreamingProtocol {
	SPPoseEuler = 0x01,
	SPPoseQuaternion = 0x02,
	SPPosePositions = 0x03,
	SPJackProcessSimulate = 0x04,
	SPPoseUnity3D = 0x05,

	// meta data
	//SPMetaScaling = 0x10,		// obsolete protocol
	//SPMetaPropInfo = 0x11,	// obsolete protocol
	SPMetaMoreMeta = 0x12,
	SPMetaScaling = 0x13,

	// additional information
	SPJointAngles = 0x20,
	SPLinearSegmentKinematics = 0x21,
	SPAngularSegmentKinematics = 0x22,
	SPTrackerKinematics = 0x23,
	SPCenterOfMass = 0x24,
	SPTimeCode = 0x25,

	SPUnknown = 0	// different kind of protocol than the above, example: {"type":"transport","value":"record"}
};

class Datagram
{
public:
	Datagram();
	~Datagram();

	bool deserialize(const XsByteArray& arr);
	void setDataCount(uint8_t c);
	void setType(StreamingProtocol proto);
	int32_t messageType() const;
	int32_t sampleCounter() const;
	int32_t frameTime() const;
	uint8_t avatarId() const;
	uint8_t dataCount() const;
	uint8_t datagramCounter() const;

	inline int bodySegmentCount() const { return m_bodySegmentCount; }
	inline int propCount() const { return m_propCount; }
	inline int fingerTrackingSegmentCount() const { return m_fingerTrackingSegmentCount; }

	static int messageType(const XsByteArray& arr);
	std::string decode(StreamingProtocol proto) const;

	void convertFromYupToZup(float *vector) const;

	void printHeader() const;
	virtual void printData() const = 0;

protected:
	virtual void deserializeData(Streamer &inputStreamer) = 0;
	static const float EULERPOSITIONSCALE;

private:
	std::string m_header;
	int32_t m_sampleCounter;
	int32_t m_frameTime;
	uint8_t m_avatarId;
	uint8_t m_dataCount;
	uint8_t m_dgramCounter;

	uint8_t m_bodySegmentCount;
	uint8_t m_propCount;
	uint8_t m_fingerTrackingSegmentCount;

	uint16_t m_dataSize;

	int getDataSize() const;
	void initMap(std::map<int, std::string> &map);
	std::map<int, std::string> m_packetsName;
};

#endif
