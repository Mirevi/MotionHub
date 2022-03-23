#pragma once

#include "GUIView.h"
#include "GUIEventForwarder.h"
#include "GUITab.h"
#include "GUIConvertDataViewModel.h"
#include "GUITabViewModel.h"
#include "GUIMessageViewModel.h"

#include <memory>

namespace facesynthesizing::domain::adapters::gui {
	class ConvertDataView : public View, public GUITab {
	public:
		~ConvertDataView();
		GUITabType getTabType();

		//construction purpose
		void setGUIEventForwarder(std::shared_ptr<GUIEventForwarder> guiEventForwarder);
		void setTabViewModel(std::shared_ptr<TabViewModel> tabViewModel);
		void setMessageViewModel(std::shared_ptr<MessageViewModel> messageViewModel);
		void setConvertDataViewModel(std::shared_ptr<ConvertDataViewModel> convertDataViewModel);
	protected:
		std::shared_ptr<GUIEventForwarder> guiEventForwarder;
		std::shared_ptr<TabViewModel> tabViewModel;
		std::shared_ptr<MessageViewModel> messageViewModel;
		std::shared_ptr<ConvertDataViewModel> convertDataViewModel;
	};
}
