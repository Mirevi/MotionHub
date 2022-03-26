#pragma once

#include "GUIViewModel.h"
#include "../usecases/FaceSynthesizingEnums.h"
#include "../usecases/FaceSynthesizingDataStructures.h"

#include <memory>
#include <string>
#include <vector>

namespace usecases = facesynthesizing::domain::usecases;

namespace facesynthesizing::domain::adapters::gui {
	class ConvertDataViewModel : public ViewModel {
	public:
		// Configuration
		bool isConfigurationActivated = true;
		
		// General Configuration
		std::vector<std::string> allNames;
		std::string name = "";
		float horizontalFoV = 90.0;
		int outputImageSize = 512;
		int padding = 20;
		float pcaLandmarkComponents = 0.9999;
		float pcaImageComponents = 0.9999;
		int flmLineThickness = 4;

		// Landmark Tracking Configuration
		usecases::EyeTrackingAlgorithm eyeTrackingAlgorithm = usecases::EyeTrackingAlgorithm::Gradient_Based;
		usecases::FaceTrackingAlgorithm faceTrackingAlgorithm = usecases::FaceTrackingAlgorithm::FAN;
		int eyeTrackingStride = 1;
		int eyeTrackingThreshold = 89;

		// Depth Conversion Configuration
		int faceDepth = 80;
		int depthPadding = 20;
		bool useDepthFilling = true;
		usecases::DepthFillingAlgorithm depthFillingAlgorithm = usecases::DepthFillingAlgorithm::FDCBOP;
		int depthFillingImageSize = 400;
		int depthFillingPatchSize = 17;
		int depthFillingExtendedPatchSize = 29;
		int depthFillingPadding = 150;
		int depthFillingSourceAmount = 10000;
		bool depthFillingUseBlur = true;
		int depthFillingBlurKernelSize = 9;

		// Tasks
		bool isConvertButtonActivated = true;
		bool isCancelButtonActivated = false;

		// Visualization
		bool visualize = true;
		std::shared_ptr<usecases::Image> firstImage = nullptr;
		std::shared_ptr<usecases::Image> secondImage = nullptr;
		std::shared_ptr<usecases::Image> thirdImage = nullptr;
	};

	const inline std::shared_ptr<usecases::ConvertDataInformation> convertDataInfoFromModel(ConvertDataViewModel* model) {
		auto infos = std::make_shared<usecases::ConvertDataInformation>();
		infos->name = model->name;
		infos->horizontalFoV = model->horizontalFoV;
		infos->outputImageSize = model->outputImageSize;
		infos->padding = model->padding;
		infos->pcaLandmarkComponents = model->pcaLandmarkComponents;
		infos->pcaImageComponents = model->pcaImageComponents;
		infos->flmLineThickness = model->flmLineThickness;
		infos->eyeTrackingAlgorithm = model->eyeTrackingAlgorithm;
		infos->faceTrackingAlgorithm = model->faceTrackingAlgorithm;
		infos->eyeTrackingStride = model->eyeTrackingStride;
		infos->eyeTrackingThreshold = model->eyeTrackingThreshold;
		infos->faceDepth = model->faceDepth;
		infos->depthPadding = model->depthPadding;
		infos->useDepthFilling = model->useDepthFilling;
		infos->depthFillingAlgorithm = model->depthFillingAlgorithm;
		infos->depthFillingImageSize = model->depthFillingImageSize;
		infos->depthFillingPatchSize = model->depthFillingPatchSize;
		infos->depthFillingExtendedPatchSize = model->depthFillingExtendedPatchSize;
		infos->depthFillingPadding = model->depthFillingPadding;
		infos->depthFillingSourceAmount = model->depthFillingSourceAmount;
		infos->depthFillingUseBlur = model->depthFillingUseBlur;
		infos->depthFillingBlurKernelSize = model->depthFillingBlurKernelSize;

		return infos;
	}
}

