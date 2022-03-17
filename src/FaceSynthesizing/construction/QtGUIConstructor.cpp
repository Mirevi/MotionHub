#include "QtGUIConstructor.h"
#include <iostream>

namespace facesynthesizing::construction {
	QtGUIConstructor::QtGUIConstructor()
	{
		// QT Views
		qtTabView = std::make_shared<qtgui::QtTabView>();
		qtCaptureDataView = std::make_shared<qtgui::QtCaptureDataView>();
		//qtConvertDataView = std::make_shared<qtgui::QtConvertDataView>();
		//qtTrainView = std::make_shared<qtgui::QtTrainView>();
		//qtInferenceDataView = std::make_shared<qtgui::QtInferenceDataView>();

		// Views
		tabView = std::shared_ptr<qtgui::QtTabView>(qtTabView);
		captureDataView = std::shared_ptr<qtgui::QtCaptureDataView>(qtCaptureDataView);
		//convertDataView = std::shared_ptr<qtgui::QtConvertDataView>(qtConvertDataView);
		//trainView = std::shared_ptr<qtgui::QtTrainView>(qtTrainView);
		//inferenceView = std::shared_ptr<qtgui::QtInferenceDataView>(qtInferenceDataView);
	}

	void QtGUIConstructor::resolveDependencies()
	{
		GUIConstructor::resolveDependencies();

		// add Tabs to qtTabView
		qtTabView->addTab(qtCaptureDataView.get(), "Capture Data");
		//qtTabView->addTab(qtConvertDataView.get(), "Convert Data");
		//qtTabView->addTab(qtTrainView.get(), "Train");
		//qtTabView->addTab(qtInferenceDataView.get(), "Inference");

		// View and View Model consistency
		tabViewModel->notify();
		messageViewModel->notify();
		captureDataViewModel->notify();
		//convertDataViewModel->notify();
		//trainViewModel->notify();
		//inferenceViewModel->notify();
	}

	std::shared_ptr<GuiMainWindow> QtGUIConstructor::getMainWindow()
	{
		QtGuiMainWindow* mainWindow = new QtGuiMainWindow();
		mainWindow->mainWindow = std::shared_ptr<QDialog>(qtTabView);

		return std::shared_ptr<GuiMainWindow>(mainWindow);
	}
}
