#pragma once

#include "GUIViewModel.h"
#include "../usecases/FaceSynthesizingEnums.h"
#include "../usecases/FaceSynthesizingDataStructures.h"

#include <memory>

namespace usecases = facesynthesizing::domain::usecases;

namespace facesynthesizing::domain::adapters::gui {
	class CaptureDataViewModel : public ViewModel {
	public:
		// Configuration
		bool isConfigurationActivated = true;
		std::string name = "new_data";
		int train_images = 1500;
		int evaluation_images = 250;
		usecases::BoundingBoxAlgorithm boundingBoxAlgorithm = usecases::BoundingBoxAlgorithm::SynergyNet;
		usecases::FaceAlignmentAlgorithm faceAlignmentAlgorithm = usecases::FaceAlignmentAlgorithm::SynergyNet;
		float maxPitch = 10.0;
		float maxRoll = 10.0;
		float maxYaw = 10.0;

		// Tasks
		bool isInitializeButtonActivated = true;
		bool isCaptureButtonActivated = false;
		bool isCancelButtonActivated = false;

		// Visualization
		bool visualize = true;
		std::shared_ptr<usecases::Image> colorImage = nullptr;
		std::shared_ptr<usecases::Image> faceAlignmentImage = nullptr;
	};

	const inline std::shared_ptr<usecases::CaptureDataInformation> captureDataInfoFromModel(CaptureDataViewModel* model) {
		auto infos = std::make_shared<usecases::CaptureDataInformation>();
		infos->name = model->name;
		infos->train_images = model->train_images;
		infos->evaluation_images = model->evaluation_images;
		infos->boundingBoxAlgorithm = model->boundingBoxAlgorithm;
		infos->faceAlignmentAlgorithm = model->faceAlignmentAlgorithm;
		infos->maxPitch = model->maxPitch;
		infos->maxRoll = model->maxRoll;
		infos->maxYaw = model->maxYaw;

		return infos;
	}
}

