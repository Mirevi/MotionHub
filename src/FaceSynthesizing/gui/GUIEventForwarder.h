#pragma once

#include "../usecases/FaceSynthesizingGUIInteractor.h"
#include "GUICaptureDataViewModel.h"
#include "GUIConvertDataViewModel.h"
#include "GUITrainingViewModel.h"

#include <memory>

namespace usecases = facesynthesizing::domain::usecases;

namespace facesynthesizing::domain::adapters::gui {
	class GUIEventForwarder {
	public:
		void dataAlreadySetPromtDone(usecases::DataAlreadyExistsResult result);
		void initializeCamera();
		void startCaptureData();
		void cancel();

		void requestAllCaptureNames();
		void requestAllTrainingDatasetNames();
		void requestAllCheckpointNames();

		// Construction purpose
		void setFaceSynthesizingGUIInteractor(std::shared_ptr<usecases::FaceSynthesizingGUIInteractor> faceSynthesizing);
		void setCaptureDataViewModel(std::shared_ptr<CaptureDataViewModel> captureDataViewModel);
		void setConvertDataViewModel(std::shared_ptr<ConvertDataViewModel> convertDataViewModel);
		void setTrainingViewModel(std::shared_ptr<TrainingViewModel> trainingViewModel);
		//void setInferenceViewModel(std::shared_ptr<InferenceViewModel> inferenceViewModel);
	private:
		std::shared_ptr<usecases::FaceSynthesizingGUIInteractor> faceSynthesizing;
		std::shared_ptr<CaptureDataViewModel> captureDataViewModel;
		std::shared_ptr<gui::ConvertDataViewModel> convertDataViewModel;
		std::shared_ptr<gui::TrainingViewModel> trainingViewModel;
		//std::shared_ptr<gui::InferenceViewModel> inferenceViewModel;
	};
}


//TODO implement