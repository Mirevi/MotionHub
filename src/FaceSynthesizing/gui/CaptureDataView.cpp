#include "CaptureDataView.h"

namespace facesynthesizing::domain::adapters::gui {
	FaceSynthesizingGUITabType CaptureDataView::getTabType()
	{
		return FaceSynthesizingGUITabType::CAPTURE;
	}

	void CaptureDataView::setGUIEventForwarder(std::shared_ptr<GUIEventForwarder> guiEventForwarder)
	{
		this->guiEventForwarder = guiEventForwarder;
	}

	void CaptureDataView::setMessageViewModel(std::shared_ptr<MessageViewModel> messageViewModel)
	{
		this->messageViewModel = messageViewModel;
	}

	void CaptureDataView::setCaptureDataViewModel(std::shared_ptr<CaptureDataViewModel> captureDataViewModel)
	{
		this->captureDataViewModel = captureDataViewModel;
	}

}
