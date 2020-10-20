#pragma once
#include "k4a/k4a.h"
#include "k4a/k4a.hpp"
#include "k4abt.h"
#include "k4abt.hpp"

#include <cmath>
#include <time.h>
#include <string>
#include <fstream>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <opencv2/core/mat.hpp>

#include "MotionHubUtil/ConfigManager.h"
#include "MotionHubUtil/Vector3.h"

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
	void extractAndSaveFeatureImages();

private:
	struct Landmark {
		float x;
		float y;
		float depth;
	};

	enum class ImageType {
		COLOR_IMAGE,
		DEPTH_IMAGE,
		INFRARED_IMAGE,
		FEATURE_IMAGE
	};

	boolean captureIsComplete(k4a::capture* capture);

	void saveColorImage();
	void saveDepthImage();
	void saveIrImage();
	void saveJointLandmarks();
	void extractJointLandmarks(k4abt::frame* bodyFrame);

	void extractLandmarkFileMetadata();
	void calculateNormalizationRatios();
	void processLandmarkFileData(float startTime);
	bool stringStartsWith(std::string* string, std::string startsWith);
	void splitDataString(std::string fullString, std::string* splittedString);
	void extractImageLandmarks();
	void calculateNormalizedLandmarks();
	void mapNormalizedToImageLandmarks();
	void saveFeatureImage();
	void drawFeaturesToMatrix();
	void continuousLineDrawingBetweenLandmarks(int start, int end);
	void drawSingleLineBetweenLandmarks(int landmarkIndexStart, int landmarkIndexEnd);

	std::string imageFilePath(ImageType type);
	void printCaptureConsoleInfos(float startTime, float& endTime, float& lastEndTime, 
		int currentFrame, int frameCount);
	float differenceInSeconds(float startTime, float endTime);

	Ui::RGBDCaptureForGANWindow*ui;

	//config
	ConfigManager* m_configManager;
	int m_framesToCapture;
	int m_clippingDistance;
	int m_landmarkImageSize;
	int m_landmarkImagePadding;
	std::string m_dirSavePath;

	//azure kinect
	k4a::device m_azureKinectSensor;
	k4a_device_configuration_t m_config;
	k4a::calibration m_calibration;
	//azure body tracker
	k4abt::tracker m_tracker;
	k4abt_tracker_configuration_t m_trackerConfig;

	//capture
	int m_currentCaptureCount;
	k4a::capture* m_currentCapture;

	//saving
	std::thread* m_colorImageSavingThread;
	std::thread* m_depthImageSavingThread;
	std::thread* m_infraredImageSavingThread;
	std::thread* m_featureImageSavingThread;

	//feature map calculation
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
	int m_frameCounter;
	cv::Mat m_featureMatrix;
};
