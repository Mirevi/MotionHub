#include "GUIPresenter.h"

#include <iostream>

namespace facesynthesizing::domain::adapters::gui {
	void GUIPresenter::clearImage(usecases::ImageType imageType)
	{
		processImageChange(imageType, nullptr);
	}
	void GUIPresenter::visualizeImage(std::shared_ptr<usecases::Image> image)
	{
		processImageChange(image->type, image);
	}
	void GUIPresenter::processImageChange(usecases::ImageType imageType, std::shared_ptr<usecases::Image> image)
	{
		switch (imageType) {
		case usecases::ImageType::Camera_Color:
			captureDataViewModel->colorImage = image;
			break;
		case usecases::ImageType::Capture_FaceAlignment:
			captureDataViewModel->faceAlignmentImage = image;
			break;
		case usecases::ImageType::Convert_Face_Bounding_box:
			convertDataViewModel->firstImage = image;
			break;
		case usecases::ImageType::Convert_Landmarks:
			convertDataViewModel->secondImage = image;
			break;
		case usecases::ImageType::Convert_Depth_Holes:
			convertDataViewModel->thirdImage = image;
			break;
		case usecases::ImageType::Dataset_Color:
			if (tabViewModel->currentTab == GUITabType::CONVERT)
				convertDataViewModel->firstImage = image;
			else if (tabViewModel->currentTab == GUITabType::TRAINING)
				trainingViewModel->colorRealImage = image;
			break;
		case usecases::ImageType::Dataset_Depth:
			if (tabViewModel->currentTab == GUITabType::CONVERT)
				convertDataViewModel->secondImage = image;
			else if (tabViewModel->currentTab == GUITabType::TRAINING)
				trainingViewModel->depthRealImage = image;
			break;
		case usecases::ImageType::Dataset_FLM:
			if (tabViewModel->currentTab == GUITabType::CONVERT)
				convertDataViewModel->thirdImage = image;
			else if (tabViewModel->currentTab == GUITabType::TRAINING)
				trainingViewModel->flmImage = image;
			break;
		case usecases::ImageType::Synthesized_Color:
			trainingViewModel->colorFakeImage = image;
			break;
		case usecases::ImageType::Synthesized_Depth:
			trainingViewModel->depthFakeImage = image;
			break;
		default:
			return;
		}
		notifyImageChange(imageType);
	}
	void GUIPresenter::notifyImageChange(usecases::ImageType imageType)
	{
		// notify change listener
		switch (imageType) {
		case usecases::ImageType::Camera_Color:
		case usecases::ImageType::Capture_FaceAlignment:
			captureDataViewModel->notify();
			break;
		case usecases::ImageType::Convert_Face_Bounding_box:
		case usecases::ImageType::Convert_Landmarks:
		case usecases::ImageType::Convert_Depth_Holes:
			convertDataViewModel->notify();
			break;
		case usecases::ImageType::Dataset_Color:
		case usecases::ImageType::Dataset_Depth:
		case usecases::ImageType::Dataset_FLM:
			viewDataModelFromCurrentTab()->notify();
			break;
		case usecases::ImageType::Synthesized_Color:
		case usecases::ImageType::Synthesized_Depth:
			trainingViewModel->notify();
			break;
		}
	}
	std::shared_ptr<ViewModel> GUIPresenter::viewDataModelFromCurrentTab()
	{
		switch (tabViewModel->currentTab) {
		case GUITabType::CAPTURE:
			return captureDataViewModel;
		case GUITabType::CONVERT:
			return convertDataViewModel;
		case GUITabType::TRAINING:
			return trainingViewModel;
		default:
			throw std::exception("Unknown Data View Model");
		}
	}

	void GUIPresenter::cameraInitializationStarted()
	{
		switch (tabViewModel->currentTab) {
		case GUITabType::CAPTURE:
			captureCameraInitializationStarted();
			break;
		}
	}
	void GUIPresenter::captureCameraInitializationStarted()
	{
		lockCurrentView();

		captureDataViewModel->isConfigurationActivated = true;
		captureDataViewModel->isInitializeButtonActivated = false;
		captureDataViewModel->isCaptureButtonActivated = false;
		captureDataViewModel->isCancelButtonActivated = true;
		captureDataViewModel->notify();
	}
	void GUIPresenter::cameraIsInitialized()
	{
		switch (tabViewModel->currentTab) {
		case GUITabType::CAPTURE:
			captureCameraIsInitialized();
			break;
		}
	}
	void GUIPresenter::captureCameraIsInitialized()
	{
		lockCurrentView();

		captureDataViewModel->isConfigurationActivated = true;
		captureDataViewModel->isInitializeButtonActivated = false;
		captureDataViewModel->isCaptureButtonActivated = true;
		captureDataViewModel->isCancelButtonActivated = true;
		captureDataViewModel->notify();
	}
	void GUIPresenter::dataPairCaptureStarted()
	{
		lockCurrentView();

		captureDataViewModel->isConfigurationActivated = false;
		captureDataViewModel->isInitializeButtonActivated = false;
		captureDataViewModel->isCaptureButtonActivated = false;
		captureDataViewModel->isCancelButtonActivated = true;
		captureDataViewModel->notify();
	}
	void GUIPresenter::dataPairConversionStarted()
	{
		lockCurrentView();

		convertDataViewModel->isConfigurationActivated = false;
		convertDataViewModel->isConvertButtonActivated = false;
		convertDataViewModel->isCancelButtonActivated = true;
		convertDataViewModel->notify();
	}
	void GUIPresenter::trainingStarted()
	{
		lockCurrentView();

		trainingViewModel->isConfigurationActivated = false;
		trainingViewModel->isTrainingButtonActivated = false;
		trainingViewModel->isCancelButtonActivated = true;
		trainingViewModel->notify();
	}
	void GUIPresenter::endOfTask()
	{
		switch (tabViewModel->currentTab) {
		case GUITabType::CAPTURE:
			endOfDataCaptureTask();
			break;
		case GUITabType::CONVERT:
			endOfDataConvertTask();
			break;
		case GUITabType::TRAINING:
			endOfTrainingTask();
			break;
		}
	}
	void GUIPresenter::endOfDataCaptureTask()
	{
		unlockCurrentView();

		captureDataViewModel->isConfigurationActivated = true;
		captureDataViewModel->isInitializeButtonActivated = true;
		captureDataViewModel->isCaptureButtonActivated = false;
		captureDataViewModel->isCancelButtonActivated = false;
		captureDataViewModel->notify();
	}
	void GUIPresenter::endOfDataConvertTask()
	{
		unlockCurrentView();

		convertDataViewModel->isConfigurationActivated = true;
		convertDataViewModel->isConvertButtonActivated = true;
		convertDataViewModel->isCancelButtonActivated = false;
		convertDataViewModel->notify();
	}
	void GUIPresenter::endOfTrainingTask()
	{
		unlockCurrentView();

		trainingViewModel->isConfigurationActivated = true;
		trainingViewModel->isTrainingButtonActivated = true;
		trainingViewModel->isCancelButtonActivated = false;
		trainingViewModel->notify();
	}
	void GUIPresenter::dataAlreadyExistsUserPrompt()
	{
		std::string message = getDataAlreadyExistsPromptMessage();

		tabViewModel->showDataAlreadyExistsUserPrompt = true;
		tabViewModel->dialogMessage = message;
		tabViewModel->notify();
	}
	std::string GUIPresenter::getDataAlreadyExistsPromptMessage()
	{
		std::string message = "";
		switch (tabViewModel->currentTab) {
		case GUITabType::CAPTURE:
			message = "Capture already exists! If you want to override the existing Data press 'Overwrite'. If you want to Continue the Capture Process press the 'Continue' Button.";
			break;
		case GUITabType::CONVERT:
			message = "Dataset already exists! If you want to override the existing Dataset press 'Overwrite'. If you want to Continue the Data Conversion Process press the 'Continue' Button.";
			break;
		case GUITabType::TRAINING:
			message = "Checkpoint already exists! If you want to override the existing Model press 'Overwrite'. If you want to Continue the Training Process press the 'Continue' Button.";
			break;
		}

		return message;
	}

	void GUIPresenter::lockCurrentView()
	{
		tabViewModel->isLocked = true;
		tabViewModel->notify();
	}
	void GUIPresenter::unlockCurrentView()
	{
		tabViewModel->isLocked = false;
		tabViewModel->notify();
	}

	void GUIPresenter::addNote(std::string message)
	{
		messageViewModel->notes.push_back(message);
		if (messageViewModel->notes.size() > 50)
			messageViewModel->notes.erase(messageViewModel->notes.begin());

		messageViewModel->notify();
	}
	void GUIPresenter::newStatusMessage(std::string message)
	{
		addNote(messageViewModel->statusMessage);
		updateStatusMessage(message);
	}
	void GUIPresenter::updateStatusMessage(std::string message)
	{
		messageViewModel->statusMessage = message;
		messageViewModel->notify();
	}
	void GUIPresenter::statusMessageToNotes()
	{
		newStatusMessage("");
	}

	void GUIPresenter::presentAllCaptureNames(std::vector<std::string> allNames)
	{
		convertDataViewModel->allNames = allNames;
		if (convertDataViewModel->allNames.size() > 0 && convertDataViewModel->name.empty())
			convertDataViewModel->name = allNames[0];

		convertDataViewModel->notify();
	}
	void GUIPresenter::presentTrainingDatasetNames(std::vector<std::string> allNames)
	{
		trainingViewModel->allDatasetNames = allNames;
		if (trainingViewModel->allDatasetNames.size() > 0 && trainingViewModel->datasetName.empty())
			trainingViewModel->datasetName = allNames[0];

		trainingViewModel->notify();
	}
	void GUIPresenter::presentAllCheckpointNames(std::vector<std::string> allNames)
	{
		//TODO implement when Inference Model exists
		/*inferenceViewModel->allCheckpointNames = allNames;
		if (inferenceViewModel->allCheckpointNames.size() > 0 && 
			inferenceViewModel->checkpointName.empty())
			inferenceViewModel->checkpointName = allNames[0];

		inferenceViewModel->notify();*/
	}

	void GUIPresenter::setTabViewModel(std::shared_ptr<TabViewModel> tabViewModel) {
		this->tabViewModel = tabViewModel;
	}
	void GUIPresenter::setMessageViewModel(std::shared_ptr<MessageViewModel> messageViewModel)
	{
		this->messageViewModel = messageViewModel;
	}
	void GUIPresenter::setCaptureDataViewModel(std::shared_ptr<CaptureDataViewModel> captureDataViewModel) {
		this->captureDataViewModel = captureDataViewModel;
	}
	void GUIPresenter::setConvertDataViewModel(std::shared_ptr<ConvertDataViewModel> convertDataViewModel)
	{
		this->convertDataViewModel = convertDataViewModel;
	}
	void GUIPresenter::setTrainingViewModel(std::shared_ptr<TrainingViewModel> trainingViewModel) {
		this->trainingViewModel = trainingViewModel;
	}/*
	void GUIPresenter::setInferenceViewModel(std::shared_ptr<InferenceViewModel> inferenceViewModel) {
		this->inferenceViewModel = inferenceViewModel;
	}*/
}