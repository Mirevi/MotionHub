#include "QtConvertDataView.h"
#include "ui_QtConvertDataView.h"

#include<iostream>

namespace facesynthesizing::infrastructure::qtgui {
	QtConvertDataView::QtConvertDataView(QWidget* parent) : QWidget(parent), ui(new Ui::QtConvertDataView) {
		ui->setupUi(this);
		connectSignalsAndSlots();
		addItemsToEyeTrackingComboBox();
		addItemsToFaceTrackingComboBox();
		addItemsToDepthFillingComboBox();
	}
	void QtConvertDataView::connectSignalsAndSlots()
	{
		QObject::connect(this, &QtConvertDataView::updateGeneralConfigurationGroupAsync,
			this, &QtConvertDataView::updateGeneralConfigurationGroup, Qt::QueuedConnection);
		QObject::connect(this, &QtConvertDataView::updateLandmarkTrackingConfigurationGroupAsync,
			this, &QtConvertDataView::updateLandmarkTrackingConfigurationGroup, Qt::QueuedConnection);
		QObject::connect(this, &QtConvertDataView::updateDepthConversionConfigurationGroupAsync,
			this, &QtConvertDataView::updateDepthConversionConfigurationGroup, Qt::QueuedConnection);
		QObject::connect(this, &QtConvertDataView::updateTaskGroupAsync,
			this, &QtConvertDataView::updateTaskGroup, Qt::QueuedConnection);
		QObject::connect(this, &QtConvertDataView::updateVisualizationGroupAsync,
			this, &QtConvertDataView::updateVisualizationGroup, Qt::QueuedConnection);
		QObject::connect(this, &QtConvertDataView::updateMessagesAsync,
			this, &QtConvertDataView::updateMessages, Qt::QueuedConnection);
	}
	void QtConvertDataView::addItemsToEyeTrackingComboBox()
	{
		std::list<usecases::EyeTrackingAlgorithm> allETAlgorithms = usecases::allEyeTrackingAlgorithms();
		for (usecases::EyeTrackingAlgorithm algorithm : allETAlgorithms) {
			std::string algorithmName = usecases::eyeTrackingAlgorithmToString(algorithm);
			ui->eyeTrackingAlgorithmComboBox->addItem(QString::fromStdString(algorithmName));
		}
	}
	void QtConvertDataView::addItemsToFaceTrackingComboBox()
	{
		std::list<usecases::FaceTrackingAlgorithm> allFTAlgorithms = usecases::allFaceTrackingAlgorithms();
		for (usecases::FaceTrackingAlgorithm algorithm : allFTAlgorithms) {
			std::string algorithmName = usecases::faceTrackingAlgorithmToString(algorithm);
			ui->faceTrackingAlgorithmComboBox->addItem(QString::fromStdString(algorithmName));
		}
	}
	void QtConvertDataView::addItemsToDepthFillingComboBox()
	{
		std::list<usecases::DepthFillingAlgorithm> allFTAlgorithms = usecases::allDepthFillingAlgorithms();
		for (usecases::DepthFillingAlgorithm algorithm : allFTAlgorithms) {
			std::string algorithmName = usecases::depthFillingAlgorithmToString(algorithm);
			ui->depthFillingAlgorithmComboBox->addItem(QString::fromStdString(algorithmName));
		}
	}
	QtConvertDataView::~QtConvertDataView() {
		delete ui;
	}

	void QtConvertDataView::changeListenerTriggered(util::ChangeObservable* source)
	{
		if (!isInitialized) {
			// Synchronized !!!
			updateGeneralConfigurationGroup();
			updateLandmarkTrackingConfigurationGroup();
			updateDepthConversionConfigurationGroup();
			updateTaskGroup();
			updateVisualizationGroup();
			updateMessages();

			isInitialized = true;
		}
		else if (source == convertDataViewModel.get()) {
			updateGeneralConfigurationGroupAsync();
			updateLandmarkTrackingConfigurationGroupAsync();
			updateDepthConversionConfigurationGroupAsync();
			updateTaskGroupAsync();
			updateVisualizationGroupAsync();
		}
		else if (source == messageViewModel.get()) {
			updateMessagesAsync();
		}
		else if (source == tabViewModel.get()) {
			if (tabViewModel->currentTab == getTabType())
				guiEventForwarder->requestAllCaptureNames();
		}
	}
	void QtConvertDataView::updateGeneralConfigurationGroup()
	{
		isUpdating = true;
		ui->nameComboBox->clear();
		for (std::string name : convertDataViewModel->allNames)
			ui->nameComboBox->addItem(QString::fromStdString(name));
		if (!convertDataViewModel->name.empty())
			ui->nameComboBox->setCurrentText(QString::fromStdString(convertDataViewModel->name));
		ui->horizontalCameraFOVDoubleSpinBox->setValue(convertDataViewModel->horizontalFoV);
		ui->outputImageSizeSpinBox->setValue(convertDataViewModel->outputImageSize);
		ui->paddingSpinBox->setValue(convertDataViewModel->padding);
		ui->pcaLandmarkComponentsDoubleSpinBox->setValue(convertDataViewModel->pcaLandmarkComponents);
		ui->pcaImageComponentsDoubleSpinBox->setValue(convertDataViewModel->pcaImageComponents);
		ui->flmLineThicknessSpinBox->setValue(convertDataViewModel->flmLineThickness);

		ui->general->setEnabled(convertDataViewModel->isConfigurationActivated);
		isUpdating = false;
	}
	void QtConvertDataView::updateLandmarkTrackingConfigurationGroup()
	{
		isUpdating = true;
		auto etAlgorithmName = usecases::eyeTrackingAlgorithmToString(convertDataViewModel->eyeTrackingAlgorithm);
		ui->eyeTrackingAlgorithmComboBox->setCurrentText(QString::fromStdString(etAlgorithmName));
		auto ftAlgorithmName = usecases::faceTrackingAlgorithmToString(convertDataViewModel->faceTrackingAlgorithm);
		ui->faceTrackingAlgorithmComboBox->setCurrentText(QString::fromStdString(etAlgorithmName));
		ui->eyeTrackingStrideSpinBox->setValue(convertDataViewModel->eyeTrackingStride);
		ui->eyeTrackingThresholdSpinBox->setValue(convertDataViewModel->eyeTrackingThreshold);

		ui->eyeTrackingStrideSpinBox->setEnabled(
			convertDataViewModel->eyeTrackingAlgorithm == usecases::EyeTrackingAlgorithm::Gradient_Based);
		ui->eyeTrackingThresholdSpinBox->setEnabled(
			convertDataViewModel->eyeTrackingAlgorithm == usecases::EyeTrackingAlgorithm::Infrared);

		ui->landmarkTracking->setEnabled(convertDataViewModel->isConfigurationActivated);
		isUpdating = false;
	}
	void QtConvertDataView::updateDepthConversionConfigurationGroup()
	{
		isUpdating = true;
		ui->faceDepthSpinBox->setValue(convertDataViewModel->faceDepth);
		ui->depthPaddingSpinBox->setValue(convertDataViewModel->depthPadding);
		ui->useDepthFillingCheckBox->setChecked(convertDataViewModel->useDepthFilling);
		auto dfAlgorithmName = usecases::depthFillingAlgorithmToString(convertDataViewModel->depthFillingAlgorithm);
		ui->depthFillingAlgorithmComboBox->setCurrentText(QString::fromStdString(dfAlgorithmName));
		ui->depthFillingImageSizeSpinBox->setValue(convertDataViewModel->depthFillingImageSize);
		ui->depthFillingPatchSizeSpinBox->setValue(convertDataViewModel->depthFillingPatchSize);
		ui->depthFillingExtendedPatchSizeSpinBox->setValue(convertDataViewModel->depthFillingExtendedPatchSize);
		ui->depthFillingPaddingSpinBox->setValue(convertDataViewModel->depthFillingPadding);
		ui->depthFillingSourceAmountSpinBox->setValue(convertDataViewModel->depthFillingSourceAmount);
		ui->depthFillingUseBlurCheckBox->setChecked(convertDataViewModel->depthFillingUseBlur);
		ui->depthFillingBlurkSizeSpinBox->setValue(convertDataViewModel->depthFillingBlurKernelSize);

		bool useDepthFilling = convertDataViewModel->useDepthFilling;
		bool dfIsGradientBased = convertDataViewModel->depthFillingAlgorithm == usecases::DepthFillingAlgorithm::FDCBOP;
		ui->depthFillingAlgorithmComboBox->setEnabled(useDepthFilling);
		ui->depthFillingImageSizeSpinBox->setEnabled(useDepthFilling && dfIsGradientBased);
		ui->depthFillingPatchSizeSpinBox->setEnabled(useDepthFilling && dfIsGradientBased);
		ui->depthFillingExtendedPatchSizeSpinBox->setEnabled(useDepthFilling && dfIsGradientBased);
		ui->depthFillingPaddingSpinBox->setEnabled(useDepthFilling && dfIsGradientBased);
		ui->depthFillingSourceAmountSpinBox->setEnabled(useDepthFilling && dfIsGradientBased);
		ui->depthFillingUseBlurCheckBox->setEnabled(useDepthFilling && dfIsGradientBased);
		ui->depthFillingBlurkSizeSpinBox->setEnabled(useDepthFilling && dfIsGradientBased);

		ui->depthConversion->setEnabled(convertDataViewModel->isConfigurationActivated);
		isUpdating = false;
	}
	void QtConvertDataView::updateTaskGroup()
	{
		isUpdating = true;
		bool nameIsValid = !convertDataViewModel->name.empty();
		ui->convertDataPushButton->setEnabled(convertDataViewModel->isConvertButtonActivated && nameIsValid);
		ui->cancelPushButton->setEnabled(convertDataViewModel->isCancelButtonActivated);
		isUpdating = false;
	}
	void QtConvertDataView::updateVisualizationGroup()
	{
		isUpdating = true;
		ui->visualizeCheckBox->setChecked(convertDataViewModel->visualize);
		if (convertDataViewModel->visualize) {
			visualizeImage(convertDataViewModel->firstImage, ui->firstImageLabel, ui->firstImageIdentifierLabel);
			visualizeImage(convertDataViewModel->secondImage, ui->secondImageLabel, ui->secondImageIdentifierLabel);
			visualizeImage(convertDataViewModel->thirdImage, ui->thirdImageLabel, ui->thirdImageIdentifierLabel);
		}
		else {
			ui->firstImageLabel->clear();
			ui->secondImageLabel->clear();
			ui->thirdImageLabel->clear();
		}
		isUpdating = false;
	}
	void QtConvertDataView::updateMessages()
	{
		isUpdating = true;
		std::string allMessages = "";
		QString messages;
		for (std::string message : messageViewModel->notes)
			allMessages += message + "\n";
		allMessages += messageViewModel->statusMessage;

		ui->statusMessagesContainer->setText(QString::fromStdString(allMessages));
		auto scrollBarBottom = ui->statusMessagesContainer->verticalScrollBar()->maximum();
		ui->statusMessagesContainer->verticalScrollBar()->setValue(scrollBarBottom);
		isUpdating = false;
	}

	void QtConvertDataView::onGeneralConfigurationGroupChanged()
	{
		if (isUpdating || !isInitialized)
			return;

		convertDataViewModel->name = ui->nameComboBox->currentText().toStdString();
		convertDataViewModel->horizontalFoV = ui->horizontalCameraFOVDoubleSpinBox->value();
		convertDataViewModel->outputImageSize = ui->outputImageSizeSpinBox->value();
		convertDataViewModel->padding = ui->paddingSpinBox->value();
		convertDataViewModel->flmLineThickness = ui->flmLineThicknessSpinBox->value();

		auto pcaLandmarkComponents = ui->pcaLandmarkComponentsDoubleSpinBox->value();
		pcaLandmarkComponents = pcaLandmarkComponents > 1 ? floor(pcaLandmarkComponents) : pcaLandmarkComponents;
		convertDataViewModel->pcaLandmarkComponents = pcaLandmarkComponents;
		auto pcaImageComponents = ui->pcaImageComponentsDoubleSpinBox->value();
		pcaImageComponents = pcaImageComponents > 1 ? floor(pcaImageComponents) : pcaImageComponents;
		convertDataViewModel->pcaImageComponents = pcaImageComponents;

		convertDataViewModel->notify();
	}
	void QtConvertDataView::onLandmarkTrackingConfigurationGroupChanged()
	{
		if (isUpdating || !isInitialized)
			return;

		auto etAlgorithmName = ui->eyeTrackingAlgorithmComboBox->currentText().toStdString();
		convertDataViewModel->eyeTrackingAlgorithm = usecases::stringToEyeTrackingAlgorithm(etAlgorithmName);
		auto ftAlgorithmName = ui->faceTrackingAlgorithmComboBox->currentText().toStdString();
		convertDataViewModel->faceTrackingAlgorithm = usecases::stringToFaceTrackingAlgorithm(ftAlgorithmName);
		convertDataViewModel->eyeTrackingStride = ui->eyeTrackingStrideSpinBox->value();
		convertDataViewModel->eyeTrackingThreshold = ui->eyeTrackingThresholdSpinBox->value();

		convertDataViewModel->notify();
	}
	void QtConvertDataView::onDepthConversionConfigurationGroupChanged()
	{
		if (isUpdating || !isInitialized)
			return;

		convertDataViewModel->faceDepth = ui->faceDepthSpinBox->value();
		convertDataViewModel->depthPadding = ui->depthPaddingSpinBox->value();
		convertDataViewModel->useDepthFilling = ui->useDepthFillingCheckBox->isChecked();
		auto dfAlgorithmName = ui->depthFillingAlgorithmComboBox->currentText().toStdString();
		convertDataViewModel->depthFillingAlgorithm = usecases::stringToDepthFillingAlgorithm(dfAlgorithmName);
		convertDataViewModel->depthFillingImageSize = ui->depthFillingImageSizeSpinBox->value();
		convertDataViewModel->depthFillingPatchSize = ui->depthFillingPatchSizeSpinBox->value();
		convertDataViewModel->depthFillingExtendedPatchSize = ui->depthFillingExtendedPatchSizeSpinBox->value();
		convertDataViewModel->depthFillingPadding = ui->depthFillingPaddingSpinBox->value();
		convertDataViewModel->depthFillingSourceAmount = ui->depthFillingSourceAmountSpinBox->value();
		convertDataViewModel->depthFillingUseBlur = ui->depthFillingUseBlurCheckBox->isChecked();
		convertDataViewModel->depthFillingBlurKernelSize = ui->depthFillingBlurkSizeSpinBox->value();

		convertDataViewModel->notify();
	}
	void QtConvertDataView::onConvert()
	{
		if (isUpdating || !isInitialized)
			return;

		guiEventForwarder->startConvertData();
	}
	void QtConvertDataView::onCancel()
	{
		if (isUpdating || !isInitialized)
			return;

		guiEventForwarder->cancel();
	}
	void QtConvertDataView::onVisualizationGroupChanged()
	{
		if (isUpdating || !isInitialized)
			return;

		convertDataViewModel->visualize = ui->visualizeCheckBox->isChecked();
		convertDataViewModel->notify();
	}
}