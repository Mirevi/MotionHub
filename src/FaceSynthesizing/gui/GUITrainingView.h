#pragma once

#include "GUIView.h"
#include "GUITab.h"
#include "GUIEventForwarder.h"
#include "GUITabViewModel.h"
#include "GUIMessageViewModel.h"
#include "GUITrainingViewModel.h"

namespace facesynthesizing::domain::adapters::gui {
	class TrainingView : public View, public GUITab {
	public:
		~TrainingView();
		GUITabType getTabType();

		//construction purpose
		void setGUIEventForwarder(std::shared_ptr<GUIEventForwarder> guiEventForwarder);
		void setTabViewModel(std::shared_ptr<TabViewModel> tabViewModel);
		void setMessageViewModel(std::shared_ptr<MessageViewModel> messageViewModel);
		void setTrainingViewModel(std::shared_ptr<TrainingViewModel> trainingViewModel);
	protected:
		std::shared_ptr<GUIEventForwarder> guiEventForwarder;
		std::shared_ptr<TabViewModel> tabViewModel;
		std::shared_ptr<MessageViewModel> messageViewModel;
		std::shared_ptr<TrainingViewModel> trainingViewModel;
	};
}