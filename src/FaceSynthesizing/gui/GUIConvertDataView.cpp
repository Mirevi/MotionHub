#include "GUIConvertDataView.h"

namespace facesynthesizing::domain::adapters::gui {
	ConvertDataView::~ConvertDataView()
	{
		messageViewModel->detachListener(this);
		convertDataViewModel->detachListener(this);
	}
	GUITabType ConvertDataView::getTabType()
	{
		return GUITabType::CONVERT;
	}

	void ConvertDataView::setGUIEventForwarder(std::shared_ptr<GUIEventForwarder> guiEventForwarder)
	{
		this->guiEventForwarder = guiEventForwarder;
	}

	void ConvertDataView::setTabViewModel(std::shared_ptr<TabViewModel> tabViewModel)
	{
		this->tabViewModel = tabViewModel;
	}

	void ConvertDataView::setMessageViewModel(std::shared_ptr<MessageViewModel> messageViewModel)
	{
		this->messageViewModel = messageViewModel;
	}

	void ConvertDataView::setConvertDataViewModel(std::shared_ptr<ConvertDataViewModel> convertDataViewModel)
	{
		this->convertDataViewModel = convertDataViewModel;
	}
}