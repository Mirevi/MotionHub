#pragma once

#include "GUITrainingView.h"
#include "QtUtil.h"

#include <list>
#include <QtWidgets/QWidget>
#include <QScrollBar>
#include <QLabel>

namespace gui = facesynthesizing::domain::adapters::gui;

namespace Ui
{
	class QtTrainingView;
}

namespace facesynthesizing::infrastructure::qtgui {
	class QtTrainingView : public QWidget, public gui::TrainingView {
		Q_OBJECT

	public:
		explicit QtTrainingView(QWidget* parent = nullptr);
		~QtTrainingView();
		void changeListenerTriggered(util::ChangeObservable* source);
	signals:
		void updateGeneralConfigurationGroupAsync();
		void updateDatasetConfigurationGroupAsync();
		void updateModelConfigurationGroupAsync();
		void updateTrainingConfigurationGroupAsync();
		void updateEvaluationConfigurationGroupAsync();
		void updateTaskGroupAsync();
		void updateVisualizationGroupAsync();
		void updateMessagesAsync();
	private slots:
		void updateGeneralConfigurationGroup();
		void updateDatasetConfigurationGroup();
		void updateModelConfigurationGroup();
		void updateTrainingConfigurationGroup();
		void updateEvaluationConfigurationGroup();
		void updateTaskGroup();
		void updateVisualizationGroup();
		void updateMessages();

		void onGeneralConfigurationGroupChanged();
		void onDatasetConfigurationGroupChanged();
		void onModelConfigurationGroupChanged();
		void onTrainingConfigurationGroupChanged();
		void onEvaluationConfigurationGroupChanged();
		void onTrain();
		void onCancel();
		void onVisualizationGroupChanged();
	private:
		void connectSignalsAndSlots();
		void addItemsToColorFormatComboBox();
		void addItemsToModelTypesComboBox();
		void addItemsToGeneratprArchitectureComboBox();
		void addItemsToDiscriminatorArchitectureComboBox();
		void addItemsToGANModeComboBox();
		void addItemsToNormalizationLayerTypeComboBox();
		void addItemsToMappingNetworkComboBox();
		void addItemsToLearningRatePolicyComboBox();
		void addItemsToWeightInitTypeComboBox();
		void updateAllTrainModelElements();
		void updateAllTrainModelElementsAsync();

		bool isUpdating = false;
		bool isInitialized = false;
		Ui::QtTrainingView* ui;
	};
}