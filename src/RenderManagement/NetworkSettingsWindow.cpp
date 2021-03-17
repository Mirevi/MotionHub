#include "NetworkSettingsWindow.h"
#include "ui_NetworkSettingsWindow.h"

NetworkSettingsWindow::NetworkSettingsWindow(NetworkManager* networkManager, ConfigManager* configManager, QWidget* parent) : QDialog(parent), ui(new Ui::NetworkSettingsWindow)
{

	ui->setupUi(this);

	m_refNetworkManager = networkManager;
	m_configManager = configManager;
	m_LineEditIP = ui->lineEdit_ipAdress;

	m_LineEditIP->setText(QString::fromStdString(m_refNetworkManager->m_ipAddress));

}



NetworkSettingsWindow::~NetworkSettingsWindow()
{

	delete ui;

}



void NetworkSettingsWindow::accept()
{
	
	std::string newAddress = m_LineEditIP->text().toStdString();

	m_refNetworkManager->m_ipAddress = newAddress;
	//m_configManager->writeToConfig("ipAddress", newAddress);
	m_configManager->writeString("ipAddress", newAddress);

	delete this;

}


//void NetworkSettingsWindow::setBroadcast()
//{
//
//	m_refNetworkManager->m_ipAddress = BROADCAST;
//	m_LineEditIP->setText(QString::fromStdString(BROADCAST));
//
//}

void NetworkSettingsWindow::setLocalhost()
{

	m_refNetworkManager->m_ipAddress = LOCALHOST;
	m_LineEditIP->setText(QString::fromStdString(LOCALHOST));


}