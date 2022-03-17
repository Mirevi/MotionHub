#include "GUIEventForwarder.h"

//TODO implement

namespace facesynthesizing::domain::adapters::gui {
	void GUIEventForwarder::initializeCamera()
	{
		faceSynthesizing->startInitializeCameraTask();
	}
	void GUIEventForwarder::startCaptureData()
	{
		auto infos = captureDataInfoFromModel(captureDataViewModel.get());
		faceSynthesizing->startCaptureDataTask(infos);
	}

	void GUIEventForwarder::cancel()
	{
		faceSynthesizing->cancelTask();
	}

	void GUIEventForwarder::setFaceSynthesizingGUIInteractor(std::shared_ptr<usecases::FaceSynthesizingGUIInteractor> faceSynthesizing)
	{
		this->faceSynthesizing = faceSynthesizing;
	}

	void GUIEventForwarder::setCaptureDataViewModel(std::shared_ptr<CaptureDataViewModel> captureDataViewModel) {
		this->captureDataViewModel = captureDataViewModel;
	}
	/*void GUIEventForwarder::setConvertDataViewModel(std::shared_ptr<ConvertDataViewModel> convertDataViewModel) {
		this->convertDataViewModel = convertDataViewModel;
	}
	void GUIEventForwarder::setTrainViewModel(std::shared_ptr<TrainViewModel> trainViewModel) {
		this->trainViewModel = trainViewModel;
	}
	void GUIEventForwarder::setInferenceViewModel(std::shared_ptr<InferenceViewModel> inferenceViewModel) {
		this->inferenceViewModel = inferenceViewModel;
	}*/
}
