#include "GUICaptureDataView.h"

namespace facesynthesizing::domain::adapters::gui {
	CaptureDataView::~CaptureDataView()
	{
		messageViewModel->detachListener(this);
		captureDataViewModel->detachListener(this);
	}
	GUITabType CaptureDataView::getTabType()
	{
		return GUITabType::CAPTURE;
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
