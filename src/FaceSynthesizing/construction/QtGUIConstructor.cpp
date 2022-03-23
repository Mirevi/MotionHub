#include "QtGUIConstructor.h"
#include <iostream>

namespace facesynthesizing::construction {
	QtGUIConstructor::QtGUIConstructor()
	{
		// QT Views
		qtTabView = std::make_shared<qtgui::QtTabView>();
		qtCaptureDataView = std::make_shared<qtgui::QtCaptureDataView>();
		qtConvertDataView = std::make_shared<qtgui::QtConvertDataView>();
		qtTrainingView = std::make_shared<qtgui::QtTrainingView>();
		//qtInferenceDataView = std::make_shared<qtgui::QtInferenceDataView>();

		// Views
		tabView = std::shared_ptr<qtgui::QtTabView>(qtTabView);
		captureDataView = std::shared_ptr<qtgui::QtCaptureDataView>(qtCaptureDataView);
		convertDataView = std::shared_ptr<qtgui::QtConvertDataView>(qtConvertDataView);
		trainingView = std::shared_ptr<qtgui::QtTrainingView>(qtTrainingView);
		//inferenceView = std::shared_ptr<qtgui::QtInferenceDataView>(qtInferenceDataView);
	}

	void QtGUIConstructor::resolveDependencies()
	{
		GUIConstructor::resolveDependencies();

		// add Tabs to qtTabView
		qtTabView->addTab(qtCaptureDataView.get(), "Capture Data");
		qtTabView->addTab(qtConvertDataView.get(), "Convert Data");
		qtTabView->addTab(qtTrainingView.get(), "Training");
		//qtTabView->addTab(qtInferenceDataView.get(), "Inference");
	}

	std::shared_ptr<GuiMainWindow> QtGUIConstructor::getMainWindow()
	{
		QtGuiMainWindow* mainWindow = new QtGuiMainWindow();
		mainWindow->mainWindow = std::shared_ptr<QDialog>(qtTabView);

		return std::shared_ptr<GuiMainWindow>(mainWindow);
	}
}
