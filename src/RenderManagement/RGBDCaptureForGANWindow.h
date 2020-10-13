#pragma once
#include "k4a/k4a.h"
#include "k4a/k4a.hpp"
#include "k4abt.h"

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
	void initiateAzureKinect();
	void startCapture();

private:
	Ui::RGBDCaptureForGANWindow*ui;

	QLineEdit* m_LineEditIP;

	ConfigManager* m_configManager;

	int m_framesToCapture;


	k4a::device m_azureKinectSensor;
	k4a_device_configuration_t m_config;
	k4a::capture* m_kinectCapture;
	k4a::image m_kinectColorImage;
	int m_clippingDistance;
};
