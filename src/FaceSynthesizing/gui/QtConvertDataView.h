#pragma once

#include "GUIConvertDataView.h"
#include "QtUtil.h"

#include <list>
#include <cmath>
#include <QtWidgets/QWidget>
#include <QScrollBar>
#include <QLabel>

namespace gui = facesynthesizing::domain::adapters::gui;

namespace Ui
{
	class QtConvertDataView;
}

namespace facesynthesizing::infrastructure::qtgui {
	class QtConvertDataView : public QWidget, public gui::ConvertDataView {
		Q_OBJECT

	public:
		explicit QtConvertDataView(QWidget* parent = nullptr);
		~QtConvertDataView();
		void changeListenerTriggered(util::ChangeObservable* source);

	signals:
		void updateGeneralConfigurationGroupAsync();
		void updateLandmarkTrackingConfigurationGroupAsync();
		void updateDepthConversionConfigurationGroupAsync();
		void updateTaskGroupAsync();
		void updateVisualizationGroupAsync();
		void updateMessagesAsync();
	private slots:
		void updateGeneralConfigurationGroup();
		void updateLandmarkTrackingConfigurationGroup();
		void updateDepthConversionConfigurationGroup();
		void updateTaskGroup();
		void updateVisualizationGroup();
		void updateMessages();

		void onGeneralConfigurationGroupChanged();
		void onLandmarkTrackingConfigurationGroupChanged();
		void onDepthConversionConfigurationGroupChanged();
		void onConvert();
		void onCancel();
		void onVisualizationGroupChanged();
	private:
		void connectSignalsAndSlots();
		void addItemsToEyeTrackingComboBox();
		void addItemsToFaceTrackingComboBox();
		void addItemsToDepthFillingComboBox();

		bool isUpdating = false;
		bool isInitialized = false;
		Ui::QtConvertDataView* ui;
	};
}