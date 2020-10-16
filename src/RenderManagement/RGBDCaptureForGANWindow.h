#pragma once
#include "k4a/k4a.h"
#include "k4a/k4a.hpp"
#include "k4abt.h"
#include "k4abt.hpp"

#include <cmath>
#include <opencv2/opencv.hpp>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>

#include "MotionHubUtil/ConfigManager.h"
#include "MotionHubUtil/Vector3.h"
#include "ui_RGBDCaptureForGANWindow.h"

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
	struct Landmark {
		float x;
		float y;
		float depth;
	};

	void extractImageLandmarks(k4abt::frame* bodyFrame);
	void continuousLineDrawingBetweenLandmarks(int start, int end);
	void drawSingleLineBetweenLandmarks(int landmarkIndexStart, int landmarkIndexEnd);

	Ui::RGBDCaptureForGANWindow*ui;

	QLineEdit* m_LineEditIP;

	ConfigManager* m_configManager;

	int m_framesToCapture;
	int m_landmarkImageSize;
	int m_landmarkImagePadding;

	k4a::device m_azureKinectSensor;
	k4a_device_configuration_t m_config;
	k4a::capture* m_kinectCapture;
	k4a::calibration m_calibration;
	k4abt::tracker m_tracker;
	k4abt_tracker_configuration_t m_trackerConfig;
	k4a::image m_kinectColorImage;
	int m_clippingDistance;
	Landmark m_normalizedLanmarks[K4ABT_JOINT_COUNT];
	Landmark m_imageLandmarks[K4ABT_JOINT_COUNT];
	cv::Mat m_landmarkMat;
};
