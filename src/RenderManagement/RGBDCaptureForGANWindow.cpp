
#include "RGBDCaptureForGANWindow.h"


RGBDCaptureForGANWindow::RGBDCaptureForGANWindow(ConfigManager* configManager, QWidget* parent) : QDialog(parent), ui(new Ui::RGBDCaptureForGANWindow)
{
	ui->setupUi(this);

	m_configManager = configManager;

	m_framesToCapture = 10;
	m_azureKinectSensor = NULL;
	m_config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	m_clippingDistance = 2048; //behind this value (millimeters) the points will be rejected

	m_landmarkImageSize = 256;
	m_landmarkImagePadding = 2;

}

RGBDCaptureForGANWindow::~RGBDCaptureForGANWindow()
{

	delete ui;

}

void RGBDCaptureForGANWindow::initiateAzureKinect()
{

	m_kinectCapture = new k4a::capture();
	//TODO: error handling
	m_azureKinectSensor = k4a::device::open(K4A_DEVICE_DEFAULT);

	m_config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
	m_config.color_resolution = K4A_COLOR_RESOLUTION_720P;
	m_config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
	m_config.camera_fps = K4A_FRAMES_PER_SECOND_30;
	m_config.synchronized_images_only = true; // ensures that depth and color images are both available in the capture
	m_azureKinectSensor.start_cameras(&m_config);
	std::cout << "Initiate Azure Kinect done" << std::endl;

	//Initiate tracker
	m_tracker = NULL;
	m_trackerConfig = K4ABT_TRACKER_CONFIG_DEFAULT;
	m_calibration = m_azureKinectSensor.get_calibration(m_config.depth_mode, m_config.color_resolution);
	m_tracker = m_tracker.create(m_calibration, m_trackerConfig);
	std::cout << "Initiate Body Tracking Component done" << std::endl;

}

void RGBDCaptureForGANWindow::startCapture()
{

	std::cout << "Start RGBD Capture for GAN" << std::endl;



	int returnCode = 1;
	const int32_t TIMEOUT_IN_MS = 1000;
	k4a::transformation transformation;
	k4a_transformation_t transformation_color_downscaled = NULL;
	std::string file_name = "";
	uint32_t device_count = 0;
	k4a::image color_image;
	k4a::image depthImage;
	k4a::image ir_image;
	k4a::image transformedDepthImage;
	std::vector<k4a::capture> captures;

	//####################################

	captures.reserve(m_framesToCapture);

	//####################################
	//Unimportant stuff vor face scanning
	k4a::calibration calibration;
	calibration = m_azureKinectSensor.get_calibration(m_config.depth_mode, m_config.color_resolution);
	transformation = k4a::transformation(calibration);

	//####################################
	//Now it's getting interesting for face scanning
	//Take a number of frames
	for (int j = 0; j < m_framesToCapture + 1; j++)
	{
		m_azureKinectSensor.get_capture(m_kinectCapture);
		captures.push_back(*m_kinectCapture);
	}

	//k4a::image depthAverageImage = captures.at(0).get_depth_image();
	//uint8_t* depth_buffer_average = depthAverageImage.get_buffer();
	//for (size_t p = 0; p < depthAverageImage.get_size(); p += 2)
	//{
	//	depth_buffer_average[p] = 0;
	//	depth_buffer_average[p + 1] = 0;
	//}


	for (int j = 1; j < m_framesToCapture + 1; j++)
	{
		//Get a color image capture
		color_image = captures.at(j).get_color_image();

		//Get a depth image capture
		//depthImage = k4a_capture_get_depth_image(captures.at(j));
		depthImage = captures.at(j).get_depth_image();

		ir_image = captures.at(j).get_ir_image();

		//Check if results are ok
		if (color_image && depthImage && ir_image)
		{
			//Create a color image buffer
			uint8_t* color_buffer = color_image.get_buffer();
			size_t color_buffer_size = color_image.get_size();

			// The YUY2 image format is the same stride as the 16-bit depth image, so we can modify it in-place.
			uint8_t* depth_buffer = depthImage.get_buffer();
			size_t depth_buffer_size = depthImage.get_size();

			//Create IR image buffer
			uint8_t* ir_buffer = ir_image.get_buffer();
			size_t ir_buffer_size = ir_image.get_size();

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

			std::stringstream dirSavePath;
			//time_t t = time(0);   // get time now
			//struct tm* now = localtime(&t);
			//dirSavePath <<
			//	"data\\RGBD-GAN\\"
			//	<< now->tm_year + 1900 << "-"
			//	<< now->tm_mon + 1 << "-"
			//	<< now->tm_mday << "---"
			//	<< now->tm_hour << "-"
			//	<< now->tm_min << "-"
			//	<< now->tm_sec;

			dirSavePath << "data\\RGBD-GAN\\";


			//Need dimension in order to allocate memory
			m_kinectColorImage = m_kinectCapture->get_color_image();
			int colorWidth = m_kinectColorImage.get_width_pixels();
			int colorHeight = m_kinectColorImage.get_height_pixels();
			//Create a file name
			std::string colorFileName;
			colorFileName.append(dirSavePath.str());
			colorFileName.append("_colorImage_");
			colorFileName.append(std::to_string(j));
			colorFileName.append(".png");
			std::thread* threadColorImage;

			cv::Mat colorMat(colorHeight, colorWidth, CV_8UC4, (void*)color_buffer, cv::Mat::AUTO_STEP);
			threadColorImage = new std::thread(&cv::imwrite, colorFileName, colorMat, std::vector<int>());
			//Tooo slow
			//threadColorImage = new std::thread(&PointCloudManager::saveImage, this, osgColorImage, colorFileName);
			//osgDB::writeImageFile(*osgColorImage, colorFileName);


			//Adpat the depth image to the size, aspect ratio and intrinsic parameters of the color cam
			k4a::image transformedDepthImage = k4a::image::create(K4A_IMAGE_FORMAT_DEPTH16,
				calibration.color_camera_calibration.resolution_width,
				calibration.color_camera_calibration.resolution_height,
				calibration.color_camera_calibration.resolution_width *
				static_cast<int>(sizeof(uint16_t)));
			transformation.depth_image_to_color_camera(depthImage, &transformedDepthImage);
			uint8_t* transformed_depth_buffer = transformedDepthImage.get_buffer();

			//Get dimensions of the "new" transformed depth image and get the dimension in order to allocate memory
			int transformedDepthWidth = transformedDepthImage.get_width_pixels();
			int transformedDepthHeight = transformedDepthImage.get_height_pixels();

			//Create a file name
			std::string depthFileName;
			depthFileName.append(dirSavePath.str());
			depthFileName.append("_depthImage_");
			depthFileName.append(std::to_string(j));
			depthFileName.append(".png");
			std::thread* threadDepthImage;

			cv::Mat depthMat(transformedDepthHeight, transformedDepthWidth, CV_16UC1, (void*)transformed_depth_buffer, cv::Mat::AUTO_STEP);
			threadDepthImage = new std::thread(&cv::imwrite, depthFileName, depthMat, std::vector<int>());

			//SAVE IR IMAGE #######################################################
			k4a::image customIrImage = NULL;
			int cirImageWidth = ir_image.get_width_pixels();
			int cirImageHeight = ir_image.get_height_pixels();
			int cirImageStrideBytes = ir_image.get_stride_bytes();
			uint8_t* cirImageBuffer = ir_image.get_buffer();
			customIrImage = k4a::image::create_from_buffer(K4A_IMAGE_FORMAT_CUSTOM16,
				cirImageWidth,
				cirImageHeight,
				cirImageStrideBytes,
				cirImageBuffer,
				cirImageHeight * cirImageStrideBytes,
				NULL,
				NULL);

			//k4a::image transformedIRImage = k4a::image::create(K4A_IMAGE_FORMAT_CUSTOM16,
			//	calibration.color_camera_calibration.resolution_width,
			//	calibration.color_camera_calibration.resolution_height,
			//	calibration.color_camera_calibration.resolution_width *
			//	static_cast<int>(sizeof(uint16_t)));

			//Adpat the ir image to the size, aspect ratio and intrinsic parameters of the color cam
			//k4a::image transformedIRImage = k4a::image::create(K4A_IMAGE_FORMAT_IR16,
			//	calibration.color_camera_calibration.resolution_width,
			//	calibration.color_camera_calibration.resolution_height,
			//	calibration.color_camera_calibration.resolution_width *
			//	static_cast<int>(sizeof(uint16_t)));

			std::pair<k4a::image, k4a::image> images = transformation.depth_image_to_color_camera_custom(depthImage,
				customIrImage,
				K4A_TRANSFORMATION_INTERPOLATION_TYPE_NEAREST,
				0);
			//transformation.depth_image_to_color_camera_custom(ir_image, &transformedIRImage);
			uint8_t* transformed_ir_buffer = customIrImage.get_buffer();


			//Need dimension in order to allocate memory
			int irImageWidth = images.second.get_width_pixels();
			int irImageHeight = images.second.get_height_pixels();

			//Create a file name
			std::string irFileName;
			irFileName.append(dirSavePath.str());
			irFileName.append("_IRimage_");
			irFileName.append(std::to_string(j));
			irFileName.append(".png");
			std::thread* threadIrImage;
			cv::Mat irMat(irImageHeight, irImageWidth, CV_16UC1, (void*)images.second.get_buffer(), cv::Mat::AUTO_STEP);
			threadIrImage = new std::thread(&cv::imwrite, irFileName, irMat, std::vector<int>());
			//threadIrImage = new std::thread(&PointCloudManager::saveImage, this, osgIRimage, irFileName);

			threadDepthImage->join();
			threadColorImage->join();
			threadIrImage->join();

			//SAVE LANDMARK IMAGE #######################################################

			//if something is wrong, check out the timeout parameter of the following method calls
			m_tracker.enqueue_capture(*m_kinectCapture); 
			k4abt::frame bodyFrame = m_tracker.pop_result();

			extractImageLandmarks(&bodyFrame);

			//create cv matrix and set black

			m_landmarkMat = cv::Mat::zeros(m_landmarkImageSize, m_landmarkImageSize, CV_8UC3);

			//color landmark pixels

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

			std::string landmarkFileName;
			landmarkFileName.append(dirSavePath.str());
			landmarkFileName.append("_landmark_");
			landmarkFileName.append(std::to_string(j));
			landmarkFileName.append(".png");
			std::thread* threadLandmarkImage;
			threadLandmarkImage = new std::thread(&cv::imwrite, landmarkFileName, m_landmarkMat, std::vector<int>());
			threadLandmarkImage->join();

		}

	}

	m_azureKinectSensor.close();
	std::cout << "End RGBD Capture for GAN" << std::endl;
}

void RGBDCaptureForGANWindow::continuousLineDrawingBetweenLandmarks(int start, int end)
{
	for (int i = start + 1; i < end; i++)
	{
		drawSingleLineBetweenLandmarks(i - 1, i);
	}
}

void RGBDCaptureForGANWindow::drawSingleLineBetweenLandmarks(int landmarkIndexStart, int landmarkIndexEnd)
{
	uchar x1 = static_cast<int>(m_imageLandmarks[landmarkIndexStart].x);
	uchar y1 = static_cast<int>(m_imageLandmarks[landmarkIndexStart].y);

	uchar x2 = static_cast<int>(m_imageLandmarks[landmarkIndexEnd].x);
	uchar y2 = static_cast<int>(m_imageLandmarks[landmarkIndexEnd].y);

	line(m_landmarkMat, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255), 4);
}

// extract skeletons from a body frame
void RGBDCaptureForGANWindow::extractImageLandmarks(k4abt::frame* bodyFrame) {
	uint32_t bodyCount = bodyFrame->get_num_bodies();

	//only one body is allowed
	if (bodyCount == 1) {
		k4abt_skeleton_t skeleton = bodyFrame->get_body_skeleton(0);
		k4abt_joint_t* joints = skeleton.joints;

		//first joint position
		k4a_float3_t jointPosition = joints[0].position;
		
		//get min and max values of joint positions for normalization and landmark extraction

		Vector3 minPositionAxisValues = Vector3(jointPosition.xyz.x, jointPosition.xyz.y, jointPosition.xyz.z);
		Vector3 maxPositionAxisValues = Vector3(jointPosition.xyz.x, jointPosition.xyz.y, jointPosition.xyz.z);

		for (int i = 1; i < K4ABT_JOINT_COUNT; i++) {
			jointPosition = joints[i].position;

			//x
			if (jointPosition.xyz.x < minPositionAxisValues.m_xyz.x)
				minPositionAxisValues.m_xyz.x = jointPosition.xyz.x;
			else if (jointPosition.xyz.x > maxPositionAxisValues.m_xyz.x)
				maxPositionAxisValues.m_xyz.x = jointPosition.xyz.x;

			//y
			if (jointPosition.xyz.y < minPositionAxisValues.m_xyz.y)
				minPositionAxisValues.m_xyz.y = jointPosition.xyz.y;
			else if (jointPosition.xyz.y > maxPositionAxisValues.m_xyz.y)
				maxPositionAxisValues.m_xyz.y = jointPosition.xyz.y;

			//z
			if (jointPosition.xyz.z < minPositionAxisValues.m_xyz.z)
				minPositionAxisValues.m_xyz.z = jointPosition.xyz.z;
			else if (jointPosition.xyz.z > maxPositionAxisValues.m_xyz.z)
				maxPositionAxisValues.m_xyz.z = jointPosition.xyz.z;
		}

		//normalize joint positions without loosing the proportion between x and y to get normalized landmarks

		Vector3 jointPositionSpaceSize = maxPositionAxisValues - minPositionAxisValues;

		float xOffset = 0;
		float yOffset = 0;
		float maxSpaceSize; //value for normalization

		//check which axis space is bigger than the other and which has to be filled to obtain the proportions
		if (jointPositionSpaceSize.m_xyz.x > jointPositionSpaceSize.m_xyz.y) {
			maxSpaceSize = jointPositionSpaceSize.m_xyz.x;
			//x space is bigger than y space so the missing space of y has to be filled (* 0.5 for center alignment)
			yOffset = abs(jointPositionSpaceSize.m_xyz.x - jointPositionSpaceSize.m_xyz.y) * 0.5f;
		}
		else 
		{
			maxSpaceSize = jointPositionSpaceSize.m_xyz.y;
			//y space is bigger or the same as x space so the eventually missing space of x has to be filled * 0.5 for center alignment)
			xOffset = abs(jointPositionSpaceSize.m_xyz.x - jointPositionSpaceSize.m_xyz.y) * 0.5f;
		}

		//Calculate and save normalized landmarks

		for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
			jointPosition = joints[i].position;

			Landmark current;
			current.x = (jointPosition.xyz.x - minPositionAxisValues.m_xyz.x + xOffset) / maxSpaceSize;
			current.y = (jointPosition.xyz.y - minPositionAxisValues.m_xyz.y + yOffset) / maxSpaceSize;
			current.depth = 0; //TODO calculate a value you can proceed with

			m_normalizedLanmarks[i] = current;
		}

		//Calculate image landmarks

		float landmarkSpaceOnImage = m_landmarkImageSize - 2 * m_landmarkImagePadding;

		for (int i = 0; i < K4ABT_JOINT_COUNT; i++) {
			Landmark current;
			current.x = m_normalizedLanmarks[i].x * landmarkSpaceOnImage + m_landmarkImagePadding;
			current.y = m_normalizedLanmarks[i].y * landmarkSpaceOnImage + m_landmarkImagePadding;
			current.depth = m_normalizedLanmarks[i].depth;

			m_imageLandmarks[i] = current;
		}
	}
}
