#ifndef NETWORKSETTINGSWINDOW_H
#define NETWORKSETTINGSWINDOW_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>

#include "NetworkManagement/NetworkManager.h"

namespace Ui
{
	class NetworkSettingsWindow;
}

class NetworkSettingsWindow : public QDialog
{
	Q_OBJECT

public:
	explicit NetworkSettingsWindow(NetworkManager* networkManager, QWidget *parent = nullptr);
	~NetworkSettingsWindow();

private slots:
	void accept();
	//void setBroadcast();
	void setLocalhost();

private:
	Ui::NetworkSettingsWindow *ui;

	NetworkManager* m_refNetworkManager;

	QLineEdit* m_LineEditIP;

};

#endif // NETWORKSETTINGSWINDOW_H
