#pragma once

#include "Constructor.h"
#include "../usecases/FaceSynthesizingGUIInteractor.h"
#include "../gui/GUIPresenter.h"
#include "../gui/GUIEventForwarder.h"
#include "../gui/GUITabViewModel.h"
#include "../gui/GUIMessageViewModel.h"
#include "../gui/GUICaptureDataViewModel.h"
#include "../gui/GUIConvertDataViewModel.h"
#include "../gui/GUITrainingViewModel.h"
//#include "../gui/GUIInferenceViewModel.h"
#include "../gui/GUITabView.h"
#include "../gui/GUICaptureDataView.h"
#include "../gui/GUIConvertDataView.h"
#include "../gui/GUITrainingView.h"
//#include "../gui/GUIInferenceView.h"

#include <memory>

namespace usecases = facesynthesizing::domain::usecases;
namespace gui = facesynthesizing::domain::adapters::gui;

namespace facesynthesizing::construction {
	struct GuiMainWindow {};  // Here to be overwritten by subclass

	class GUIConstructor : public Constructor {
	public:
		GUIConstructor();
		virtual void resolveDependencies();

		std::shared_ptr<gui::GUIPresenter> getPresenter();
		virtual std::shared_ptr<GuiMainWindow> getMainWindow() = 0;

		void addHigherLevelDependency(std::shared_ptr<usecases::FaceSynthesizingGUIInteractor> faceSynthesizerGUIInteractor);
	private:
		std::shared_ptr<gui::GUIPresenter> guiPresenter;
		std::shared_ptr<gui::GUIEventForwarder> guiEventForwarder;

	protected:
		// View Models
		std::shared_ptr<gui::TabViewModel> tabViewModel;
		std::shared_ptr<gui::MessageViewModel> messageViewModel;
		std::shared_ptr<gui::CaptureDataViewModel> captureDataViewModel;
		std::shared_ptr<gui::ConvertDataViewModel> convertDataViewModel;
		std::shared_ptr<gui::TrainingViewModel> trainingViewModel;
		//std::shared_ptr<gui::InferenceViewModel> inferenceViewModel;
		
		// Views
		std::shared_ptr<gui::TabView> tabView;
		std::shared_ptr<gui::CaptureDataView> captureDataView;
		std::shared_ptr<gui::ConvertDataView> convertDataView;
		std::shared_ptr<gui::TrainingView> trainingView;
		//std::shared_ptr<gui::InferenceDataView> inferenceView;
	};
}
