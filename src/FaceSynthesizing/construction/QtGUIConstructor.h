#pragma once

#include "GUIConstructor.h"
#include "../gui/QtTabView.h"
#include "../gui/QtCaptureDataView.h"
#include "../gui/QtConvertDataView.h"
#include "../gui/QtTrainingView.h"

#include <QtWidgets/QDialog>

namespace qtgui = facesynthesizing::infrastructure::qtgui;

namespace facesynthesizing::construction {
	struct QtGuiMainWindow: GuiMainWindow {
		std::shared_ptr<QDialog> mainWindow;
	};

	class QtGUIConstructor : public GUIConstructor {
	public:
		QtGUIConstructor();
		virtual void resolveDependencies();
		std::shared_ptr<GuiMainWindow> getMainWindow();
	private:
		std::shared_ptr<qtgui::QtTabView> qtTabView;
		std::shared_ptr<qtgui::QtCaptureDataView> qtCaptureDataView;
		std::shared_ptr<qtgui::QtConvertDataView> qtConvertDataView;
		std::shared_ptr<qtgui::QtTrainingView> qtTrainingView;
		//std::shared_ptr<qtgui::QtInferenceDataView> qtInferenceDataView;
	};
}
