#pragma once

#include "View.h"
#include "TabViewModel.h"
#include "CaptureDataView.h"

#include <memory>

namespace facesynthesizing::domain::adapters::gui {
	class TabView : public View {
	public:
		//construction purpose
		void setTabViewModel(std::shared_ptr<TabViewModel> tabViewModel);
		void setCaptureDataView(std::shared_ptr<CaptureDataView> captureDataView);
		//void setConvertDataView(std::shared_ptr<ConvertDataView> convertDataView);
		//void setTrainView(std::shared_ptr<TrainView> trainView);
		//void setInferenceDataView(std::shared_ptr<InferenceDataView> inferenceView);
	protected:
		std::shared_ptr<TabViewModel> tabViewModel;

		// Tabs
		std::shared_ptr<gui::CaptureDataView> captureDataView;
		//std::shared_ptr<gui::ConvertDataView> convertDataView;
		//std::shared_ptr<gui::TrainView> trainView;
		//std::shared_ptr<gui::InferenceDataView> inferenceView;
	};
}