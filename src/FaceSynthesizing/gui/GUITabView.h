#pragma once

#include "GUIView.h"
#include "GUIEventForwarder.h"
#include "GUITabViewModel.h"
#include "GUICaptureDataView.h"
#include "GUIConvertDataView.h"
#include "GUITrainingView.h"

#include <memory>

namespace usecases = facesynthesizing::domain::usecases;

namespace facesynthesizing::domain::adapters::gui {
	class TabView : public View {
	public:
		~TabView();

		//construction purpose
		void setGUIEventForwarder(std::shared_ptr<GUIEventForwarder> guiEventForwarder);
		void setTabViewModel(std::shared_ptr<TabViewModel> tabViewModel);
		void setCaptureDataView(std::shared_ptr<CaptureDataView> captureDataView);
		void setConvertDataView(std::shared_ptr<ConvertDataView> convertDataView);
		void setTrainingView(std::shared_ptr<TrainingView> trainingView);
		//void setInferenceDataView(std::shared_ptr<InferenceDataView> inferenceView);
	protected:
		std::shared_ptr<GUIEventForwarder> guiEventForwarder;
		std::shared_ptr<TabViewModel> tabViewModel;

		// Tabs
		std::shared_ptr<gui::CaptureDataView> captureDataView;
		std::shared_ptr<gui::ConvertDataView> convertDataView;
		std::shared_ptr<gui::TrainingView> trainingView;
		//std::shared_ptr<gui::InferenceDataView> inferenceView;
	};
}