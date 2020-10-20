
#include "RGBDCaptureForGANWindow.h"
#include <opencv2/opencv.hpp>
#include "ui_RGBDCaptureForGANWindow.h"


RGBDCaptureForGANWindow::RGBDCaptureForGANWindow(ConfigManager* configManager, QWidget* parent) : QDialog(parent), ui(new Ui::RGBDCaptureForGANWindow)
{
	ui->setupUi(this);

	m_configManager = configManager; 

	m_framesToCapture = 400;
	m_azureKinectSensor = NULL;
	m_config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	m_clippingDistance = 2048; //behind this value (millimeters) the points will be rejected

	m_landmarkImageSize = 256;
	m_landmarkImagePadding = 20;

	m_dirSavePath = "data\\RGBD-GAN\\";

}

RGBDCaptureForGANWindow::~RGBDCaptureForGANWindow()
{

	delete ui;

}

void RGBDCaptureForGANWindow::initiateAzureKinect()
{

	//TODO: error handling
	m_azureKinectSensor = k4a::device::open(K4A_DEVICE_DEFAULT);

	m_config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
	m_config.color_resolution = K4A_COLOR_RESOLUTION_720P;
	m_config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
	m_config.camera_fps = K4A_FRAMES_PER_SECOND_30;
	m_config.synchronized_images_only = true; // ensures that depth and color images are both available in the capture
	m_azureKinectSensor.start_cameras(&m_config);
	m_calibration = m_azureKinectSensor.get_calibration(m_config.depth_mode, m_config.color_resolution);
	std::cout << "Initiate Azure Kinect done" << std::endl;

	//Initiate tracker
	m_trackerConfig = K4ABT_TRACKER_CONFIG_DEFAULT;
	m_tracker = m_tracker.create(m_calibration, m_trackerConfig);
	std::cout << "Initiate Body Tracking Component done" << std::endl;

}

void RGBDCaptureForGANWindow::startCapture()
{
	std::cout << "Start RGBD Capture for GAN" << std::endl;

	float startTime = clock();
	float lastCaptureEnd = startTime;
	float captureEnd;

	//initiation
	m_jointLandmarkWriter.open(m_dirSavePath + "jointLandmarks.txt");
	m_currentCapture = new k4a::capture();

	//process all captures and save on disk
	for (int i = 1; i < m_framesToCapture + 1; i++)
	{
		m_currentCaptureCount = i;
		m_azureKinectSensor.get_capture(m_currentCapture);

		if (captureIsComplete(m_currentCapture)) {

			saveColorImage();
			saveDepthImage();
			saveIrImage();
			saveJointLandmarks();

			printCaptureConsoleInfos(startTime, captureEnd, lastCaptureEnd, m_currentCaptureCount, m_framesToCapture);
		}
	}

	m_jointLandmarkWriter.close();
	m_azureKinectSensor.close();

	float endTime = clock();
	std::cout << "End RGBD Capture for GAN after " << differenceInSeconds(startTime, endTime) << " seconds." << std::endl;
}

boolean RGBDCaptureForGANWindow::captureIsComplete(k4a::capture* capture)
{
	return capture->get_color_image()
		&& capture->get_depth_image()
		&& capture->get_ir_image();
}

void RGBDCaptureForGANWindow::saveColorImage()
{
	k4a::image color_image = m_currentCapture->get_color_image();

	//Create a color image buffer
	uint8_t* color_buffer = color_image.get_buffer();
	size_t color_buffer_size = color_image.get_size();

	//Need dimension in order to allocate memory
	int colorWidth = color_image.get_width_pixels();
	int colorHeight = color_image.get_height_pixels();

	//convert to cv:Mat
	cv::Mat colorMat(colorHeight, colorWidth, CV_8UC4, (void*)color_buffer, cv::Mat::AUTO_STEP);

	//save
	std::string colorFileName = imageFilePath(ImageType::COLOR_IMAGE);
	m_colorImageSavingThread = new std::thread(&cv::imwrite, colorFileName, colorMat, std::vector<int>());
	m_colorImageSavingThread->join();
	delete m_colorImageSavingThread;
}

void RGBDCaptureForGANWindow::saveDepthImage()
{
	k4a::image depthImage = m_currentCapture->get_depth_image();
	k4a::transformation transformation = k4a::transformation(m_calibration);

	// The YUY2 image format is the same stride as the 16-bit depth image, so we can modify it in-place.
	uint8_t* depth_buffer = depthImage.get_buffer();
	size_t depth_buffer_size = depthImage.get_size();

	//Depth Clipping
	for (size_t i = 0; i < depth_buffer_size; i += 2)
	{
		//If the distance is farther than clip (format is YUY2 -> Two byte per pixel results in 16bit and 65535mm depth resolution)
		uint16_t depth = (uint16_t)(depth_buffer[i + 1] << 8 | depth_buffer[i]);
		if (depth > m_clippingDistance)
		{
			depth_buffer[i] = 0;
			depth_buffer[i + 1] = 0;
		}
	}

	//Adpat the depth image to the size, aspect ratio and intrinsic parameters of the color cam
	k4a::image transformedDepthImage = k4a::image::create(K4A_IMAGE_FORMAT_DEPTH16,
		m_calibration.color_camera_calibration.resolution_width,
		m_calibration.color_camera_calibration.resolution_height,
		m_calibration.color_camera_calibration.resolution_width *
		static_cast<int>(sizeof(uint16_t)));
	transformation.depth_image_to_color_camera(depthImage, &transformedDepthImage);
	uint8_t* transformed_depth_buffer = transformedDepthImage.get_buffer();

	//Get dimensions of the "new" transformed depth image and get the dimension in order to allocate memory
	int transformedDepthWidth = transformedDepthImage.get_width_pixels();
	int transformedDepthHeight = transformedDepthImage.get_height_pixels();

	cv::Mat depthMat(transformedDepthHeight, transformedDepthWidth, CV_16UC1, (void*)transformed_depth_buffer, cv::Mat::AUTO_STEP);

	//Create a file name
	std::string depthFileName = imageFilePath(ImageType::DEPTH_IMAGE);
	m_depthImageSavingThread = new std::thread(&cv::imwrite, depthFileName, depthMat, std::vector<int>());
	m_depthImageSavingThread->join();
	delete m_depthImageSavingThread;
}

void RGBDCaptureForGANWindow::saveIrImage()
{
	k4a::image ir_image = m_currentCapture->get_ir_image();
	k4a::image depthImage = m_currentCapture->get_depth_image();
	k4a::transformation transformation = k4a::transformation(m_calibration);

	//Create IR image buffer
	uint8_t* ir_buffer = ir_image.get_buffer();
	size_t ir_buffer_size = ir_image.get_size();

	int cirImageWidth = ir_image.get_width_pixels();
	int cirImageHeight = ir_image.get_height_pixels();
	int cirImageStrideBytes = ir_image.get_stride_bytes();
	uint8_t* cirImageBuffer = ir_image.get_buffer();
	k4a::image customIrImage = k4a::image::create_from_buffer(K4A_IMAGE_FORMAT_CUSTOM16,
		cirImageWidth,
		cirImageHeight,
		cirImageStrideBytes,
		cirImageBuffer,
		cirImageHeight * cirImageStrideBytes,
		NULL,
		NULL);

	std::pair<k4a::image, k4a::image> images = transformation.depth_image_to_color_camera_custom(depthImage,
		customIrImage,
		K4A_TRANSFORMATION_INTERPOLATION_TYPE_NEAREST,
		0);

	//Need dimension in order to allocate memory
	int irImageWidth = images.second.get_width_pixels();
	int irImageHeight = images.second.get_height_pixels();

	//convert to cv:Mat
	cv::Mat irMat(irImageHeight, irImageWidth, CV_16UC1, (void*)images.second.get_buffer(), cv::Mat::AUTO_STEP);

	//save
	std::string irFileName = imageFilePath(ImageType::INFRARED_IMAGE);
	m_infraredImageSavingThread = new std::thread(&cv::imwrite, irFileName, irMat, std::vector<int>());
	m_infraredImageSavingThread->join();
	delete m_infraredImageSavingThread;
}

void RGBDCaptureForGANWindow::saveJointLandmarks()
{
	m_tracker.enqueue_capture(*m_currentCapture, std::chrono::milliseconds(-1));
	k4abt::frame bodyFrame = m_tracker.pop_result(std::chrono::milliseconds(-1));

	extractJointLandmarks(&bodyFrame);

	//save to text file
	m_jointLandmarkWriter << "ID: " << m_currentCaptureCount << std::endl;

	m_jointLandmarkWriter << "X: ";
	for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
		m_jointLandmarkWriter << m_jointLandmarks[i].x << ",";
	}
	m_jointLandmarkWriter << std::endl;

	m_jointLandmarkWriter << "Y: ";
	for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
		m_jointLandmarkWriter << m_jointLandmarks[i].y << ",";
	}
	m_jointLandmarkWriter << std::endl;

	m_jointLandmarkWriter << "Depth: ";
	for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
		m_jointLandmarkWriter << m_jointLandmarks[i].depth << ",";
	}
	m_jointLandmarkWriter << std::endl;

	m_jointLandmarkWriter << "END" << std::endl;
}

void RGBDCaptureForGANWindow::extractJointLandmarks(k4abt::frame* bodyFrame)
{
	uint32_t bodyCount = bodyFrame->get_num_bodies();

	//only one body is allowed
	if (bodyCount == 1) {
		k4abt_skeleton_t skeleton = bodyFrame->get_body_skeleton(0);
		k4abt_joint_t* joints = skeleton.joints;

		//first joint position
		k4a_float3_t jointPosition;

		for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
			jointPosition = joints[i].position;

			Landmark current;
			current.x = jointPosition.xyz.x;
			current.y = jointPosition.xyz.y;
			current.depth = jointPosition.xyz.z;

			m_jointLandmarks[i] = current;
		}
	}
}

void RGBDCaptureForGANWindow::extractAndSaveFeatureImages()
{
	std::cout << "Start Extract And Save Feature Image" << std::endl;

	//time for console logging
	float startTime = clock();

	m_jointLandmarkReader.open(m_dirSavePath + "jointLandmarks.txt");
	std::string line;
	std::string splitted[K4ABT_JOINT_COUNT];

	//get min and max boundaries for normalization
	extractLandmarkFileMetadata();

	//reset reader
	m_jointLandmarkReader.clear();
	m_jointLandmarkReader.seekg(0);

	//calculate necessary values for normalization
	calculateNormalizationRatios();

	//process data from file
	processLandmarkFileData(startTime);

	m_jointLandmarkReader.close();

	float endTime = clock();
	std::cout << "Start Extract And Save Feature Image after " << differenceInSeconds(startTime, endTime) << " seconds." << std::endl;
}

void RGBDCaptureForGANWindow::extractLandmarkFileMetadata()
{
	m_dataSetCount = 0;

	std::string line;
	std::string splitted[K4ABT_JOINT_COUNT];

	while (std::getline(m_jointLandmarkReader, line)) {
		//x
		if (stringStartsWith(&line, "X: ")) {
			splitDataString(line.substr(3), splitted);

			for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
				float value = std::stof(splitted[i]);

				if (value < m_minPositionAxisValues.m_xyz.x)
					m_minPositionAxisValues.m_xyz.x = value;
				else if (value > m_maxPositionAxisValues.m_xyz.x)
					m_maxPositionAxisValues.m_xyz.x = value;
			}
		}
		//y
		else if (stringStartsWith(&line, "Y: ")) {
			splitDataString(line.substr(3), splitted);

			for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
				float value = std::stof(splitted[i]);

				if (value < m_minPositionAxisValues.m_xyz.y)
					m_minPositionAxisValues.m_xyz.y = value;
				else if (value > m_maxPositionAxisValues.m_xyz.y)
					m_maxPositionAxisValues.m_xyz.y = value;
			}
		}
		//depth
		else if (stringStartsWith(&line, "Depth: ")) {
			splitDataString(line.substr(7), splitted);

			for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
				float value = std::stof(splitted[i]);

				if (value < m_minPositionAxisValues.m_xyz.z)
					m_minPositionAxisValues.m_xyz.z = value;
				else if (value > m_maxPositionAxisValues.m_xyz.z)
					m_maxPositionAxisValues.m_xyz.z = value;
			}
		}
		else if (stringStartsWith(&line, "END")) {
			m_dataSetCount++;
		}
	}
}

void RGBDCaptureForGANWindow::calculateNormalizationRatios()
{
	Vector3 jointPositionSpaceSize = m_maxPositionAxisValues - m_minPositionAxisValues;

	//values for normalization without loosing the proportions between x and y

	m_xNormalizationOffset = 0;
	m_yNormalizationOffset = 0;

	//check which axis space is bigger than the other and which has to be filled to obtain the proportions
	if (jointPositionSpaceSize.m_xyz.x > jointPositionSpaceSize.m_xyz.y) {
		m_normalizationMaxSpaceSize = jointPositionSpaceSize.m_xyz.x;
		//x space is bigger than y space so the missing space of y has to be filled (* 0.5 for center alignment)
		m_yNormalizationOffset = abs(jointPositionSpaceSize.m_xyz.x - jointPositionSpaceSize.m_xyz.y) * 0.5f;
	}
	else
	{
		m_normalizationMaxSpaceSize = jointPositionSpaceSize.m_xyz.y;
		//y space is bigger or the same as x space so the eventually missing space of x has to be filled * 0.5 for center alignment)
		m_xNormalizationOffset = abs(jointPositionSpaceSize.m_xyz.x - jointPositionSpaceSize.m_xyz.y) * 0.5f;
	}
}

void RGBDCaptureForGANWindow::processLandmarkFileData(float startTime)
{
	float lastSaveEnd = clock();
	float saveEnd;
	m_frameCounter = 1;

	std::string line;
	std::string splitted[K4ABT_JOINT_COUNT];

	while (std::getline(m_jointLandmarkReader, line)) {
		if (stringStartsWith(&line, "X: ")) {
			splitDataString(line.substr(3), splitted);

			for (int i = 0; i < K4ABT_JOINT_COUNT; i++)
				m_jointLandmarks[i].x = std::stof(splitted[i]);
		}
		else if (stringStartsWith(&line, "Y: ")) {
			splitDataString(line.substr(3), splitted);

			for (int i = 0; i < K4ABT_JOINT_COUNT; i++)
				m_jointLandmarks[i].y = std::stof(splitted[i]);
		}
		else if (stringStartsWith(&line, "Depth: ")) {
			splitDataString(line.substr(7), splitted);

			for (int i = 0; i < K4ABT_JOINT_COUNT; i++)
				m_jointLandmarks[i].depth = std::stof(splitted[i]);
		}
		else if (stringStartsWith(&line, "END")) {
			//process current Landmarks
			extractImageLandmarks();
			saveFeatureImage();

			printCaptureConsoleInfos(startTime, saveEnd, lastSaveEnd, m_frameCounter, m_dataSetCount);
			m_frameCounter++;
		}
	}
}

bool RGBDCaptureForGANWindow::stringStartsWith(std::string* s, std::string startsWith)
{
	if (startsWith.length() > s->length())
		return false;

	bool isStartingWith = true;

	for (int i = 0; i < startsWith.length(); i++) {
		if (s->at(i) != startsWith.at(i))
			isStartingWith = false;
	}

	return isStartingWith;
}

void RGBDCaptureForGANWindow::splitDataString(std::string fullString, std::string* splittedString)
{
	int splitterCounter = 0;
	int lastDelimiterIndex = 0;
	for (int i = 0; i < fullString.length(); i++) {
		if (fullString.at(i) == ',') {
			int length = i - lastDelimiterIndex;
			splittedString[splitterCounter] = fullString.substr(lastDelimiterIndex, length);

			lastDelimiterIndex = i + 1;
			splitterCounter++;
		}
	}
}

void RGBDCaptureForGANWindow::extractImageLandmarks() {

	calculateNormalizedLandmarks();
	mapNormalizedToImageLandmarks();

}

void RGBDCaptureForGANWindow::calculateNormalizedLandmarks()
{
	//normalize joint positions without loosing the proportion between x and y to get normalized landmarks 

	for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
		Landmark* currentJointLandmark = &m_jointLandmarks[i];

		m_normalizedLanmarks[i].x =
			(currentJointLandmark->x - m_minPositionAxisValues.m_xyz.x + m_xNormalizationOffset) / m_normalizationMaxSpaceSize;
		m_normalizedLanmarks[i].y =
			(currentJointLandmark->y - m_minPositionAxisValues.m_xyz.y + m_yNormalizationOffset) / m_normalizationMaxSpaceSize;
		m_normalizedLanmarks[i].depth =
			(currentJointLandmark->depth - m_minPositionAxisValues.m_xyz.z)
			/ (m_maxPositionAxisValues.m_xyz.z - m_minPositionAxisValues.m_xyz.z);
	}
}

void RGBDCaptureForGANWindow::mapNormalizedToImageLandmarks()
{
	//Calculate image landmarks

	float landmarkSpaceOnImage = m_landmarkImageSize - 2 * m_landmarkImagePadding;

	for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
		Landmark current;
		current.x = m_normalizedLanmarks[i].x * landmarkSpaceOnImage + m_landmarkImagePadding;
		current.y = m_normalizedLanmarks[i].y * landmarkSpaceOnImage + m_landmarkImagePadding;
		current.depth = m_normalizedLanmarks[i].depth * 255;

		m_imageLandmarks[i] = current;
	}
}

void RGBDCaptureForGANWindow::saveFeatureImage()
{
	//create cv matrix and set black

	m_featureMatrix = cv::Mat::zeros(m_landmarkImageSize, m_landmarkImageSize, CV_8UC3);

	//color landmark pixels

	drawFeaturesToMatrix();

	//save
	std::string landmarkFileName = imageFilePath(ImageType::FEATURE_IMAGE);
	m_featureImageSavingThread = new std::thread(&cv::imwrite, landmarkFileName, m_featureMatrix, std::vector<int>());
	m_featureImageSavingThread->join();
	delete m_featureImageSavingThread;
}

void RGBDCaptureForGANWindow::drawFeaturesToMatrix()
{
	//draw lines, see https://docs.microsoft.com/en-us/azure/kinect-dk/body-joints for landmark index types		
	continuousLineDrawingBetweenLandmarks(0, 3);
	drawSingleLineBetweenLandmarks(3, 26);
	continuousLineDrawingBetweenLandmarks(26, 29);
	drawSingleLineBetweenLandmarks(27, 30);
	drawSingleLineBetweenLandmarks(30, 31);

	drawSingleLineBetweenLandmarks(2, 4);
	continuousLineDrawingBetweenLandmarks(4, 9);
	drawSingleLineBetweenLandmarks(7, 10);

	drawSingleLineBetweenLandmarks(2, 11);
	continuousLineDrawingBetweenLandmarks(11, 16);
	drawSingleLineBetweenLandmarks(14, 17);

	drawSingleLineBetweenLandmarks(0, 18);
	continuousLineDrawingBetweenLandmarks(18, 21);

	drawSingleLineBetweenLandmarks(0, 22);
	continuousLineDrawingBetweenLandmarks(22, 25);

	for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
		uchar x1 = static_cast<int>(m_imageLandmarks[i].x);
		uchar y1 = static_cast<int>(m_imageLandmarks[i].y);

		circle(m_featureMatrix, cv::Point(x1, y1), 1, cv::Scalar(0, 255, 0), -1);
	}
}

void RGBDCaptureForGANWindow::continuousLineDrawingBetweenLandmarks(int start, int end)
{
	for (int i = start; i < end; i++)
	{
		drawSingleLineBetweenLandmarks(i, i + 1);
	}
}

void RGBDCaptureForGANWindow::drawSingleLineBetweenLandmarks(int landmarkIndexStart, int landmarkIndexEnd)
{
	uchar x1 = static_cast<int>(m_imageLandmarks[landmarkIndexStart].x);
	uchar y1 = static_cast<int>(m_imageLandmarks[landmarkIndexStart].y);
	uchar depth1 = static_cast<int>(m_imageLandmarks[landmarkIndexStart].depth);

	uchar x2 = static_cast<int>(m_imageLandmarks[landmarkIndexEnd].x);
	uchar y2 = static_cast<int>(m_imageLandmarks[landmarkIndexEnd].y);
	uchar depth2 = static_cast<int>(m_imageLandmarks[landmarkIndexEnd].depth);

	cv::LineIterator lineIterator(m_featureMatrix, cv::Point(x1, y1), cv::Point(x2, y2));

	for (int i = 0; i < lineIterator.count; i++, lineIterator++) {
		float alpha = float(i) / lineIterator.count;

		//TODO check for depth at this point + thicker line

		//cv::circle(m_featureMatrix, lineIterator.pos(), 1, cv::Scalar(255, 255, 255 * alpha));
		cv::Vec3b color = cv::Vec3b(255, 255, 255 * alpha);
		m_featureMatrix.at<cv::Vec3b>(lineIterator.pos()) = color;
	}

	//line(m_featureMatrix, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 255, 255), 4);
}

std::string RGBDCaptureForGANWindow::imageFilePath(ImageType type)
{
	std::string filename;
	filename.append(m_dirSavePath);
	switch (type) {
	case ImageType::COLOR_IMAGE:
		filename.append("colorImage_");
		break;
	case ImageType::DEPTH_IMAGE:
		filename.append("depthImage_");
		break;
	case ImageType::INFRARED_IMAGE:
		filename.append("infraredImage_");
		break;
	case ImageType::FEATURE_IMAGE:
		filename.append("featureImage_");
		break;
	}

	if (type == ImageType::FEATURE_IMAGE)
		filename.append(std::to_string(m_frameCounter));
	else
		filename.append(std::to_string(m_currentCaptureCount));
	filename.append(".png");

	return filename;
}

void RGBDCaptureForGANWindow::printCaptureConsoleInfos(float startTime, float& endTime, float& lastEndTime, int currentFrame, int frameCount)
{
	endTime = clock();
	float totalTime = differenceInSeconds(startTime, endTime);
	float estimatedTimeLeft = (totalTime / currentFrame) * (frameCount - currentFrame);
	std::cout << "Frame " << currentFrame << "/" << m_framesToCapture << " needed ";
	std::cout << differenceInSeconds(lastEndTime, endTime) << " sec." << "Total Time: " << static_cast<int>(totalTime);
	std::cout << " sec. Estimated Time left:" << static_cast<int>(estimatedTimeLeft) << " sec." << std::endl;
	lastEndTime = endTime;
}

float RGBDCaptureForGANWindow::differenceInSeconds(float startTime, float endTime)
{
	return ((endTime - startTime) / CLOCKS_PER_SEC);
}
