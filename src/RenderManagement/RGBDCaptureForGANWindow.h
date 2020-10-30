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
#include <QLabel>
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
	void initiateAzureKinect();
	void startCapture();
	void extractAndSaveFeatureImages();
	void startLandmarkTransmission();
	void startLandmarkTransmissionWithCapturedData();

private:

	enum class ImageType {
		COLOR_IMAGE,
		DEPTH_IMAGE,
		INFRARED_IMAGE,
		FEATURE_IMAGE
	};

	enum class Mode {
		SAVE,
		TRANSMISSION
	};

	boolean currentCaptureIsValid(k4a::capture* capture);

	void saveColorImage();
	void saveDepthImage();
	void saveIrImage();
	void extractJointLandmarks();
	void saveJointLandmarks();

	//capture
	//feature image generation
	//transmission
	//general
	void extractLandmarkFileMetadata();
	void saveCropRegion();
	void saveMinMaxAxisValues();
	void readMinMaxAxisValues();
	void calculateNormalizationRatios();
	void processLandmarkFileData(float startTime, Mode mode);
	bool stringStartsWith(std::string* string, std::string startsWith);
	void splitDataString(std::string fullString, std::string* splittedString);
	void calculateNormalizedLandmarks();
	void mapNormalizedToImageLandmarks();
	void sendImageLandmarksOverNetwork();
	void saveFeatureImage();
	void drawFeaturesToMatrix();
	void continuousLineDrawingBetweenLandmarks(int start, int end);
	void drawSingleLineBetweenLandmarks(int landmarkIndexStart, int landmarkIndexEnd);
	void drawPixelWithDepthTest(int x, int y, cv::Vec4b& color);

	std::string imageFilePath(ImageType type);
	void printCaptureConsoleInfos(float startTime, float& lastEndTime, int currentFrame, int frameCount);
	void printSkipFramesToConsole(float startTime);
	float differenceInSeconds(float startTime, float endTime);

	Ui::RGBDCaptureForGANWindow*ui;

	//config
	ConfigManager* m_configManager;
	NetworkManager* m_networkManager;
	int m_senderId;
	int m_framesToCapture;
	int m_clippingDistance;
	int m_landmarkImageSize;
	int m_landmarkImagePadding;
	std::string m_saveIdPrefix;
	std::string m_dirSavePath;
	cv::Vec4b m_featureLineColors[31];

	//cv mats
	cv::Mat m_colorMat;
	cv::Mat m_depthMat;
	cv::Mat m_depthMat8;
	cv::Mat m_irMat;

	//azure kinect
	k4a::device m_azureKinectSensor;
	k4a_device_configuration_t m_config;
	k4a::calibration m_calibration;
	k4a::transformation m_transformation;
	//azure body tracker
	k4abt::tracker m_tracker;
	k4abt_tracker_configuration_t m_trackerConfig;
	k4abt::frame m_bodyFrame;

	//console
	float m_totalSubtaskTime;
	int m_skippedFrames;

	//capture
	int m_currentCaptureCount;
	k4a::capture* m_currentCapture;

	//saving
	std::thread* m_colorImageSavingThread;
	std::thread* m_depthImageSavingThread;
	std::thread* m_infraredImageSavingThread;
	std::thread* m_featureImageSavingThread;

	//feature map calculation
	std::string m_cropRegionFilePath;
	std::string m_minMaxAxisValuesWriterPath;
	std::string m_minMaxAxisValuesReaderPath;
	std::ofstream m_jointLandmarkWriter;
	std::ifstream m_jointLandmarkReader;
	Landmark m_jointLandmarks[K4ABT_JOINT_COUNT];
	Landmark m_normalizedLanmarks[K4ABT_JOINT_COUNT];
	Landmark m_imageLandmarks[K4ABT_JOINT_COUNT];
	Vector3 m_minPositionAxisValues;
	Vector3 m_maxPositionAxisValues;
	float m_xNormalizationOffset;
	float m_yNormalizationOffset;
	float m_normalizationMaxSpaceSize;
	int m_dataSetCount;
	std::string m_id;
	int m_frameCounter;
	cv::Mat m_featureMatrix;
};
