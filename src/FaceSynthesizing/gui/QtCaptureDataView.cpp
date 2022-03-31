#include "QtCaptureDataView.h"
#include "ui_QtCaptureDataView.h"

#include<iostream>

namespace facesynthesizing::infrastructure::qtgui{
    QtCaptureDataView::QtCaptureDataView(QWidget* parent) : QWidget(parent), ui(new Ui::QtCaptureDataView) {
        ui->setupUi(this);
        connectSignalsAndSlots();
        addItemsToBoundingBoxComboBox();
        addItemsToFaceAlignmentComboBox();
    }
    void QtCaptureDataView::connectSignalsAndSlots()
    {
        QObject::connect(this, &QtCaptureDataView::updateConfigurationGroupAsync,
            this, &QtCaptureDataView::updateConfigurationGroup, Qt::QueuedConnection);
        QObject::connect(this, &QtCaptureDataView::updateTaskGroupAsync,
            this, &QtCaptureDataView::updateTaskGroup, Qt::QueuedConnection);
        QObject::connect(this, &QtCaptureDataView::updateVisualizationGroupAsync,
            this, &QtCaptureDataView::updateVisualizationGroup, Qt::QueuedConnection);
        QObject::connect(this, &QtCaptureDataView::updateMessagesAsync,
            this, &QtCaptureDataView::updateMessages, Qt::QueuedConnection);
    }
    void QtCaptureDataView::addItemsToBoundingBoxComboBox()
    {
        std::list<usecases::BoundingBoxAlgorithm> allBBAlgorithms = usecases::allBoundingBoxAlgorithms();
        for (usecases::BoundingBoxAlgorithm algorithm : allBBAlgorithms) {
            std::string algorithmName = usecases::boundingBoxAlgorithmToString(algorithm);
            ui->bbAlgorithmComboBox->addItem(QString::fromStdString(algorithmName));
        }
    }
    void QtCaptureDataView::addItemsToFaceAlignmentComboBox()
    {
        std::list<usecases::FaceAlignmentAlgorithm> allFAAlgorithms = usecases::allFaceAlignmentAlgorithm();
        for (usecases::FaceAlignmentAlgorithm algorithm : allFAAlgorithms) {
            std::string algorithmName = usecases::faceAlignmentAlgorithmToString(algorithm);
            ui->faAlgorithmComboBox->addItem(QString::fromStdString(algorithmName));
        }
    }
    QtCaptureDataView::~QtCaptureDataView() {
        delete ui;
    }

    void QtCaptureDataView::changeListenerTriggered(util::ChangeObservable* source)
    {
        if (!isInitialized) {
            // Synchronized !!!
            updateConfigurationGroup();
            updateTaskGroup();
            updateVisualizationGroup();
            updateMessages();

            isInitialized = true;
        }
        else if (source == captureDataViewModel.get()) {
            updateConfigurationGroupAsync();
            updateTaskGroupAsync();
            updateVisualizationGroupAsync();
        }
        else if (source == messageViewModel.get()) {
            updateMessagesAsync();
        }
    }
    void QtCaptureDataView::updateConfigurationGroup()
    {
        std::unique_lock<std::mutex> lock(captureDataViewModel->viewModelMutex);
        isUpdating = true;
        ui->nameLineEdit->setText(QString::fromStdString(captureDataViewModel->name));
        ui->trainImagesSpinBox->setValue(captureDataViewModel->train_images);
        ui->evalImagesSpinBox->setValue(captureDataViewModel->evaluation_images);
        auto bbAlgorithmName = usecases::boundingBoxAlgorithmToString(captureDataViewModel->boundingBoxAlgorithm);
        ui->bbAlgorithmComboBox->setCurrentText(QString::fromStdString(bbAlgorithmName));
        auto faAlgorithmName = usecases::faceAlignmentAlgorithmToString(captureDataViewModel->faceAlignmentAlgorithm);
        ui->faAlgorithmComboBox->setCurrentText(QString::fromStdString(faAlgorithmName));
        ui->minFaceSizeSpinBox->setValue(captureDataViewModel->minimalFaceSize);
        ui->maxPitchDoubleSpinBox->setValue(captureDataViewModel->maxPitch);
        ui->maxRollDoubleSpinBox->setValue(captureDataViewModel->maxRoll);
        ui->maxYawDoubleSpinBox->setValue(captureDataViewModel->maxYaw);

        ui->nameLineEdit->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->trainImagesSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->evalImagesSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->bbAlgorithmComboBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->faAlgorithmComboBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->minFaceSizeSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->maxPitchDoubleSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->maxRollDoubleSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->maxYawDoubleSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        isUpdating = false;
    }
    void QtCaptureDataView::updateTaskGroup()
    {
        isUpdating = true;
        ui->initializeCameraPushButton->setEnabled(captureDataViewModel->isInitializeButtonActivated);
        ui->startCapturePushButton->setEnabled(captureDataViewModel->isCaptureButtonActivated);
        ui->cancelPushButton->setEnabled(captureDataViewModel->isCancelButtonActivated);
        isUpdating = false;
    }
    void QtCaptureDataView::updateVisualizationGroup()
    {
        std::unique_lock<std::mutex> lock(captureDataViewModel->viewModelMutex);
        isUpdating = true;
        ui->visualizeCheckBox->setChecked(captureDataViewModel->visualize);
        if (captureDataViewModel->visualize) {
            visualizeImage(captureDataViewModel->colorImage, ui->imageColorLabel, nullptr);
            visualizeImage(captureDataViewModel->boundingBoxImage, ui->imageBBLabel, nullptr);
            visualizeImage(captureDataViewModel->faceAlignmentImage, ui->imageAlignmentLabel, nullptr);
        }
        else {
            ui->imageColorLabel->clear();
            ui->imageBBLabel->clear();
            ui->imageAlignmentLabel->clear();
        }
        isUpdating = false;
    }
    void QtCaptureDataView::updateMessages()
    {
        std::unique_lock<std::mutex> lock(messageViewModel->viewModelMutex);
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

    void QtCaptureDataView::onConfigurationGroupChanged() {
        if (isUpdating || !isInitialized)
            return;

        std::unique_lock<std::mutex> lock(captureDataViewModel->viewModelMutex);
        captureDataViewModel->name = ui->nameLineEdit->text().toStdString();
        captureDataViewModel->train_images = ui->trainImagesSpinBox->value();
        captureDataViewModel->evaluation_images = ui->evalImagesSpinBox->value();
        auto bbAlgorithmName = ui->bbAlgorithmComboBox->currentText().toStdString();
        captureDataViewModel->boundingBoxAlgorithm = usecases::stringToBoundingBoxAlgorithm(bbAlgorithmName);
        auto faAlgorithmName = ui->bbAlgorithmComboBox->currentText().toStdString();
        captureDataViewModel->faceAlignmentAlgorithm = usecases::stringToFaceAlignmentAlgorithm(faAlgorithmName);
        captureDataViewModel->minimalFaceSize = static_cast<float>(ui->minFaceSizeSpinBox->value());
        captureDataViewModel->maxPitch = static_cast<float>(ui->maxPitchDoubleSpinBox->value());
        captureDataViewModel->maxRoll = static_cast<float>(ui->maxRollDoubleSpinBox->value());
        captureDataViewModel->maxYaw = static_cast<float>(ui->maxYawDoubleSpinBox->value());
        lock.unlock();

        captureDataViewModel->notify();
    }
    void QtCaptureDataView::onInitializeCamera() {
        if (isUpdating || !isInitialized)
            return;

        guiEventForwarder->initializeCamera();
    }
    void QtCaptureDataView::onStartCapture() {
        if (isUpdating || !isInitialized)
            return;

        guiEventForwarder->startCaptureData();
    }
    void QtCaptureDataView::onCancel() {
        if (isUpdating || !isInitialized)
            return;

        guiEventForwarder->cancel();
    }
    void QtCaptureDataView::onVisualizationGroupChanged() {
        if (isUpdating || !isInitialized)
            return;

        std::unique_lock<std::mutex> lock(captureDataViewModel->viewModelMutex);
        captureDataViewModel->visualize = ui->visualizeCheckBox->isChecked();
        lock.unlock();

        captureDataViewModel->notify();
    }
}