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
		//convertDataViewModel = std::make_shared<gui::ConvertDataViewModel>();
		//trainViewModel = std::make_shared<gui::TrainViewModel>();
		//inferenceViewModel = std::make_shared<gui::InferenceViewModel>();
	}

	void GUIConstructor::resolveDependencies()
	{
		// GUI Presenter
		guiPresenter->setTabViewModel(tabViewModel);
		guiPresenter->setMessageViewModel(messageViewModel);
		guiPresenter->setCaptureDataViewModel(captureDataViewModel);
		//guiPresenter->setConvertDataViewModel(convertDataViewModel);
		//guiPresenter->setTrainViewModel(trainViewModel);
		//guiPresenter->setInferenceViewModel(inferenceViewModel);

		// GUI Event Forwarder
		guiEventForwarder->setCaptureDataViewModel(captureDataViewModel);
		//guiEventForwarder->setConvertDataViewModel(convertDataViewModel);
		//guiEventForwarder->setTrainViewModel(trainViewModel);
		//guiEventForwarder->setInferenceViewModel(inferenceViewModel);

		// Views 
		tabView->setTabViewModel(tabViewModel);
		captureDataView->setCaptureDataViewModel(captureDataViewModel);
		captureDataView->setMessageViewModel(messageViewModel);
		//convertDataView->setConvertDataViewModel(convertDataViewModel);
		//convertDataView->setMessageViewModel(messageViewModel);
		//trainView->setTrainViewModel(trainViewModel);
		//trainView->setMessageViewModel(messageViewModel);
		//inferenceView->setInferenceViewModel(inferenceViewModel);
		//inferenceView->setMessageViewModel(messageViewModel);

		// View GUI Event Forwarders
		captureDataView->setGUIEventForwarder(guiEventForwarder);
		//convertDataView->setGUIEventForwarder(guiEventForwarder);
		//trainView->setGUIEventForwarder(guiEventForwarder);
		//inferenceView->setGUIEventForwarder(guiEventForwarder);

		// TabView
		tabView->setCaptureDataView(captureDataView);

		// ViewModels Change Listener
		tabViewModel->attachListener(tabView.get());
		messageViewModel->attachListener(captureDataView.get());
		//messageViewModel->attachListener(convertDataView.get());
		//messageViewModel->attachListener(trainView.get());
		//messageViewModel->attachListener(inferenceView.get());
		captureDataViewModel->attachListener(captureDataView.get());
		//convertDataViewModel->attachListener(convertDataView.get());
		//trainViewModel->attachListener(trainView.get());
		//inferenceViewModel->attachListener(inferenceView.get());
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