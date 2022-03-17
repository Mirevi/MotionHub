#include "GUIPresenter.h"

#include <iostream>

namespace facesynthesizing::domain::adapters::gui {
	void GUIPresenter::clearImage(usecases::ImageType imageType)
	{
		switch (imageType) {
		case usecases::ImageType::Camera_Color:
			captureDataViewModel->colorImage = nullptr;
			break;
		case usecases::ImageType::Capture_FaceAlignment:
			captureDataViewModel->faceAlignmentImage = nullptr;
			break;
		}
		notifyImageChange(imageType);
	}
	void GUIPresenter::visualizeImage(std::shared_ptr<usecases::Image> image)
	{
		// set
		switch (image->type) {
		case usecases::ImageType::Camera_Color:
			captureDataViewModel->colorImage = image;
			break;
		case usecases::ImageType::Capture_FaceAlignment:
			captureDataViewModel->faceAlignmentImage = image;
			break;
		}
		notifyImageChange(image->type);
	}
	void GUIPresenter::notifyImageChange(usecases::ImageType imageType)
	{
		// notify change listener
		switch (imageType) {
		case usecases::ImageType::Camera_Color:
		case usecases::ImageType::Capture_FaceAlignment:
			captureDataViewModel->notify();
			break;
		}
	}

	void GUIPresenter::cameraInitializationStarted()
	{
		switch (tabViewModel->currentTab) {
		case FaceSynthesizingGUITabType::CAPTURE:
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
		case FaceSynthesizingGUITabType::CAPTURE:
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
	void GUIPresenter::endOfTask()
	{
		switch (tabViewModel->currentTab) {
		case FaceSynthesizingGUITabType::CAPTURE:
			endOfDataCaptureTask();
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
	/*void GUIPresenter::setConvertDataViewModel(std::shared_ptr<ConvertDataViewModel> convertDataViewModel) {
		this->convertDataViewModel = convertDataViewModel;
	}
	void GUIPresenter::setTrainViewModel(std::shared_ptr<TrainViewModel> trainViewModel) {
		this->trainViewModel = trainViewModel;
	}
	void GUIPresenter::setInferenceViewModel(std::shared_ptr<InferenceViewModel> inferenceViewModel) {
		this->inferenceViewModel = inferenceViewModel;
	}*/
}