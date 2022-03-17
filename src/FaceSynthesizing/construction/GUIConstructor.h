#pragma once

#include "Constructor.h"
#include "../usecases/FaceSynthesizingGUIInteractor.h"
#include "../gui/GUIPresenter.h"
#include "../gui/GUIEventForwarder.h"
#include "../gui/TabViewModel.h"
#include "../gui/MessageViewModel.h"
#include "../gui/CaptureDataViewModel.h"
//#include "../gui/ConvertDataViewModel.h"
//#include "../gui/TrainViewModel.h"
//#include "../gui/InferenceViewModel.h"
#include "../gui/TabView.h"
#include "../gui/CaptureDataView.h"
//#include "../gui/ConvertDataView.h"
//#include "../gui/TrainView.h"
//#include "../gui/InferenceView.h"

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
		//std::shared_ptr<gui::ConvertDataViewModel> convertDataViewModel;
		//std::shared_ptr<gui::TrainViewModel> trainViewModel;
		//std::shared_ptr<gui::InferenceViewModel> inferenceViewModel;
		
		// Views
		std::shared_ptr<gui::TabView> tabView;
		std::shared_ptr<gui::CaptureDataView> captureDataView;
		//std::shared_ptr<gui::ConvertDataView> convertDataView;
		//std::shared_ptr<gui::TrainView> trainView;
		//std::shared_ptr<gui::InferenceDataView> inferenceView;
	};
}
