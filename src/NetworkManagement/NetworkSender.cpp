#include "NetworkSender.h"

NetworkSender::NetworkSender()
{

}

NetworkSender::NetworkSender(std::string address, int port) : m_address(address), m_port(port)
{

}

bool NetworkSender::isActive()
{

	return m_active;

}

void NetworkSender::setActive(bool active)
{

	m_active = active;

}