#include "NetworkSettingsWindow.h"
#include "ui_NetworkSettingsWindow.h"


//std::string RecordingSession::RECORD_PATH;

NetworkSettingsWindow::NetworkSettingsWindow(NetworkManager* networkManager, ConfigManager* configManager, QWidget* parent) : QDialog(parent), ui(new Ui::NetworkSettingsWindow)
{

	ui->setupUi(this);

	m_refNetworkManager = networkManager;
	m_configManager = configManager;
	m_LineEditIP = ui->lineEdit_ipAdress;
	ui->lineEdit_recorder_path->setText(QString(RecordingSession::RECORD_PATH.c_str()));


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
	m_configManager->writeToConfig("ipAddress", newAddress);
	RecordingSession::RECORD_PATH = std::string(ui->lineEdit_recorder_path->text().toLocal8Bit().data());

	Console::log("NetworkSettingsWindow::accept(): " + RecordingSession::RECORD_PATH);

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

void NetworkSettingsWindow::slotRecorderFileDialog()
{

	QString currFilePath = QFileDialog::getExistingDirectory(this, "choose path for recorded files.", QString(RecordingSession::RECORD_PATH.c_str()));

	if (currFilePath != "")
	{
		ui->lineEdit_recorder_path->setText(currFilePath);
	}
	

}