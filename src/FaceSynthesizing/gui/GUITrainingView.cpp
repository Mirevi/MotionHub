#include "GUITrainingView.h"

#include<iostream>

namespace facesynthesizing::domain::adapters::gui {
	TrainingView::~TrainingView()
	{
		messageViewModel->detachListener(this);
		trainingViewModel->detachListener(this);
	}
	GUITabType TrainingView::getTabType()
	{
		return GUITabType::TRAINING;
	}
	void TrainingView::setGUIEventForwarder(std::shared_ptr<GUIEventForwarder> guiEventForwarder)
	{
		this->guiEventForwarder = guiEventForwarder;
	}
	void TrainingView::setTabViewModel(std::shared_ptr<TabViewModel> tabViewModel)
	{
		this->tabViewModel = tabViewModel;
	}
	void TrainingView::setMessageViewModel(std::shared_ptr<MessageViewModel> messageViewModel)
	{
		this->messageViewModel = messageViewModel;
	}
	void TrainingView::setTrainingViewModel(std::shared_ptr<TrainingViewModel> trainingViewModel)
	{
		this->trainingViewModel = trainingViewModel;
	}
}