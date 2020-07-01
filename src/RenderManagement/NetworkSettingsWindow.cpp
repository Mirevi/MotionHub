#include "NetworkSettingsWindow.h"
#include "ui_NetworkSettingsWindow.h"

NetworkSettingsWindow::NetworkSettingsWindow(NetworkManager* networkManager, QWidget *parent) : QDialog(parent), ui(new Ui::NetworkSettingsWindow)
{

	ui->setupUi(this);

	m_refNetworkManager = networkManager;
	m_LineEditIP = ui->lineEdit_ipAdress;

	m_LineEditIP->setText(QString::fromStdString(m_refNetworkManager->m_ipAddress));

}



NetworkSettingsWindow::~NetworkSettingsWindow()
{

	delete ui;

}



void NetworkSettingsWindow::accept()
{

	m_refNetworkManager->m_ipAddress = m_LineEditIP->text().toStdString();

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