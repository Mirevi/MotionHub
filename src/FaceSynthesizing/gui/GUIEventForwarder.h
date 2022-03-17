#pragma once

#include "../usecases/FaceSynthesizingGUIInteractor.h"
#include "CaptureDataViewModel.h"

#include <memory>

namespace usecases = facesynthesizing::domain::usecases;

namespace facesynthesizing::domain::adapters::gui {
	class GUIEventForwarder {
	public:
		void initializeCamera();
		void startCaptureData();
		void cancel();

		// Construction purpose
		void setFaceSynthesizingGUIInteractor(std::shared_ptr<usecases::FaceSynthesizingGUIInteractor> faceSynthesizing);
		void setCaptureDataViewModel(std::shared_ptr<CaptureDataViewModel> captureDataViewModel);
		//void setConvertDataViewModel(std::shared_ptr<ConvertDataViewModel> convertDataViewModel);
		//void setTrainViewModel(std::shared_ptr<TrainViewModel> trainViewModel);
		//void setInferenceViewModel(std::shared_ptr<InferenceViewModel> inferenceViewModel);
	private:
		std::shared_ptr<usecases::FaceSynthesizingGUIInteractor> faceSynthesizing;
		std::shared_ptr<CaptureDataViewModel> captureDataViewModel;
		//std::shared_ptr<gui::ConvertDataViewModel> convertDataViewModel;
		//std::shared_ptr<gui::TrainViewModel> trainViewModel;
		//std::shared_ptr<gui::InferenceViewModel> inferenceViewModel;
	};
}


//TODO implement