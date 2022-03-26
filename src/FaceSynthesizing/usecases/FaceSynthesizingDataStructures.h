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
	};

	struct ConvertDataInformation {
		bool continueProcess;
		bool overwrite;
		std::string name;
		float horizontalFoV;
		int outputImageSize;
		int padding = 20;
		float pcaLandmarkComponents;
		float pcaImageComponents;
		int flmLineThickness;
		EyeTrackingAlgorithm eyeTrackingAlgorithm;
		FaceTrackingAlgorithm faceTrackingAlgorithm;
		int eyeTrackingStride;
		int eyeTrackingThreshold;
		int faceDepth;
		int depthPadding;
		bool useDepthFilling;
		DepthFillingAlgorithm depthFillingAlgorithm;
		int depthFillingImageSize;
		int depthFillingPatchSize;
		int depthFillingExtendedPatchSize;
		int depthFillingPadding;
		int depthFillingSourceAmount;
		bool depthFillingUseBlur;
		int depthFillingBlurKernelSize;
	};

	struct TrainingInformation {
		std::string name;
		bool continueProcess;
		bool overwrite;
		std::string datasetName;
		ColorFormat colorFormat;
		int continueEpoch;
		int trainingBatchSize;
		int evaluationBatchSize;
		int datasetImageSize;
		int maxDatasetSize;
		bool useSerialBatches;
		bool useDepthMask;
		ModelType modelType;
		GeneratorArchitecture generatorArchitecture;
		DiscriminatorArchitecture discriminatorArchitecture;
		GANMode ganMode;
		int generatorFilters;
		int discriminatorFilters;
		NormalizationLayerType normalizationLayerType;
		bool useDropout;
		int discriminators;
		int discriminatorLayers;
		bool useMappingNetwork;
		MappingNetwork mappingNetwork;
		int lrConsistentEpochs;
		int lrDecayingEpochs;
		float beta1;
		float learningRate;
		LearningRatePolicy learningRatePolicy;
		int stepDecayIterations;
		WeightInitType weightInitType;
		float weightInitGain;
		bool useL1Loss;
		bool useFeatureMatchingLoss;
		bool useLPIPSLoss;
		bool useCycleLoss;
		float lambdaL1;
		float lambdaFeatureMatching;
		float lambdaLPIPS;
		float lambdaCycleForward;
		float lambdaCycleBackward;
		int checkpointFrequency;
		int evaluationFrequency;
		int pixelAccuracyMetricThreshold;
		std::string visdomEnvironment;
		int visdomImageColumns;
		bool useVisdom;
		int visdomServerPort;
		std::string visdomServerURL;
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