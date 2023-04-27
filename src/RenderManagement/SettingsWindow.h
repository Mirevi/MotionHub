#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem>

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>

#include "NetworkManagement/NetworkManager.h"
#include "MotionHubUtil/ConfigManager.h"
#include "MotionHubUtil/RecordingSession.h"

namespace Ui
{
	class SettingsWindow;
}

class SettingsWindow : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsWindow(NetworkManager* networkManager, ConfigManager* configManager, QWidget *parent = nullptr);
	~SettingsWindow();

private slots:
	void accept();
	void setLocalhost();

	void slotRecorderFileDialog();

	void ipAddressTextChanged(const QString& value);

	void slotAddAddress();
	void slotRemoveAddress();

private:
	Ui::SettingsWindow* ui; 

	NetworkManager* m_refNetworkManager;

	//QLineEdit* m_LineEditIP;
	//QLineEdit* m_LineEditIP2;

	ConfigManager* m_configManager;

};

#endif // SETTINGSWINDOW_H
