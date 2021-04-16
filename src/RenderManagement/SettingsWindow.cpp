#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"


//std::string RecordingSession::RECORD_PATH;

SettingsWindow::SettingsWindow(NetworkManager* networkManager, ConfigManager* configManager, QWidget* parent) : QDialog(parent), ui(new Ui::SettingsWindow)
{

	ui->setupUi(this);

	m_refNetworkManager = networkManager;
	m_configManager = configManager;
	m_LineEditIP = ui->lineEdit_ipAdress;
	ui->lineEdit_recorder_path->setText(QString(RecordingSession::RECORD_PATH.c_str()));


	m_LineEditIP->setText(QString::fromStdString(m_refNetworkManager->m_ipAddress));

	// Connect TextChanged event to IP address textfield
	connect(m_LineEditIP, &QLineEdit::textChanged, this, &SettingsWindow::ipAddressTextChanged);
}



SettingsWindow::~SettingsWindow()
{

	delete ui;

}



void SettingsWindow::accept()
{

	// Trim text and convert to std::string
	std::string newAddress = m_LineEditIP->text().trimmed().toStdString();

	// Apply only valid IP addresses
	if (m_refNetworkManager->isValidIPAddress(newAddress)) {
		m_refNetworkManager->setIPAddress(newAddress);
		m_configManager->writeString("ipAddress", newAddress);
	}

	RecordingSession::RECORD_PATH = std::string(ui->lineEdit_recorder_path->text().toLocal8Bit().data());
	m_configManager->writeString("recordPath", RecordingSession::RECORD_PATH);


	delete this;

}




void SettingsWindow::setLocalhost()
{

	m_refNetworkManager->m_ipAddress = LOCALHOST;
	m_LineEditIP->setText(QString::fromStdString(LOCALHOST));


}

void SettingsWindow::slotRecorderFileDialog()
{

	QString currFilePath = QFileDialog::getExistingDirectory(this, "choose path for recorded files.", QString(RecordingSession::RECORD_PATH.c_str()));

	if (currFilePath != "")
	{
		ui->lineEdit_recorder_path->setText(currFilePath);
	}
	

}

void SettingsWindow::ipAddressTextChanged(const QString& text) {

	// Trim text and convert to std::string
	std::string newAddress = text.trimmed().toStdString();

	// Is text an IP address valid?
	if (m_refNetworkManager->isValidIPAddress(newAddress)) {

		// Remove border color
		m_LineEditIP->setStyleSheet("");

		// Enable OK button
		ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
	}
	// Invalid IP address
	else {
		// Change border color to red
		m_LineEditIP->setStyleSheet("border-color: orange");

		// Disable OK button
		ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
	}
}
