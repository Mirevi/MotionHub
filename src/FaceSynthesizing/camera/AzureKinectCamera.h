#pragma once

#include "CameraRGBDI.h"

#include <memory>
#include <iterator>
#include <k4a/k4a.h>
#include <k4a/k4a.hpp>

namespace camera = facesynthesizing::domain::adapters::camera;

namespace facesynthesizing::infrastructure::azurekinect {
	class AzureKinectCamera : public camera::CameraRGBDI {
	public:
		AzureKinectCamera();
		void initiate();
		void close();
		void captureImagePair();
		std::shared_ptr<usecases::Image> getColorImage();
		std::shared_ptr<usecases::Image> getDepthImage();
		std::shared_ptr<usecases::Image> getInfraredImage();
	private:
		void determineDepthIlluminatedArea();
		std::shared_ptr<usecases::Image> imageExcerpt(
			std::shared_ptr<usecases::Image> image, usecases::Rect excerptArea);

		// Camera
		k4a::device camera;
		k4a_device_configuration_t config;
		k4a::calibration calibration;
		k4a::transformation transformation;
		usecases::Rect depthIlluminatedArea;

		// Images
		std::shared_ptr<k4a::capture> capture;
		std::shared_ptr<usecases::Image> imageColor;
		std::shared_ptr<usecases::Image> imageDepth;
		std::shared_ptr<usecases::Image> imageInfrared;
	};
}