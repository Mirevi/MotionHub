#pragma once

#include "View.h"
#include "CaptureDataViewModel.h"
#include "MessageViewModel.h"
#include "GUIEventForwarder.h"
#include "GUITab.h"

#include <memory>

namespace facesynthesizing::domain::adapters::gui {
	class CaptureDataView: public View, public GUITab {
	public:
		FaceSynthesizingGUITabType getTabType();
		//construction purpose
		void setGUIEventForwarder(std::shared_ptr<GUIEventForwarder> guiEventForwarder);
		void setMessageViewModel(std::shared_ptr<MessageViewModel> messageViewModel);
		void setCaptureDataViewModel(std::shared_ptr<CaptureDataViewModel> captureDataViewModel);
	protected:
		std::shared_ptr<GUIEventForwarder> guiEventForwarder;
		std::shared_ptr<MessageViewModel> messageViewModel;
		std::shared_ptr<CaptureDataViewModel> captureDataViewModel;
	};
}