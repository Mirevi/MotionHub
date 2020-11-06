
#include "RGBDCaptureForGANWindow.h"
#include <opencv2/opencv.hpp>
#include "ui_RGBDCaptureForGANWindow.h"


RGBDCaptureForGANWindow::RGBDCaptureForGANWindow(ConfigManager* configManager, NetworkManager* networkManager, QWidget* parent) : QDialog(parent), ui(new Ui::RGBDCaptureForGANWindow)
{
	ui->setupUi(this);

	m_configManager = configManager; 
	m_networkManager = networkManager;

	m_azureKinectSensor = NULL;
	m_config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;

	//network
	m_senderId = 10000;
	m_transmissionId = 0;
	m_networkManager->createOSCSender(m_senderId);

	//states
	m_state = State::UNINITIALIZED;

	//capture
	m_currentCapture = new k4a::capture();
	m_framesToCapture = 1000;
	m_clippingDistance = 2850; //behind this value (millimeters) the points will be rejected

	//feature generation
	m_imageSize = cv::Size(256, 256);
	m_cropRegion = cv::Rect(cv::Point(280, 0), cv::Point(1000, 720));
	initializeFeatureLineColor();

	//save
	m_dataDirPath = "data\\RGBD-GAN\\";
	m_saveIdPrefix = "a";

	//preview
	m_showColorImagePreview = true;
	m_showDepthImagePreview = true;
	m_showFeatureImagePreview = true;

	updateGui();
}

void RGBDCaptureForGANWindow::updateGui()
{
	//settings
	ui->le_file_prefix->setText(QString::fromStdString(m_saveIdPrefix));
	ui->le_frames_to_capture->setText(QString::fromStdString(std::to_string(m_framesToCapture)));
	ui->le_crop_region_x1->setText(QString::fromStdString(std::to_string(m_cropRegion.tl().x)));
	ui->le_crop_region_y1->setText(QString::fromStdString(std::to_string(m_cropRegion.tl().y)));
	ui->le_crop_region_x2->setText(QString::fromStdString(std::to_string(m_cropRegion.br().x)));
	ui->le_crop_region_y2->setText(QString::fromStdString(std::to_string(m_cropRegion.br().y)));
	ui->le_image_size_x->setText(QString::fromStdString(std::to_string(m_imageSize.width)));
	ui->le_image_size_y->setText(QString::fromStdString(std::to_string(m_imageSize.height)));
	ui->le_clipping_distance->setText(QString::fromStdString(std::to_string(m_clippingDistance)));
	ui->le_transmission_id->setText(QString::fromStdString(std::to_string(m_transmissionId)));

	//preview buttons
	ui->cb_visualizeColor->setChecked(m_showColorImagePreview);
	if (m_showColorImagePreview)
		ui->cb_visualizeColor->setText("Do Not Visualize Color");
	else
		ui->cb_visualizeColor->setText("Visualize Color");

	ui->cb_visualizeDepth->setChecked(m_showDepthImagePreview);
	if (m_showDepthImagePreview)
		ui->cb_visualizeDepth->setText("Do Not Visualize Color");
	else
		ui->cb_visualizeDepth->setText("Visualize Color");

	ui->cb_visualizeLandmarks->setChecked(m_showFeatureImagePreview);
	if (m_showFeatureImagePreview)
		ui->cb_visualizeLandmarks->setText("Do Not Visualize Color");
	else
		ui->cb_visualizeLandmarks->setText("Visualize Color");

	//enable/disable
	if (m_state == State::UNINITIALIZED) {
		ui->btn_initiate_kinect->setEnabled(true);
		ui->btn_stop->setEnabled(false);
	}
	else {
		ui->btn_initiate_kinect->setEnabled(false);
		ui->btn_stop->setEnabled(true);
	}

	if (m_state == State::INITIALIZED) {
		ui->le_file_prefix->setEnabled(true);
		ui->le_frames_to_capture->setEnabled(true);
		ui->le_crop_region_x1->setEnabled(true);
		ui->le_crop_region_y1->setEnabled(true);
		ui->le_crop_region_x2->setEnabled(true);
		ui->le_crop_region_y2->setEnabled(true);
		ui->le_image_size_x->setEnabled(true);
		ui->le_image_size_y->setEnabled(true);
		ui->le_clipping_distance->setEnabled(true);
		ui->btn_start_capture->setEnabled(true);
		ui->btn_start_transmission->setEnabled(true);
		ui->btn_start_captured_transmission->setEnabled(true);
		ui->le_transmission_id->setEnabled(true);
	}
	else {
		ui->le_file_prefix->setEnabled(false);
		ui->le_frames_to_capture->setEnabled(false);
		ui->le_crop_region_x1->setEnabled(false);
		ui->le_crop_region_y1->setEnabled(false);
		ui->le_crop_region_x2->setEnabled(false);
		ui->le_crop_region_y2->setEnabled(false);
		ui->le_image_size_x->setEnabled(false);
		ui->le_image_size_y->setEnabled(false);
		ui->le_clipping_distance->setEnabled(false);
		ui->btn_start_capture->setEnabled(false);
		ui->btn_start_transmission->setEnabled(false);
		ui->btn_start_captured_transmission->setEnabled(false);
		ui->le_transmission_id->setEnabled(false);
	}

}

void RGBDCaptureForGANWindow::initializeFeatureLineColor()
{
	//TODO const color init (42 steps, 6 chaneel)
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == 7 && j == 3)
				continue;

			int b = (31 * (i + 1));
			int g = ((57 * (j + 1)) + b) % 255;

			m_featureLineColors[i * 4 + j] = cv::Vec4b(b, g, 0, 255);
		}
	}
}

RGBDCaptureForGANWindow::~RGBDCaptureForGANWindow()
{
	m_state = State::ENDING;
	delete ui;
}

void RGBDCaptureForGANWindow::reject() {
	m_state = State::ENDING;
	if (m_taskThread.get() != nullptr && m_taskThread->joinable())
		m_taskThread->join();
	QDialog::reject();
}

void RGBDCaptureForGANWindow::onGuiValueChanged() {

	//validate
	if (ui->le_frames_to_capture->text().length() == 0)
		ui->le_frames_to_capture->setText("0");
	if (ui->le_crop_region_x1->text().length() == 0)
		ui->le_crop_region_x1->setText("0");
	if (ui->le_crop_region_y1->text().length() == 0)
		ui->le_crop_region_y1->setText("0");
	if (ui->le_crop_region_x2->text().length() == 0)
		ui->le_crop_region_x2->setText("0");
	if (ui->le_crop_region_y2->text().length() == 0)
		ui->le_crop_region_y2->setText("0");
	if (ui->le_image_size_x->text().length() == 0)
		ui->le_image_size_x->setText("0");
	if (ui->le_image_size_y->text().length() == 0)
		ui->le_image_size_y->setText("0");
	if (ui->le_clipping_distance->text().length() == 0)
		ui->le_clipping_distance->setText("0");
	if (ui->le_transmission_id->text().length() == 0)
		ui->le_transmission_id->setText("0");

	//set values
	m_saveIdPrefix = ui->le_file_prefix->text().toStdString();
	m_framesToCapture = ui->le_frames_to_capture->text().toInt();

	cv::Point cropA(ui->le_crop_region_x1->text().toInt(), ui->le_crop_region_y1->text().toInt());
	cv::Point cropB(ui->le_crop_region_x2->text().toInt(), ui->le_crop_region_y2->text().toInt());
	m_cropRegion = cv::Rect(cropA, cropB);

	m_imageSize = cv::Size(ui->le_image_size_x->text().toInt(), ui->le_image_size_y->text().toInt());

	m_clippingDistance = ui->le_clipping_distance->text().toInt();
	m_transmissionId = ui->le_transmission_id->text().toInt();

	m_showColorImagePreview = ui->cb_visualizeColor->isChecked();
	m_showDepthImagePreview = ui->cb_visualizeDepth->isChecked();
	m_showFeatureImagePreview = ui->cb_visualizeLandmarks->isChecked();

	updateGui();
}

void RGBDCaptureForGANWindow::onInitializeAzureKinectTriggered()
{
	if (m_state == State::UNINITIALIZED) {
		if (m_taskThread.get() != nullptr && m_taskThread->joinable())
			m_taskThread->join();

		m_state = State::IS_INITIALIZING;
		updateGui();
		m_taskThread = std::shared_ptr<std::thread>(new std::thread(&RGBDCaptureForGANWindow::taskThreadMethod, this));
	}
}

void RGBDCaptureForGANWindow::onStopTaskAndCloseDevice()
{
	if (m_state != State::UNINITIALIZED)
		m_state = State::ENDING;
}

void RGBDCaptureForGANWindow::onStartCaptureTriggered()
{
	if (m_state == State::INITIALIZED)
		m_state = State::CAPTURING;
}

void RGBDCaptureForGANWindow::onStartLandmarkTransissionTriggered()
{
	if (m_state == State::INITIALIZED)
		m_state = State::TRANSMITTING;
}

void RGBDCaptureForGANWindow::onStartLandmarkTransissionWithCapturedDataTriggered()
{
	if (m_state == State::INITIALIZED)
		m_state = State::TRANSMITTING_FILE;
}

void RGBDCaptureForGANWindow::taskThreadMethod()
{
	printAndShowMessage("Start GAN Task Thread");

	initiateAzureKinect();
	updateUiLineEditBoundaries();
	updateGui();
	showAzurePreview();
	updateGui();

	switch (m_state) {
	case State::CAPTURING:
		startCapture();
		break;
	case State::TRANSMITTING:
		startLandmarkTransmission();
		break;
	case State::TRANSMITTING_FILE:
		startLandmarkTransmissionWithCapturedData();
		break;
	}

	printAndShowMessage("End GAN Task Thread");
	m_azureKinectSensor.close();
	m_tracker.destroy();
	m_state = State::UNINITIALIZED;
	updateGui();
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
	m_transformation = k4a::transformation(m_calibration);
	switch (m_calibration.color_resolution) {
	case K4A_COLOR_RESOLUTION_720P:
		m_colorResolution = cv::Size(1280, 720);
		break;
	case K4A_COLOR_RESOLUTION_1080P:
		m_colorResolution = cv::Size(1920, 1080);
		break;
	case K4A_COLOR_RESOLUTION_1440P:
		m_colorResolution = cv::Size(2560, 1440);
		break;
	case K4A_COLOR_RESOLUTION_1536P:
		m_colorResolution = cv::Size(2048, 1536);
		break;
	case K4A_COLOR_RESOLUTION_2160P:
		m_colorResolution = cv::Size(3840, 2160);
		break;
	case K4A_COLOR_RESOLUTION_3072P:
		m_colorResolution = cv::Size(4096, 3072);
		break;
	}
	printAndShowMessage("Initiate Azure Kinect done");

	//Initiate tracker
	m_trackerConfig = K4ABT_TRACKER_CONFIG_DEFAULT;
	m_tracker = m_tracker.create(m_calibration, m_trackerConfig);
	printAndShowMessage("Initiate Body Tracking Component done");

	m_state = State::INITIALIZED;
}

void RGBDCaptureForGANWindow::updateUiLineEditBoundaries() {
	ui->le_frames_to_capture->setValidator(new QIntValidator(0, 100000, this));
	ui->le_crop_region_x1->setValidator(new QIntValidator(0, m_colorResolution.width, this));
	ui->le_crop_region_y1->setValidator(new QIntValidator(0, m_colorResolution.height, this));
	ui->le_crop_region_x2->setValidator(new QIntValidator(0, m_colorResolution.width, this));
	ui->le_crop_region_y2->setValidator(new QIntValidator(0, m_colorResolution.height, this));
	ui->le_image_size_x->setValidator(new QIntValidator(0, m_colorResolution.width, this));
	ui->le_image_size_y->setValidator(new QIntValidator(0, m_colorResolution.height, this));
	ui->le_clipping_distance->setValidator(new QIntValidator(0, 65000, this));
	ui->le_transmission_id->setValidator(new QIntValidator(0, 1000, this));
}

void RGBDCaptureForGANWindow::showAzurePreview()
{
	printAndShowMessage("Show Preview");
	while (m_state == State::INITIALIZED) {
		m_azureKinectSensor.get_capture(m_currentCapture);
		m_tracker.enqueue_capture(*m_currentCapture, std::chrono::milliseconds(-1));
		m_bodyFrame = m_tracker.pop_result(std::chrono::milliseconds(-1));

		if (m_showColorImagePreview)
			processColorImage();
		if (m_showDepthImagePreview)
			processDepthImage();
		if (m_showFeatureImagePreview) {
			if (!currentCaptureIsValid(m_currentCapture))
				continue;
			extractJointLandmarks();
			if (!convertJointLandmarksToColorImageLandmarks())
				continue;
			processFeatureImage();
		}
	}
	std::cout << "End Show Preview" << std::endl;
}

void RGBDCaptureForGANWindow::startCapture()
{
	printAndShowMessage("Start RGBD Capture for GAN");
	std::cout << "id prefix: " << m_saveIdPrefix << std::endl;

	m_frameCounter = 0;
	m_startTime = clock();
	m_totalSubtaskTime = 0;
	m_skippedFrames = 0;

	//initiation
	prepareSaveDirLocation();
	m_jointLandmarkWriter.open(m_saveDirPath + "jointLandmarks.txt");

	//process capture and save
	while (m_state == State::CAPTURING && m_frameCounter < m_framesToCapture) {
		m_subtaskStartTime = clock();

		m_azureKinectSensor.get_capture(m_currentCapture);
		m_tracker.enqueue_capture(*m_currentCapture, std::chrono::milliseconds(-1));
		m_bodyFrame = m_tracker.pop_result(std::chrono::milliseconds(-1));

		if (m_skippedFrames >= 200)
			printSkipFramesToConsole();

		if (!currentCaptureIsValid(m_currentCapture)) {
			m_skippedFrames++;
			continue;
		}

		extractJointLandmarks();
		if (!convertJointLandmarksToColorImageLandmarks()) {
			m_skippedFrames++;
			continue;
		}

		if (m_skippedFrames > 0)
			printSkipFramesToConsole();

		processColorImage();
		processDepthImage();
		processInfraredImage();
		processFeatureImage();

		saveProcessedCapturedData();

		m_subtaskEndTime = clock();
		m_totalSubtaskTime += differenceInSeconds(m_subtaskStartTime, m_subtaskEndTime);
		printTaskConsoleInfos(m_frameCounter, m_framesToCapture);
	}

	m_jointLandmarkWriter.close();

	m_endTime = clock();
	std::cout << "End RGBD Capture for GAN after " << differenceInSeconds(m_startTime, m_endTime) 
		<< " seconds." << std::endl;
	m_state = State::ENDING;
}

void RGBDCaptureForGANWindow::prepareSaveDirLocation()
{
	if (m_saveIdPrefix.length() > 0)
		m_saveDirPath = m_dataDirPath + m_saveIdPrefix + "\\";
	else
		m_saveDirPath = std::string(m_dataDirPath);

	if (!std::filesystem::exists(m_saveDirPath)) {
		//create dir
		std::filesystem::create_directory(m_saveDirPath);
	}
	//prepare subdirs
	if (std::filesystem::exists(m_saveDirPath + "COLOR\\")) {
		for (const auto& entry : std::filesystem::directory_iterator(m_saveDirPath + "COLOR\\"))
			std::filesystem::remove_all(entry.path());
	}
	else
		std::filesystem::create_directory(m_saveDirPath + "COLOR\\");

	if (std::filesystem::exists(m_saveDirPath + "DEPTH\\")) {
		for (const auto& entry : std::filesystem::directory_iterator(m_saveDirPath + "DEPTH\\"))
			std::filesystem::remove_all(entry.path());
	}
	else
		std::filesystem::create_directory(m_saveDirPath + "DEPTH\\");

	if (std::filesystem::exists(m_saveDirPath + "INFRARED\\")) {
		for (const auto& entry : std::filesystem::directory_iterator(m_saveDirPath + "INFRARED\\"))
			std::filesystem::remove_all(entry.path());
	}
	else
		std::filesystem::create_directory(m_saveDirPath + "INFRARED\\");

	if (std::filesystem::exists(m_saveDirPath + "FEATURE\\")) {
		for (const auto& entry : std::filesystem::directory_iterator(m_saveDirPath + "FEATURE\\"))
			std::filesystem::remove_all(entry.path());
	}
	else
		std::filesystem::create_directory(m_saveDirPath + "FEATURE\\");
}

boolean RGBDCaptureForGANWindow::currentCaptureIsValid(k4a::capture* capture)
{
	return capture->get_color_image()
		&& capture->get_depth_image()
		&& capture->get_ir_image()
		//only one body is allowed
		&& m_bodyFrame.get_num_bodies() == 1;
}

void RGBDCaptureForGANWindow::processColorImage()
{
	k4a::image color_image = m_currentCapture->get_color_image();

	//Create a color image buffer
	uint8_t* color_buffer = color_image.get_buffer();
	size_t color_buffer_size = color_image.get_size();

	//Need dimension in order to allocate memory
	int colorWidth = color_image.get_width_pixels();
	int colorHeight = color_image.get_height_pixels();

	//convert to cv:Mat
	m_colorMat = std::shared_ptr<cv::Mat>(new cv::Mat(colorHeight, colorWidth, CV_8UC4, (void*)color_buffer, cv::Mat::AUTO_STEP));

	cropAndResizeMat(&m_colorMat);

	if (m_showColorImagePreview) {
		QImage colorImage = QImage(m_colorMat->data, m_colorMat->cols, m_colorMat->rows, m_colorMat->step, QImage::Format_ARGB32);
		colorImage = colorImage.scaled(256, 256, Qt::AspectRatioMode::KeepAspectRatio);
		ui->color_Image->setPixmap(QPixmap::fromImage(colorImage));
	}
}

void RGBDCaptureForGANWindow::processDepthImage()
{
	k4a::image depthImage = m_currentCapture->get_depth_image();

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
	m_transformedDepthImage = k4a::image::create(K4A_IMAGE_FORMAT_DEPTH16,
		m_calibration.color_camera_calibration.resolution_width,
		m_calibration.color_camera_calibration.resolution_height,
		m_calibration.color_camera_calibration.resolution_width *
		static_cast<int>(sizeof(uint16_t)));
	m_transformation.depth_image_to_color_camera(depthImage, &m_transformedDepthImage);
	uint8_t* transformed_depth_buffer = m_transformedDepthImage.get_buffer();

	//Get dimensions of the "new" transformed depth image and get the dimension in order to allocate memory
	int transformedDepthWidth = m_transformedDepthImage.get_width_pixels();
	int transformedDepthHeight = m_transformedDepthImage.get_height_pixels();

	m_depthMat = std::shared_ptr<cv::Mat>(new cv::Mat(transformedDepthHeight, transformedDepthWidth, CV_16UC1, (void*)transformed_depth_buffer, cv::Mat::AUTO_STEP));

	cropAndResizeMat(&m_depthMat);

	cv::Mat depthMat8 = cv::Mat(transformedDepthHeight, transformedDepthWidth, CV_8UC1);

	if (m_showDepthImagePreview) {
		m_depthMat->convertTo(depthMat8, CV_8UC1, 255.0 / m_clippingDistance);
		QImage image = QImage(
			depthMat8.data,
			depthMat8.cols,
			depthMat8.rows,
			depthMat8.step,
			QImage::Format_Grayscale8
		);
		image = image.scaled(256, 256, Qt::AspectRatioMode::KeepAspectRatio);
		ui->depth_image->setPixmap(QPixmap::fromImage(image));
	}
}

void RGBDCaptureForGANWindow::processInfraredImage()
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

	m_depthAndIrImagePair = transformation.depth_image_to_color_camera_custom(depthImage,
		customIrImage,
		K4A_TRANSFORMATION_INTERPOLATION_TYPE_NEAREST,
		0);

	//Need dimension in order to allocate memory
	int irImageWidth = m_depthAndIrImagePair.second.get_width_pixels();
	int irImageHeight = m_depthAndIrImagePair.second.get_height_pixels();

	//convert to cv:Mat
	m_irMat = std::shared_ptr<cv::Mat>(new cv::Mat(irImageHeight, irImageWidth, CV_16UC1, m_depthAndIrImagePair.second.get_buffer(), cv::Mat::AUTO_STEP));

	cropAndResizeMat(&m_irMat);
}

void RGBDCaptureForGANWindow::processFeatureImage()
{
	cv::Mat temp = cv::Mat::zeros(m_imageSize, CV_8UC4);
	m_featureMatrix = std::shared_ptr<cv::Mat>(new cv::Mat(temp)); //copy operation is fast enough

	//color landmark pixels

	mapColorLandmarksToFeatureImageLandmarks();
	drawFeaturesToMatrix();

	if (m_showFeatureImagePreview) {
		QImage colorImage = QImage(m_featureMatrix->data, m_featureMatrix->cols, m_featureMatrix->rows, m_featureMatrix->step, QImage::Format_ARGB32);
		colorImage = colorImage.scaled(256, 256, Qt::AspectRatioMode::KeepAspectRatio);
		ui->feature_image->setPixmap(QPixmap::fromImage(colorImage));
	}
}

void RGBDCaptureForGANWindow::cropAndResizeMat(std::shared_ptr<cv::Mat>* mat)
{
	//crop
	cv::Mat temp = **mat;
	cv::Mat* cropped = new cv::Mat(temp(m_cropRegion));
	mat->swap(std::shared_ptr<cv::Mat>(cropped));

	//resize
	cv::resize(*mat->get(), *mat->get(), m_imageSize);
}

void RGBDCaptureForGANWindow::saveProcessedCapturedData()
{
	saveColorImage();
	saveDepthImage();
	saveIrImage();
	saveJointLandmarks();
	saveFeatureImage();
	m_frameCounter++;
}

void RGBDCaptureForGANWindow::saveColorImage()
{
	//save
	std::string colorFileName = imageFilePath(ImageType::COLOR_IMAGE);
	cv::imwrite(colorFileName, *m_colorMat, std::vector<int>());
}

void RGBDCaptureForGANWindow::saveDepthImage()
{
	//Create a file name
	std::string depthFileName = imageFilePath(ImageType::DEPTH_IMAGE);
	cv::imwrite(depthFileName, *m_depthMat, std::vector<int>());
}

void RGBDCaptureForGANWindow::saveIrImage()
{
	//save
	std::string irFileName = imageFilePath(ImageType::INFRARED_IMAGE);
	cv::imwrite(irFileName, *m_irMat, std::vector<int>());
}

void RGBDCaptureForGANWindow::saveFeatureImage()
{
	//save
	std::string featureImageFileName = imageFilePath(ImageType::FEATURE_IMAGE);
	cv::imwrite(featureImageFileName, *m_featureMatrix, std::vector<int>());
}

void RGBDCaptureForGANWindow::extractJointLandmarks()
{
	k4abt_skeleton_t skeleton = m_bodyFrame.get_body_skeleton(0);
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

void RGBDCaptureForGANWindow::saveJointLandmarks()
{
	//save to text file
	m_jointLandmarkWriter << "ID: " << m_saveIdPrefix << m_frameCounter << std::endl;

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

	//draw landmark points
	/*for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
		uchar x1 = static_cast<int>(m_imageLandmarks[i].x);
		uchar y1 = static_cast<int>(m_imageLandmarks[i].y);

		cv::circle(*m_featureMatrix, cv::Point(x1, y1), 1, cv::Scalar(0, 255, 0, 255), -1);
	}*/
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
	int x1 = static_cast<int>(m_featureImageLandmarks[landmarkIndexStart].x);
	int y1 = static_cast<int>(m_featureImageLandmarks[landmarkIndexStart].y);
	int depth1 = static_cast<int>(m_featureImageLandmarks[landmarkIndexStart].depth);

	int x2 = static_cast<int>(m_featureImageLandmarks[landmarkIndexEnd].x);
	int y2 = static_cast<int>(m_featureImageLandmarks[landmarkIndexEnd].y);
	int depth2 = static_cast<int>(m_featureImageLandmarks[landmarkIndexEnd].depth);

	cv::LineIterator lineIterator(*m_featureMatrix, cv::Point(x1, y1), cv::Point(x2, y2));
	cv::Vec4b color = m_featureLineColors[landmarkIndexEnd - 1];

	for (int i = 0; i < lineIterator.count; i++, lineIterator++) {
		float delta = float(i) / lineIterator.count;
		color[2] = depth1 + ((depth2 - depth1) * delta);

		cv::Point position = lineIterator.pos();
		int x = position.x;
		int y = position.y;

		for (int i = -1; i <= 1; i++) drawPixelWithDepthTest(x + i, y - 2, color);
		for (int i = -2; i <= 2; i++) drawPixelWithDepthTest(x + i, y - 1, color);
		for (int i = -3; i <= 3; i++) drawPixelWithDepthTest(x + i, y, color);
		for (int i = -2; i <= 2; i++) drawPixelWithDepthTest(x + 1, y + 1, color);
		for (int i = -1; i <= 1; i++)drawPixelWithDepthTest(x + i, y + 2, color);
	}
}

void RGBDCaptureForGANWindow::drawPixelWithDepthTest(int x, int y, cv::Vec4b& color)
{
	if (x >= 0 && x < m_featureMatrix->cols && y >= 0 && y < m_featureMatrix->rows) {
		cv::Vec4b currentColor = m_featureMatrix->at<cv::Vec4b>(y, x);
		if (currentColor[2] < color[2]) {
			m_featureMatrix->at<cv::Vec4b>(y, x) = color;
		}
	}
}

void RGBDCaptureForGANWindow::startLandmarkTransmission()
{
	printAndShowMessage("Start Landmark Transmission for GAN");

	m_startTime = clock();
	m_totalSubtaskTime = 0;
	m_skippedFrames = 0;

	while(m_state == State::TRANSMITTING) {
		m_subtaskStartTime = clock();

		m_azureKinectSensor.get_capture(m_currentCapture);
		m_tracker.enqueue_capture(*m_currentCapture, std::chrono::milliseconds(-1));
		m_bodyFrame = m_tracker.pop_result(std::chrono::milliseconds(-1));

		if (m_skippedFrames >= 200)
			printSkipFramesToConsole();

		if (!currentCaptureIsValid(m_currentCapture)) {
			m_skippedFrames++;
			continue;
		}

		extractJointLandmarks();
		if (!convertJointLandmarksToColorImageLandmarks()) {
			m_skippedFrames++;
			continue;
		}

		mapColorLandmarksToFeatureImageLandmarks();
		sendImageLandmarksOverNetwork();

		if (m_skippedFrames > 0)
			printSkipFramesToConsole();

		m_subtaskEndTime = clock();
		printTaskConsoleInfos();

		//gui visualization
		if (m_showColorImagePreview)
			processColorImage();
		if (m_showDepthImagePreview)
			processDepthImage();
		if (m_showDepthImagePreview) 
			processFeatureImage();
	}

	m_endTime = clock();
	std::cout << "End Landmark Transmission for GAN after " << differenceInSeconds(m_startTime, m_endTime)
		<< " seconds." << std::endl;
}

void RGBDCaptureForGANWindow::startLandmarkTransmissionWithCapturedData()
{
	printAndShowMessage("Start Landmark Transission with captured Data");

	m_startTime = clock();
	m_totalSubtaskTime = 0;
	m_skippedFrames = 0;

	m_jointLandmarkReader.open(m_dataDirPath + "jointLandmarks.txt");

	//count frames for console output
	countFramesInLandmarkFile();

	//reset reader
	m_jointLandmarkReader.clear();
	m_jointLandmarkReader.seekg(0);

	//process data from file
	processLandmarkFileData();

	m_jointLandmarkReader.close();

	m_endTime = clock();
	std::cout << "End Landmark Transission with captured Data after "
		<< differenceInSeconds(m_startTime, m_endTime) << " seconds." << std::endl;
}

void RGBDCaptureForGANWindow::countFramesInLandmarkFile()
{
	m_landmarkFileFrameCount = 0;

	std::string line;

	while (std::getline(m_jointLandmarkReader, line)) {
		if (stringStartsWith(&line, "END")) {
			m_landmarkFileFrameCount++;
		}
	}
}

void RGBDCaptureForGANWindow::processLandmarkFileData()
{
	m_frameCounter = 0;
	std::string line;
	std::string splitted[K4ABT_JOINT_COUNT];
	m_subtaskStartTime = clock();

	while (m_state == State::TRANSMITTING_FILE && std::getline(m_jointLandmarkReader, line)) {
		if (stringStartsWith(&line, "ID: ")) {
			m_id = line.substr(4);
		}
		else if (stringStartsWith(&line, "X: ")) {
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

			if (!convertJointLandmarksToColorImageLandmarks())
				continue;
			mapColorLandmarksToFeatureImageLandmarks();
			sendImageLandmarksOverNetwork();

			if (m_showDepthImagePreview) {
				processFeatureImage(); 
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100)); //otherwise network is too fast

			m_subtaskEndTime = clock();
			m_totalSubtaskTime += differenceInSeconds(m_subtaskStartTime, m_subtaskEndTime);
			printTaskConsoleInfos(m_frameCounter, m_landmarkFileFrameCount);
			m_frameCounter++;
			m_subtaskStartTime = clock();
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

void RGBDCaptureForGANWindow::sendImageLandmarksOverNetwork()
{
	//convert to std::vector
	std::vector<Landmark> imageLandmarks;
	imageLandmarks.resize(K4ABT_JOINT_COUNT);

	for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
		imageLandmarks[i] = m_featureImageLandmarks[i];
	}

	//send
	m_networkManager->sendImageLandmarks(&imageLandmarks, m_transmissionId, m_senderId);
}

boolean RGBDCaptureForGANWindow::convertJointLandmarksToColorImageLandmarks()
{
	for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
		k4a_float3_t jointPosition;
		k4a_float2_t imagePosition;

		jointPosition.xyz.x = m_jointLandmarks[i].x;
		jointPosition.xyz.y = m_jointLandmarks[i].y;
		jointPosition.xyz.z = m_jointLandmarks[i].depth;

		if (m_calibration.convert_3d_to_2d(
			jointPosition,
			k4a_calibration_type_t::K4A_CALIBRATION_TYPE_DEPTH,
			k4a_calibration_type_t::K4A_CALIBRATION_TYPE_COLOR,
			&imagePosition
		)) {

			Landmark colorImageLandmark;
			colorImageLandmark.x = imagePosition.xy.x;
			colorImageLandmark.y = imagePosition.xy.y;
			colorImageLandmark.depth = 256 - (jointPosition.xyz.z / m_clippingDistance) * 256;

			m_colorImageLandmarks[i] = colorImageLandmark;
		}
		else {
			return false;
		}
	}

	return true;
}

void RGBDCaptureForGANWindow::mapColorLandmarksToFeatureImageLandmarks()
{
	float cropRegionOffsetX = m_cropRegion.tl().x;
	float cropRegionOffsetY = m_cropRegion.tl().y;
	float scaleFactorX = static_cast<float>(m_imageSize.width) / static_cast<float>(m_cropRegion.size().width);
	float scaleFactorY = static_cast<float>(m_imageSize.height) / static_cast<float>(m_cropRegion.size().height);

	for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
		Landmark featureImageLandmark;
		featureImageLandmark.x = (m_colorImageLandmarks[i].x - cropRegionOffsetX) * scaleFactorX;
		featureImageLandmark.y = (m_colorImageLandmarks[i].y - cropRegionOffsetY) * scaleFactorY;
		featureImageLandmark.depth = m_colorImageLandmarks[i].depth;

		m_featureImageLandmarks[i] = featureImageLandmark;
	}

}

std::string RGBDCaptureForGANWindow::imageFilePath(ImageType type)
{
	std::string fullFilePath;
	fullFilePath.append(m_saveDirPath);
	switch (type) {
	case ImageType::COLOR_IMAGE:
		fullFilePath.append("COLOR\\colorImage_");
		break;
	case ImageType::DEPTH_IMAGE:
		fullFilePath.append("DEPTH\\depthImage_");
		break;
	case ImageType::INFRARED_IMAGE:
		fullFilePath.append("INFRARED\\infraredImage_");
		break;
	case ImageType::FEATURE_IMAGE:
		fullFilePath.append("FEATURE\\featureImage_");
		break;
	}

	fullFilePath.append(m_saveIdPrefix);
	fullFilePath.append(std::to_string(m_frameCounter));

	fullFilePath.append(".png");

	return fullFilePath;
}

void RGBDCaptureForGANWindow::printTaskConsoleInfos(int currentSubtask, int subtaskCount)
{
	std::ostringstream messageStream;

	float totalTime = differenceInSeconds(m_startTime, clock());
	float subtaskTime = differenceInSeconds(m_subtaskStartTime, m_subtaskEndTime);

	messageStream << "Frame ";

	if (currentSubtask >= 0)
		messageStream << currentSubtask << " ";

	if (currentSubtask >= 0 && subtaskCount >= 0)
		messageStream << "/ " << subtaskCount << " ";

	messageStream << "needed " << subtaskTime << " sec. Total Time: " << static_cast<int>(totalTime) << " sec.";

	if (currentSubtask >= 0 && subtaskCount >= 0) {
		float estimatedTimeLeft = (m_totalSubtaskTime / currentSubtask) * (subtaskCount - currentSubtask);
		messageStream << " Estimated Time left:" << static_cast<int>(estimatedTimeLeft) << " sec.";
	}

	printAndShowMessage(messageStream.str());
}

void RGBDCaptureForGANWindow::printSkipFramesToConsole()
{
	float totalTime = differenceInSeconds(m_startTime, clock());
	std::ostringstream messageStream;
	messageStream << "Skipped " << m_skippedFrames << " frames because image data is missing or "
		<< "no / too many bodies were detected. Total time: " << totalTime ;
	printAndShowMessage(messageStream.str());
	m_skippedFrames = 0;
}

void RGBDCaptureForGANWindow::printAndShowMessage(std::string message)
{
	std::cout << message << std::endl;
	ui->l_message_label->setText(QString::fromStdString(message));
}

float RGBDCaptureForGANWindow::differenceInSeconds(float startTime, float endTime)
{
	return ((endTime - startTime) / CLOCKS_PER_SEC);
}
