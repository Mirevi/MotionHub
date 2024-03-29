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

#include "xs_udpserver.h"



DWORD WINAPI udpThreadFunc(LPVOID param)
{
	UdpServer* server = (UdpServer*)param;
	server->readMessages();
	return 0;
}

UdpServer::UdpServer(XsString address, uint16_t port)
	: m_started(false)
	, m_stopping(false)

{
	m_port = port;
	m_hostName = address;
	m_quaternionDatagram = new ParserManager::QuaternionDataWithId();
	m_scaleData = new ScaleDatagram::BvhScaleInformation();
	m_parserManager.reset(new ParserManager());
	m_socket.reset(new XsSocket(IpProtocol::IP_UDP, NetworkLayerProtocol::NLP_IPV4));


	XsResultValue res = m_socket->bind(m_hostName, m_port);

	if (res == XRV_OK)
		startThread();
	else
		std::cout << "Failed to bind..." << std::endl;
}

UdpServer::~UdpServer()
{
	stopThread();
}

void UdpServer::readMessages()
{
	XsByteArray buffer;


	while (!m_stopping)
	{

		int rv = m_socket->read(buffer);
		//filter random buffer packages 
		if (buffer.size() > 100) {


			StreamingProtocol type = static_cast<StreamingProtocol>(Datagram::messageType(buffer));


			if (type == SPPoseQuaternion) {
				//assign new datagram if not empty
				if (m_parserManager->getDatagram(buffer)) {
					m_udpLock.lock();

					m_quaternionDatagram = m_parserManager->getDatagram(buffer);

					m_udpLock.unlock();

				}
			}
			else if (type == SPMetaScaling) {
				m_scaleData = m_parserManager->getScaleDatagram(buffer);
			}



		}
		//else {
		//	m_quaternionDatagram = NULL;
		//}

		buffer.clear();
		XsTime::msleep(0);
	}

	m_stopping = false;
	m_started = false;
}


void UdpServer::startThread()
{
	if (m_started)
		return;

	m_started = true;
	m_stopping = false;
	xsStartThread(udpThreadFunc, this, 0);

}

void UdpServer::stopThread()
{
	if (!m_started)
		return;
	m_stopping = true;
	while (m_started)
		XsTime::msleep(10);
}


// get current quaterion datagram with Avatar ID
ParserManager::QuaternionDataWithId* UdpServer::getQuaternionDatagram()
{
	//lock skeleton pool for the case, that getQuaternionDatagram() is called while this method reads from the cache
	m_udpLock.lock();
	//copy cache to local copy to unlock before return
	ParserManager::QuaternionDataWithId* copyQuaData = m_quaternionDatagram;
	m_udpLock.unlock();

	return copyQuaData;
}


// get Scale Datagram
ScaleDatagram::BvhScaleInformation* UdpServer::getScaleDatagram()
{

	m_udpLock.lock();
	//copy cache to local copy to unlock before return
	ScaleDatagram::BvhScaleInformation* copyScaleData = m_scaleData;

	m_udpLock.unlock();

	return copyScaleData;
}
