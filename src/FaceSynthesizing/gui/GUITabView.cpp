#include "GUITabView.h"

namespace facesynthesizing::domain::adapters::gui {
	TabView::~TabView()
	{
		tabViewModel->detachListener(this);
	}
	void TabView::setGUIEventForwarder(std::shared_ptr<GUIEventForwarder> guiEventForwarder)
	{
		this->guiEventForwarder = guiEventForwarder;
	}

	void TabView::setTabViewModel(std::shared_ptr<TabViewModel> tabViewModel)
	{
		this->tabViewModel = tabViewModel;
	}

	void TabView::setCaptureDataView(std::shared_ptr<CaptureDataView> captureDataView)
	{
		this->captureDataView = captureDataView;
	}

	void TabView::setConvertDataView(std::shared_ptr<ConvertDataView> convertDataView)
	{
		this->convertDataView = convertDataView;
	}
	void TabView::setTrainingView(std::shared_ptr<TrainingView> trainingView)
	{
		this->trainingView = trainingView;
	}
}
