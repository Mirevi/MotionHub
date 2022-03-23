#include "GUIEventForwarder.h"

//TODO implement

namespace facesynthesizing::domain::adapters::gui {
	void GUIEventForwarder::dataAlreadySetPromtDone(usecases::DataAlreadyExistsResult result)
	{
		faceSynthesizing->setDataAlreadyExistsPromptResult(result);
	}
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

	void GUIEventForwarder::requestAllCaptureNames()
	{
		faceSynthesizing->asyncGUIRequest(usecases::GUIRequest::All_Capture_Names);
	}
	void GUIEventForwarder::requestAllTrainingDatasetNames()
	{
		faceSynthesizing->asyncGUIRequest(usecases::GUIRequest::All_Training_Dataset_Names);
	}
	void GUIEventForwarder::requestAllCheckpointNames()
	{
		faceSynthesizing->asyncGUIRequest(usecases::GUIRequest::All_Checkpoint_Names);
	}

	void GUIEventForwarder::setFaceSynthesizingGUIInteractor(std::shared_ptr<usecases::FaceSynthesizingGUIInteractor> faceSynthesizing)
	{
		this->faceSynthesizing = faceSynthesizing;
	}
	void GUIEventForwarder::setCaptureDataViewModel(std::shared_ptr<CaptureDataViewModel> captureDataViewModel) {
		this->captureDataViewModel = captureDataViewModel;
	}
	void GUIEventForwarder::setConvertDataViewModel(std::shared_ptr<ConvertDataViewModel> convertDataViewModel) {
		this->convertDataViewModel = convertDataViewModel;
	}
	void GUIEventForwarder::setTrainingViewModel(std::shared_ptr<TrainingViewModel> trainingViewModel) {
		this->trainingViewModel = trainingViewModel;
	}/*
	void GUIEventForwarder::setInferenceViewModel(std::shared_ptr<InferenceViewModel> inferenceViewModel) {
		this->inferenceViewModel = inferenceViewModel;
	}*/
}
