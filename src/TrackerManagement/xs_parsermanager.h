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

#ifndef PARSERMANAGER_H
#define PARSERMANAGER_H

#include "xs_datagram.h"
#include "xs_quaterniondatagram.h"
#include "xs_scaledatagram.h"

class ParserManager
{
public:
	ParserManager();
	~ParserManager();
	struct QuaternionDataWithId { // Avatar ID with QuaternionDatagram
		int avatarId;
		int32_t frameTime;
		std::vector<QuaternionDatagram::Kinematics>* kinematics;
	};
	ParserManager::QuaternionDataWithId* getDatagram(const XsByteArray& data);
	ScaleDatagram::BvhScaleInformation* getScaleDatagram(const XsByteArray& data);

private:
	QuaternionDatagram* datagram;
	ParserManager::QuaternionDataWithId* quaternionDatagramWithId;
	ScaleDatagram* scaleDatagram;
	ScaleDatagram::BvhScaleInformation* scaleData;
	ScaleDatagram::BvhScaleInformation m_scaleDataFiltered;
	std::vector<std::string> endEffectors = { "pTopOfHead", "pLeftTopOfHand", "pRightTopOfHand", "pLeftToe", "pRightToe" };
};

#endif
