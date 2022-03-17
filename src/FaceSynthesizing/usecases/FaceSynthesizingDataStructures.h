#pragma once

#include "FaceSynthesizingEnums.h"

#include <string>
#include <memory>

namespace facesynthesizing::domain::usecases {
	struct Rect {
		Rect() : x(0), y(0), width(0), height(0) {};
		Rect(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {};
		int x;
		int y;
		int width;
		int height;
	};

	struct Image {
		std::shared_ptr<uint8_t[]> data;
		unsigned int width;
		unsigned int height;
		unsigned int channels;
		unsigned int bytesPerChannelValue;
		ImageType type = ImageType::Unknown;

		std::size_t getDataSize() {
			return this->width * this->height * this->channels * this->bytesPerChannelValue;
		};
	};

	template< typename T >
	struct array_deleter
	{
		void operator ()(T const* p)
		{
			delete[] p;
		}
	};

	struct CaptureDataInformation {
		std::string name;
		int train_images;
		int evaluation_images;
		BoundingBoxAlgorithm boundingBoxAlgorithm;
		FaceAlignmentAlgorithm faceAlignmentAlgorithm;
		float maxPitch;
		float maxRoll;
		float maxYaw;
		bool visualize;
	};

	struct CaptureDataPair {
		CaptureDataPair() {};
		CaptureDataPair(
			std::shared_ptr<Image> colorImage,
			std::shared_ptr<Image> depthImage,
			std::shared_ptr<Image> infraredImage
		) : colorImage(colorImage), depthImage(depthImage), infraredImage(infraredImage) {};

		std::shared_ptr<Image> colorImage;
		std::shared_ptr<Image> depthImage;
		std::shared_ptr<Image> infraredImage;
	};
}