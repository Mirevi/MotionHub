#pragma once

#include "CaptureDataView.h"

#include <list>
#include <QtWidgets/QWidget>
#include <QScrollBar>

namespace gui = facesynthesizing::domain::adapters::gui;

namespace Ui
{
	class QtCaptureDataView;
}

namespace facesynthesizing::infrastructure::qtgui{
	class QtCaptureDataView : public QWidget, public gui::CaptureDataView {
		Q_OBJECT

	public:
		explicit QtCaptureDataView(QWidget* parent = nullptr);
		~QtCaptureDataView();
		void changeListenerTriggered(util::ChangeObservable* source);
	signals:
		void updateConfigurationGroupAsync();
		void updateTaskGroupAsync();
		void updateVisualizationGroupAsync();
		void updateMessagesAsync();
	private slots:
		void onConfigurationGroupChanged();
		void onInitializeCamera();
		void onStartCapture();
		void onCancel();
		void onVisualizationGroupChanged();

		void updateConfigurationGroup();
		void updateTaskGroup();
		void updateVisualizationGroup();
		void updateMessages();
	private:
		void connectSignalsAndSlots();
		void addItemsToBoundingBoxComboBox();
		void addItemsToFaceAlignmentComboBox();

		bool isInitialized = false;
		Ui::QtCaptureDataView* ui;
	};
}