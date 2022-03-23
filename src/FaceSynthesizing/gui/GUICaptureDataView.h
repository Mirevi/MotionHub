#pragma once

#include "GUIView.h"
#include "GUITab.h"
#include "GUIEventForwarder.h"
#include "GUICaptureDataViewModel.h"
#include "GUIMessageViewModel.h"

#include <memory>

namespace facesynthesizing::domain::adapters::gui {
	class CaptureDataView: public View, public GUITab {
	public:
		~CaptureDataView();
		GUITabType getTabType();

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