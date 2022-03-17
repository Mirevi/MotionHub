#include "AzureKinectCamera.h"

#include <iostream>

namespace facesynthesizing::infrastructure::azurekinect {
	AzureKinectCamera::AzureKinectCamera()
	{
		config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
		config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
		config.color_resolution = K4A_COLOR_RESOLUTION_3072P;
		config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
		config.camera_fps = K4A_FRAMES_PER_SECOND_15;
		config.synchronized_images_only = true;
		config.depth_delay_off_color_usec = 0;
		config.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;
		config.subordinate_delay_off_master_usec = 0;
		config.disable_streaming_indicator = false;

		capture = std::make_shared<k4a::capture>();
		determineDepthIlluminatedArea();
	}

	void AzureKinectCamera::determineDepthIlluminatedArea()
	{
		switch (config.color_resolution) {
		case K4A_COLOR_RESOLUTION_2160P:
			depthIlluminatedArea = usecases::Rect(900, 170, 2840 - 900, 2020 - 170);
			break;
		case K4A_COLOR_RESOLUTION_3072P:
			depthIlluminatedArea = usecases::Rect(1050, 400, 2950 - 1050, 2670 - 400);
			break;
		default:
			throw "Resolution not supported!";
		}
	}

	void AzureKinectCamera::initiate()
	{
		camera = k4a::device::open(K4A_DEVICE_DEFAULT);
		camera.start_cameras(&config);
		calibration = camera.get_calibration(config.depth_mode, config.color_resolution);
		transformation = k4a::transformation(calibration);
	}

	void AzureKinectCamera::close()
	{
		camera.close(); 
	}
	void AzureKinectCamera::captureImagePair()
	{
		camera.get_capture(capture.get());
	}
	std::shared_ptr<usecases::Image> AzureKinectCamera::getColorImage()
	{
		imageColor = std::make_shared<usecases::Image>();

		k4a::image k4aImage = capture->get_color_image();
		imageColor->width = k4aImage.get_width_pixels();
		imageColor->height = k4aImage.get_height_pixels();
		imageColor->channels = 4;
		imageColor->bytesPerChannelValue = 1;
		imageColor->type = usecases::ImageType::Camera_Color;
		std::size_t dataSize = imageColor->getDataSize();

		uint8_t* buffer = k4aImage.get_buffer();
		uint8_t* dataArray = new uint8_t[dataSize];
		std::copy(buffer, buffer + dataSize, dataArray);
		imageColor->data = std::shared_ptr<uint8_t[]>(dataArray, usecases::array_deleter<uint8_t>());
		
		return imageExcerpt(imageColor, depthIlluminatedArea);
	}
	std::shared_ptr<usecases::Image> AzureKinectCamera::getDepthImage()
	{
		imageDepth = std::make_shared<usecases::Image>();

		k4a::image k4aImage = capture->get_depth_image();
		k4a::image k4aImageTransformed = 
			k4a::image::create(K4A_IMAGE_FORMAT_DEPTH16,
			calibration.color_camera_calibration.resolution_width,
			calibration.color_camera_calibration.resolution_height,
			calibration.color_camera_calibration.resolution_width * static_cast<int>(sizeof(uint16_t)));
		transformation.depth_image_to_color_camera(k4aImage, &k4aImageTransformed);

		imageDepth->width = k4aImageTransformed.get_width_pixels();
		imageDepth->height = k4aImageTransformed.get_height_pixels();
		imageDepth->channels = 1;
		imageDepth->bytesPerChannelValue = 2;
		imageDepth->type = usecases::ImageType::Camera_Depth;
		std::size_t dataSize = imageDepth->getDataSize();

		uint8_t* buffer = k4aImageTransformed.get_buffer();
		uint8_t* dataArray = new uint8_t[dataSize];
		std::copy(buffer, buffer + dataSize, dataArray);
		imageDepth->data = std::shared_ptr<uint8_t[]>(dataArray, usecases::array_deleter<uint8_t>());

		return imageExcerpt(imageDepth, depthIlluminatedArea);
	}
	std::shared_ptr<usecases::Image> AzureKinectCamera::getInfraredImage()
	{
		imageInfrared = std::make_shared<usecases::Image>();

		k4a::image k4aIrImage = capture->get_ir_image();
		k4a::image k4aDepthImage = capture->get_depth_image();
		k4a::image k4aIrImageCustom = k4a::image::create_from_buffer(
			K4A_IMAGE_FORMAT_CUSTOM16,
			k4aIrImage.get_width_pixels(),
			k4aIrImage.get_height_pixels(),
			k4aIrImage.get_stride_bytes(),
			k4aIrImage.get_buffer(),
			k4aIrImage.get_height_pixels() * k4aIrImage.get_stride_bytes(),
			NULL,
			NULL);
		k4a::image k4aImage = transformation.depth_image_to_color_camera_custom(
			k4aDepthImage,
			k4aIrImageCustom,
			K4A_TRANSFORMATION_INTERPOLATION_TYPE_NEAREST,
			0).second;

		imageInfrared->width = k4aImage.get_width_pixels();
		imageInfrared->height = k4aImage.get_height_pixels();
		imageInfrared->channels = 1;
		imageInfrared->bytesPerChannelValue = 2;
		imageInfrared->type = usecases::ImageType::Camera_Infrared;
		std::size_t dataSize = imageInfrared->getDataSize();

		uint8_t* buffer = k4aImage.get_buffer();
		uint8_t* dataArray = new uint8_t[dataSize];
		std::copy(buffer, buffer + dataSize, dataArray);
		imageInfrared->data = std::shared_ptr<uint8_t[]>(dataArray, usecases::array_deleter<uint8_t>());

		return imageExcerpt(imageInfrared, depthIlluminatedArea);
	}

	std::shared_ptr<usecases::Image> AzureKinectCamera::imageExcerpt(std::shared_ptr<usecases::Image> image, usecases::Rect excerptArea)
	{
		std::size_t pixelBytes = image->channels * image->bytesPerChannelValue;
		std::size_t imageWidthBytes = image->width * pixelBytes;
		std::size_t excerptWidthBytes = excerptArea.width * pixelBytes;
		std::size_t startIndexX = excerptArea.x * pixelBytes;

		std::shared_ptr<usecases::Image> imageExcerpt = std::make_shared<usecases::Image>();
		imageExcerpt->width = excerptArea.width;
		imageExcerpt->height = excerptArea.height;
		imageExcerpt->channels = image->channels;
		imageExcerpt->bytesPerChannelValue = image->bytesPerChannelValue;
		imageExcerpt->type = image->type;

		uint8_t* dataArray = new uint8_t[imageExcerpt->getDataSize()];
		for (std::size_t y = 0; y < excerptArea.height; y++) {
			for (std::size_t x = 0; x < excerptWidthBytes; x++) {
				dataArray[y * excerptWidthBytes + x] = image->data[(excerptArea.y + y) * imageWidthBytes + startIndexX + x];
			}
		}
		imageExcerpt->data = std::shared_ptr<uint8_t[]>(dataArray, usecases::array_deleter<uint8_t>());

		return imageExcerpt;
	}
}