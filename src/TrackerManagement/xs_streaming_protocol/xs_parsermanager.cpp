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

#include "xs_parsermanager.h"

#include "xs_eulerdatagram.h"
#include "xs_scaledatagram.h"
#include "xs_metadatagram.h"
#include "xs_quaterniondatagram.h"
#include "xs_angularsegmentkinematicsdatagram.h"
#include "xs_centerofmassdatagram.h"
#include "xs_jointanglesdatagram.h"
#include "xs_linearsegmentkinematicsdatagram.h"
#include "xs_positiondatagram.h"
#include "xs_timecodedatagram.h"
#include "xs_trackerkinematicsdatagram.h"

ParserManager::ParserManager()
{
	datagram = new QuaternionDatagram();

	quaternionDatagramWithId = new ParserManager::QuaternionDataWithId();

}

/*! Destructor */
ParserManager::~ParserManager()
{
}



/*! Read single datagram from the incoming stream */
void ParserManager::readDatagram(const XsByteArray& data)
{
	StreamingProtocol type = static_cast<StreamingProtocol>(Datagram::messageType(data));

	//	Datagram *datagram = createDgram(type);
	QuaternionDatagram* datagram = new QuaternionDatagram();

	if (datagram != nullptr)
	{
		datagram->deserialize(data);
		// note that this can cause a lot of console spam
		datagram->printHeader();
		datagram->printData();


	}
	else
	{
		XsString str(data.size(), (const char*)data.data());
		std::cout << "Unhandled datagram: " << str.c_str() << std::endl;

	}

}

/*! Get single QuaternionDatagram from the incoming stream with AvatarId */
ParserManager::QuaternionDataWithId* ParserManager::getDatagram(const XsByteArray& buffer)
{
	StreamingProtocol type = static_cast<StreamingProtocol>(Datagram::messageType(buffer));

	datagram = new QuaternionDatagram();
	quaternionDatagramWithId = new ParserManager::QuaternionDataWithId();


	if (datagram != nullptr)
	{
		datagram->deserialize(buffer);
		quaternionDatagramWithId->avatarId = (int)datagram->avatarId();
		quaternionDatagramWithId->kinematics = datagram->getData();


	}
	else
	{
		XsString str(buffer.size(), (const char*)buffer.data());
		std::cout << "Unhandled datagram: " << str.c_str() << std::endl;

	}


	return quaternionDatagramWithId;


}

