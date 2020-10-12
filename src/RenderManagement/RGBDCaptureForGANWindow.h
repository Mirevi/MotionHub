#pragma once
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>

#include "MotionHubUtil/ConfigManager.h"

namespace Ui
{
	class RGBDCaptureForGANWindow;
}

class RGBDCaptureForGANWindow : public QDialog
{
	Q_OBJECT

public:
	explicit RGBDCaptureForGANWindow(ConfigManager* configManager, QWidget *parent = nullptr);
	~RGBDCaptureForGANWindow();

private slots:
	//void setBroadcast();

private:
	Ui::RGBDCaptureForGANWindow*ui;

	QLineEdit* m_LineEditIP;

	ConfigManager* m_configManager;

};
