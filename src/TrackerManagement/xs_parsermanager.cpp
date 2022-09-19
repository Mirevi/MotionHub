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




/*! Get single QuaternionDatagram from the incoming stream with AvatarId */
ParserManager::QuaternionDataWithId* ParserManager::getDatagram(const XsByteArray& buffer)
{
	//StreamingProtocol type = static_cast<StreamingProtocol>(Datagram::messageType(buffer));
	//if (type != SPPoseQuaternion) {
	//	return NULL;
	//}
	datagram = new QuaternionDatagram();
	quaternionDatagramWithId = new ParserManager::QuaternionDataWithId();


	if (datagram != nullptr)
	{
		datagram->deserialize(buffer);
		quaternionDatagramWithId->avatarId = (int)datagram->avatarId();
		quaternionDatagramWithId->kinematics = datagram->getData();
		quaternionDatagramWithId->frameTime = datagram->frameTime();

	}
	else
	{
		XsString str(buffer.size(), (const char*)buffer.data());
		std::cout << "Unhandled datagram: " << str.c_str() << std::endl;

	}

	return quaternionDatagramWithId;

}


/*! Get single ScaleDatagram from the incoming stream */
ScaleDatagram::BvhScaleInformation* ParserManager::getScaleDatagram(const XsByteArray& buffer)
{

	// 23 BODYSEGMENTS, 5 endeffectors - MAYBE CONSIDER INCLUDING PROPS
	if (m_scaleDataFiltered.tPose.size() >= 23 && m_scaleDataFiltered.endSites.size() == 5 && m_scaleDataFiltered.avatarId != -1) {
		return &m_scaleDataFiltered;
	}

	//StreamingProtocol type = static_cast<StreamingProtocol>(Datagram::messageType(buffer));
	//if (type != SPMetaScaling) {
	//	return NULL;
	//}

	scaleDatagram = new ScaleDatagram();


	scaleData = new ScaleDatagram::BvhScaleInformation();

	if (scaleDatagram != nullptr)
	{
		scaleDatagram->deserialize(buffer);
		scaleData = scaleDatagram->getScaleData();


	}
	else
	{
		XsString str(buffer.size(), (const char*)buffer.data());
		std::cout << "Unhandled datagram: " << str.c_str() << std::endl;

	}



	if (m_scaleDataFiltered.tPose.size() < scaleData->tPose.size()) {

		for (int i = 0; i < scaleData->tPose.size(); i++) {

			m_scaleDataFiltered.tPose.push_back(scaleData->tPose.at(i));

		}
	}

	if (m_scaleDataFiltered.endSites.size() < 5) {

		for (int i = 0; i < scaleData->endSites.size(); i++) {
			if (std::find(endEffectors.begin(), endEffectors.end(), scaleData->endSites.at(i).segmentName) != endEffectors.end())
			{
				m_scaleDataFiltered.endSites.push_back(scaleData->endSites.at(i));
			}

		}


	}

	m_scaleDataFiltered.avatarId = (int)scaleDatagram->avatarId();

	return &m_scaleDataFiltered;

}

