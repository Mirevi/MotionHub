#pragma once

#include "../usecases/FaceSynthesizingGUIPresenter.h"
#include "GUITab.h"
#include "GUITabViewModel.h"
#include "GUIMessageViewModel.h"
#include "GUICaptureDataViewModel.h"
#include "GUIConvertDataViewModel.h"
#include "GUITrainingViewModel.h"

#include <memory>

namespace usecases = facesynthesizing::domain::usecases;

namespace facesynthesizing::domain::adapters::gui {
	class GUIPresenter: public usecases::FaceSynthesizingGUIPresenter {
	public:
		void clearImage(usecases::ImageType imageType);
		void visualizeImage(std::shared_ptr<usecases::Image> image);

		void cameraInitializationStarted();
		void cameraIsInitialized();
		void dataPairCaptureStarted();
		void dataPairConversionStarted();
		void trainingStarted();
		void endOfTask();
		void dataAlreadyExistsUserPrompt();

		void addNote(std::string message);
		void newStatusMessage(std::string message);
		void updateStatusMessage(std::string message);
		void statusMessageToNotes();

		void presentAllCaptureNames(std::vector<std::string> allNames);
		void presentTrainingDatasetNames(std::vector<std::string> allNames);
		void presentAllCheckpointNames(std::vector<std::string> allNames);

		//construction purpose
		void setTabViewModel(std::shared_ptr<TabViewModel> tabViewModel);
		void setMessageViewModel(std::shared_ptr<MessageViewModel> messageViewModel);
		void setCaptureDataViewModel(std::shared_ptr<CaptureDataViewModel> captureDataViewModel);
		void setConvertDataViewModel(std::shared_ptr<ConvertDataViewModel> convertDataViewModel);
		void setTrainingViewModel(std::shared_ptr<TrainingViewModel> trainingViewModel);
		//void setInferenceViewModel(std::shared_ptr<InferenceViewModel> inferenceViewModel);
	private:
		void processImageChange(usecases::ImageType imageType, std::shared_ptr<usecases::Image> image);
		void notifyImageChange(usecases::ImageType imageType);
		std::shared_ptr<ViewModel> viewDataModelFromCurrentTab();
		void captureCameraInitializationStarted();
		void captureCameraIsInitialized();
		void endOfDataCaptureTask();
		void endOfDataConvertTask();
		void endOfTrainingTask();
		std::string getDataAlreadyExistsPromptMessage();

		void lockCurrentView();
		void unlockCurrentView();

		std::shared_ptr<TabViewModel> tabViewModel;
		std::shared_ptr<MessageViewModel> messageViewModel;
		std::shared_ptr<CaptureDataViewModel> captureDataViewModel;
		std::shared_ptr<gui::ConvertDataViewModel> convertDataViewModel;
		std::shared_ptr<gui::TrainingViewModel> trainingViewModel;
		//std::shared_ptr<gui::InferenceViewModel> inferenceViewModel;
	};
}