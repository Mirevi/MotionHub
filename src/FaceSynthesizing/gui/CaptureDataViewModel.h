#pragma once

#include "ViewModel.h"
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

	const inline std::shared_ptr<usecases::CaptureDataInformation> captureDataInfoFromModel(CaptureDataViewModel* viewModel) {
		auto infos = std::make_shared<usecases::CaptureDataInformation>();
		infos->name = viewModel->name;
		infos->train_images = viewModel->train_images;
		infos->evaluation_images = viewModel->evaluation_images;
		infos->boundingBoxAlgorithm = viewModel->boundingBoxAlgorithm;
		infos->faceAlignmentAlgorithm = viewModel->faceAlignmentAlgorithm;
		infos->maxPitch = viewModel->maxPitch;
		infos->maxRoll = viewModel->maxRoll;
		infos->maxYaw = viewModel->maxYaw;
		infos->visualize = viewModel->visualize;

		return infos;
	}
}

