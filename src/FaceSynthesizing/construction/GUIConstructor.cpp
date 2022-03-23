#include "GUIConstructor.h"

#include<iostream>

namespace facesynthesizing::construction {
	GUIConstructor::GUIConstructor()
	{
		guiPresenter = std::make_shared<gui::GUIPresenter>();
		guiEventForwarder = std::make_shared<gui::GUIEventForwarder>();

		tabViewModel = std::make_shared<gui::TabViewModel>();
		messageViewModel = std::make_shared<gui::MessageViewModel>();
		captureDataViewModel = std::make_shared<gui::CaptureDataViewModel>();
		convertDataViewModel = std::make_shared<gui::ConvertDataViewModel>();
		trainingViewModel = std::make_shared<gui::TrainingViewModel>();
		//inferenceViewModel = std::make_shared<gui::InferenceViewModel>();
	}

	void GUIConstructor::resolveDependencies()
	{
		// GUI Presenter
		guiPresenter->setTabViewModel(tabViewModel);
		guiPresenter->setMessageViewModel(messageViewModel);
		guiPresenter->setCaptureDataViewModel(captureDataViewModel);
		guiPresenter->setConvertDataViewModel(convertDataViewModel);
		guiPresenter->setTrainingViewModel(trainingViewModel);
		//guiPresenter->setInferenceViewModel(inferenceViewModel);

		// GUI Event Forwarder
		guiEventForwarder->setCaptureDataViewModel(captureDataViewModel);
		guiEventForwarder->setConvertDataViewModel(convertDataViewModel);
		guiEventForwarder->setTrainingViewModel(trainingViewModel);
		//guiEventForwarder->setInferenceViewModel(inferenceViewModel);

		// Views 
		tabView->setTabViewModel(tabViewModel);
		captureDataView->setCaptureDataViewModel(captureDataViewModel);
		captureDataView->setMessageViewModel(messageViewModel);
		convertDataView->setConvertDataViewModel(convertDataViewModel);
		convertDataView->setTabViewModel(tabViewModel);
		convertDataView->setMessageViewModel(messageViewModel);
		trainingView->setTrainingViewModel(trainingViewModel);
		trainingView->setTabViewModel(tabViewModel);
		trainingView->setMessageViewModel(messageViewModel);
		//inferenceView->setInferenceViewModel(inferenceViewModel);
		//inferenceView->setMessageViewModel(messageViewModel);

		// View GUI Event Forwarders
		tabView->setGUIEventForwarder(guiEventForwarder);
		captureDataView->setGUIEventForwarder(guiEventForwarder);
		convertDataView->setGUIEventForwarder(guiEventForwarder);
		trainingView->setGUIEventForwarder(guiEventForwarder);
		//inferenceView->setGUIEventForwarder(guiEventForwarder);

		// TabView
		tabView->setCaptureDataView(captureDataView);
		tabView->setConvertDataView(convertDataView);
		tabView->setTrainingView(trainingView);
		//tabView->setInferenceView(inferenceView);

		// ViewModels Change Listener
		tabViewModel->attachListener(tabView.get());
		tabViewModel->attachListener(convertDataView.get());
		tabViewModel->attachListener(trainingView.get());
		messageViewModel->attachListener(captureDataView.get());
		messageViewModel->attachListener(convertDataView.get());
		messageViewModel->attachListener(trainingView.get());
		//messageViewModel->attachListener(inferenceView.get());
		captureDataViewModel->attachListener(captureDataView.get());
		convertDataViewModel->attachListener(convertDataView.get());
		trainingViewModel->attachListener(trainingView.get());
		//inferenceViewModel->attachListener(inferenceView.get());

		// View and View Model consistency
		tabViewModel->notify();
		messageViewModel->notify();
		captureDataViewModel->notify();
		convertDataViewModel->notify();
		trainingViewModel->notify();
		//inferenceViewModel->notify();
	}

	std::shared_ptr<gui::GUIPresenter> GUIConstructor::getPresenter()
	{
		return guiPresenter;
	}

	void GUIConstructor::addHigherLevelDependency(std::shared_ptr<usecases::FaceSynthesizingGUIInteractor> faceSynthesizerGUIInteractor)
	{
		guiEventForwarder->setFaceSynthesizingGUIInteractor(faceSynthesizerGUIInteractor);
	}
}