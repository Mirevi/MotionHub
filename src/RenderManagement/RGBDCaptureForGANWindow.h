#pragma once
#include "k4a/k4a.h"
#include "k4a/k4a.hpp"
#include "k4abt.h"
#include "k4abt.hpp"

#include <cmath>
#include <time.h>
#include <string>
#include <limits>
#include <fstream>
#include <filesystem>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <opencv2/core/mat.hpp>

#include "MotionHubUtil/ConfigManager.h"
#include "MotionHubUtil/Vector3.h"
#include "MotionHubUtil/Landmark.h"

#include "NetworkManagement/NetworkManager.h"

namespace Ui
{
	class RGBDCaptureForGANWindow;
}

class RGBDCaptureForGANWindow : public QDialog
{
	Q_OBJECT

public:
	explicit RGBDCaptureForGANWindow(ConfigManager* configManager, NetworkManager* networkManager, QWidget *parent = nullptr);
	~RGBDCaptureForGANWindow();

private slots:
	void reject();
	void onGuiValueChanged();
	void onInitializeAzureKinectTriggered();
	void onStopTaskAndCloseDevice();
	void onStartCaptureTriggered();
	void onStartLandmarkTransissionTriggered();
	void onStartLandmarkTransissionWithCapturedDataTriggered();

private:

	enum class State {
		UNINITIALIZED,
		IS_INITIALIZING,
		INITIALIZED,
		CAPTURING,
		TRANSMITTING,
		TRANSMITTING_FILE,
		ENDING,
	};

	enum class ImageType {
		COLOR_IMAGE,
		DEPTH_IMAGE,
		INFRARED_IMAGE,
		FEATURE_IMAGE
	};

	void updateGui();
	void initializeFeatureLineColor();

	//threads
	void taskThreadMethod();
	void initiateAzureKinect();
	void updateUiLineEditBoundaries();

	//preview
	void showAzurePreview();

	//capture
	void startCapture();
	void prepareSaveDirLocation();
	boolean currentCaptureIsValid(k4a::capture* capture);
	void processColorImage();
	void processDepthImage();
	void estimateDepth8Mat();
	void processInfraredImage();
	void processFeatureImage();
	void cropAndResizeMat(std::shared_ptr<cv::Mat>* mat);
	void saveProcessedCapturedData();
	void saveColorImage();
	void saveDepthImage();
	void saveIrImage();
	void saveFeatureImage();

	//feature image generation 
	void extractJointLandmarks();
	void saveColorImageLandmarks();
	void drawFeaturesToMatrix();
	void continuousLineDrawingBetweenLandmarks(int start, int end);
	void drawSingleLineBetweenLandmarks(int landmarkIndexStart, int landmarkIndexEnd);
	void drawPixelWithDepthTest(int x, int y, cv::Vec4b& color);

	//transmission
	void startLandmarkTransmission();
	void startLandmarkTransmissionWithCapturedData();
	void countFramesInLandmarkFile();
	void processImageLandmarkFileData();
	bool stringStartsWith(std::string* string, std::string startsWith);
	void splitDataString(std::string fullString, std::string* splittedString);
	void sendFeatureImageLandmarksOverNetwork();

	//general
	boolean convertJointLandmarksToColorImageLandmarks();
	void mapColorLandmarksToFeatureImageLandmarks();
	std::string imageFilePath(ImageType type);

	//console + show message in gui
	void printTaskConsoleInfos(int currentFrame = -1, int frameCount = -1);
	void printSkipFramesToConsole();
	void printAndShowMessage(std::string message);
	float differenceInSeconds(float startTime, float endTime);

	Ui::RGBDCaptureForGANWindow*ui;

	//config
	ConfigManager* m_configManager;
	NetworkManager* m_networkManager;
	int m_senderId;
	int m_transmissionId;
	int m_framesToCapture;
	int m_clippingNear;
	int m_clippingFar;
	int m_unclippedAreaSize;
	int m_depthCaptureBitSize;
	cv::Size m_imageSize;
	cv::Size m_featureImageSize;
	cv::Rect m_cropRegion;
	std::string m_saveIdPrefix;
	std::string m_dataDirPath;
	std::string m_saveDirPath;
	cv::Vec4b m_featureLineColors[31];
	bool m_showColorImagePreview;
	bool m_showDepthImagePreview;
	bool m_showFeatureImagePreview;

	//threads
	std::shared_ptr<std::thread> m_taskThread;
	State m_state;
	bool m_closeAzureDeviceAfterTask;

	//azure kinect
	k4a::device m_azureKinectSensor;
	k4a_device_configuration_t m_config;
	k4a::calibration m_calibration;
	k4a::transformation m_transformation;
	cv::Size m_colorResolution;
	//azure body tracker
	k4abt::tracker m_tracker;
	k4abt_tracker_configuration_t m_trackerConfig;
	k4abt::frame m_bodyFrame;

	//cv mats
	std::shared_ptr<cv::Mat> m_colorMat;
	std::shared_ptr<cv::Mat> m_depthMat16;
	std::shared_ptr<cv::Mat> m_depthMat8;
	std::shared_ptr<cv::Mat> m_irMat;
	std::shared_ptr<cv::Mat> m_featureMatrix;

	//capture
	k4a::capture* m_currentCapture;
	k4a::image m_transformedDepthImage;
	std::pair<k4a::image, k4a::image> m_depthAndIrImagePair;
	std::ofstream m_colorImageLandmarkWriter;
	std::ifstream m_colorImageLandmarkReader;

	//transmission
	int m_landmarkFileFrameCount;
	std::string m_id;

	//general
	Landmark m_jointLandmarks[K4ABT_JOINT_COUNT];
	Landmark m_colorImageLandmarks[K4ABT_JOINT_COUNT];
	Landmark m_featureImageLandmarks[K4ABT_JOINT_COUNT];
	int m_frameCounter;

	//console
	float m_startTime;
	float m_endTime;
	float m_subtaskStartTime;
	float m_subtaskEndTime;
	float m_totalSubtaskTime;
	int m_skippedFrames;
};
