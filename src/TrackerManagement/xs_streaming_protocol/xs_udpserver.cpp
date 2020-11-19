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

UdpServer::UdpServer(XsString address, uint16_t port, std::function<void(const std::vector<QuaternionDatagram::Kinematics>& data)> func)
	: m_started(false)
	, m_stopping(false)

{
	m_port = port;
	m_hostName = address;
	m_func = func;
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

	std::cout << "Das ist der erste Build-Versuch: \nWaiting to receive packets from the client on port " << m_port << " ..." << std::endl << std::endl;

	while (!m_stopping)
	{
		//std::cout << ".";
		int rv = m_socket->read(buffer);
		if (buffer.size() > 0) {

			if (!m_parserManager->getDatagram(buffer).empty()) {
				m_func(m_parserManager->getDatagram(buffer));
				//m_func();
			}
			//m_parserManager->readDatagram(buffer);
		}



		buffer.clear();
		XsTime::msleep(1);
	}

	std::cout << "Stopping receiving packets..." << std::endl << std::endl;

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
