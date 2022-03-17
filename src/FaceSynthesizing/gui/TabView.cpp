#include "TabView.h"

//TODO implement

void facesynthesizing::domain::adapters::gui::TabView::setTabViewModel(std::shared_ptr<TabViewModel> tabViewModel)
{
	this->tabViewModel = tabViewModel;
}

void facesynthesizing::domain::adapters::gui::TabView::setCaptureDataView(std::shared_ptr<CaptureDataView> captureDataView)
{
	this->captureDataView = captureDataView;
}
