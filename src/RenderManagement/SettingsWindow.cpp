#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"


//std::string RecordingSession::RECORD_PATH;

SettingsWindow::SettingsWindow(NetworkManager* networkManager, ConfigManager* configManager, QWidget* parent) : QDialog(parent), ui(new Ui::SettingsWindow)
{

	ui->setupUi(this);

	m_refNetworkManager = networkManager;
	m_configManager = configManager;
	//m_LineEditIP = ui->lineEdit_ipAdress;
	//m_LineEditIP2 = ui->lineEdit_ipAdress2;
	ui->lineEdit_recorder_path->setText(QString(RecordingSession::RECORD_PATH.c_str()));


	//m_LineEditIP->setText(QString::fromStdString(m_refNetworkManager->m_ipAddresses[0]));
	if (m_refNetworkManager->m_ipAddresses.size() > 1)
	{
		//m_LineEditIP2->setText(QString::fromStdString(m_refNetworkManager->m_ipAddresses[1]));

	}


	// Connect TextChanged event to IP address textfield
	//connect(m_LineEditIP, &QLineEdit::textChanged, this, &SettingsWindow::ipAddressTextChanged);
}

SettingsWindow::~SettingsWindow()
{

	delete ui;

}

void SettingsWindow::accept()
{

	// Trim text and convert to std::string
	//std::string newAddress = m_LineEditIP->text().trimmed().toStdString();
	//std::string newAddress2 = m_LineEditIP2->text().trimmed().toStdString();


	//// Apply only valid IP addresses
	//if (m_refNetworkManager->isValidIPAddress(newAddress)) {
	//	m_refNetworkManager->setIPAddress(0, newAddress);
	//	m_configManager->writeString("ipAddress", newAddress);
	//}

	//if (m_refNetworkManager->isValidIPAddress(newAddress2)) {
	//	m_refNetworkManager->setIPAddress(1, newAddress2);
	//	m_configManager->writeString("ipAddress2", newAddress2);
	//}

	std::string pathname(ui->lineEdit_recorder_path->text().toLocal8Bit().data());



	//add slash if nessecary
	if (std::string(1, pathname.at(pathname.length() - 1)) != "/")
	{
		pathname.append("/");
	}

	bool newFolderIsAvailable = true;





	////check if path is valid
	//struct stat info;
	//if (stat(pathname.c_str(), &info) != 0)
	//{
	//	Console::logError("cannot access " + pathname + ". Creating new folder.");

	//	if (std::filesystem::create_directory(pathname))
	//	{
	//		Console::log("successfully created new folder.");
	//	}
	//	else
	//	{
	//		Console::logError("ERROR creating new folder! Path reset to old one.");
	//		newFolderIsAvailable = false;
	//	}

	//}


	newFolderIsAvailable = checkAndFixPath(pathname);




	if (newFolderIsAvailable)
	{
		RecordingSession::RECORD_PATH = pathname;
		m_configManager->writeString("recordPath", RecordingSession::RECORD_PATH);
	}
	else
	{
		Console::logError("SettingsWindow::accept(): path fix not working!");

	}



	delete this;

}

void SettingsWindow::setLocalhost()
{

	m_refNetworkManager->m_ipAddresses[0] = LOCALHOST;
	//m_LineEditIP->setText(QString::fromStdString(LOCALHOST));


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
		//m_LineEditIP->setStyleSheet("");

		// Enable OK button
		ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(false);
	}
	// Invalid IP address
	else {
		// Change border color to red
		//m_LineEditIP->setStyleSheet("border-color: orange");

		// Disable OK button
		ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
	}
}

void SettingsWindow::slotAddAddress()
{
	Console::log("PLUS");

}

void SettingsWindow::slotRemoveAddress()
{
	Console::log("MINUS");

}
