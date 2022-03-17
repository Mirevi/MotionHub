#include "QtCaptureDataView.h"
#include "ui_QtCaptureDataView.h"

#include<iostream>

namespace facesynthesizing::infrastructure::qtgui{
    QtCaptureDataView::QtCaptureDataView(QWidget* parent) : QWidget(parent), ui(new Ui::QtCaptureDataView) {
        ui->setupUi(this);
        connectSignalsAndSlots();
        addItemsToBoundingBoxComboBox();
        addItemsToFaceAlignmentComboBox();

        isInitialized = true;
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
        this->captureDataViewModel->detachListener(this);
    }

    void QtCaptureDataView::changeListenerTriggered(util::ChangeObservable* source)
    {
        if (source == captureDataViewModel.get()) {
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
        ui->nameLineEdit->setText(QString::fromStdString(captureDataViewModel->name));
        ui->trainImagesSpinBox->setValue(captureDataViewModel->train_images);
        ui->evalImagesSpinBox->setValue(captureDataViewModel->evaluation_images);
        auto bbAlgorithmName = usecases::boundingBoxAlgorithmToString(captureDataViewModel->boundingBoxAlgorithm);
        ui->bbAlgorithmComboBox->setCurrentText(QString::fromStdString(bbAlgorithmName));
        auto faAlgorithmName = usecases::faceAlignmentAlgorithmToString(captureDataViewModel->faceAlignmentAlgorithm);
        ui->faAlgorithmComboBox->setCurrentText(QString::fromStdString(faAlgorithmName));
        ui->maxPitchDoubleSpinBox->setValue(captureDataViewModel->maxPitch);
        ui->maxRollDoubleSpinBox->setValue(captureDataViewModel->maxRoll);
        ui->maxYawDoubleSpinBox->setValue(captureDataViewModel->maxYaw);

        ui->nameLineEdit->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->trainImagesSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->evalImagesSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->bbAlgorithmComboBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->faAlgorithmComboBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->maxPitchDoubleSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->maxRollDoubleSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
        ui->maxYawDoubleSpinBox->setDisabled(!captureDataViewModel->isConfigurationActivated);
    }
    void QtCaptureDataView::updateTaskGroup()
    {
        ui->initializeCameraPushButton->setDisabled(!captureDataViewModel->isInitializeButtonActivated);
        ui->startCapturePushButton->setDisabled(!captureDataViewModel->isCaptureButtonActivated);
        ui->cancelPushButton->setDisabled(!captureDataViewModel->isCancelButtonActivated);
    }
    void QtCaptureDataView::updateVisualizationGroup()
    {
        ui->visualizeCheckBox->setChecked(captureDataViewModel->visualize);
        if (captureDataViewModel->visualize) {
            QImage qimage;
            usecases::Image* image;
            image = captureDataViewModel->colorImage.get();
            if (image != nullptr) {
                qimage = QImage(image->data.get(), image->width, image->height, QImage::Format_ARGB32);
                qimage = qimage.scaled(256, 256, Qt::AspectRatioMode::KeepAspectRatio);
                ui->imageColorLabel->setPixmap(QPixmap::fromImage(qimage));
            }
            else 
                ui->imageColorLabel->clear();
            
            image = captureDataViewModel->faceAlignmentImage.get();
            if (image != nullptr) {
                qimage = QImage(image->data.get(), image->width, image->height, QImage::Format_ARGB32);
                qimage = qimage.scaled(256, 256, Qt::AspectRatioMode::KeepAspectRatio);
                ui->imageAlignmentLabel->setPixmap(QPixmap::fromImage(qimage));
            }
            else
                ui->imageAlignmentLabel->clear();
        }
        else {
            ui->imageColorLabel->setPixmap(QPixmap());
            ui->imageAlignmentLabel->setPixmap(QPixmap());
        }        
    }
    void QtCaptureDataView::updateMessages()
    {
        std::string allMessages = "";
        QString messages;
        for (std::string message : messageViewModel->notes) 
            allMessages += message + "\n";
        allMessages += messageViewModel->statusMessage;

        ui->statusMessagesContainer->setText(QString::fromStdString(allMessages));
        auto scrollBarBottom = ui->statusMessagesContainer->verticalScrollBar()->maximum();
        ui->statusMessagesContainer->verticalScrollBar()->setValue(scrollBarBottom);
    }

    void QtCaptureDataView::onConfigurationGroupChanged() {
        if (isInitialized) {
            captureDataViewModel->name = ui->nameLineEdit->text().toStdString();
            captureDataViewModel->train_images = ui->trainImagesSpinBox->value();
            captureDataViewModel->evaluation_images = ui->evalImagesSpinBox->value();
            auto bbAlgorithmName = ui->bbAlgorithmComboBox->currentText().toStdString();
            captureDataViewModel->boundingBoxAlgorithm = usecases::stringToBoundingBoxAlgorithm(bbAlgorithmName);
            auto faAlgorithmName = ui->bbAlgorithmComboBox->currentText().toStdString();
            captureDataViewModel->faceAlignmentAlgorithm = usecases::stringToFaceAlignmentAlgorithm(faAlgorithmName);
            captureDataViewModel->maxPitch = static_cast<float>(ui->maxPitchDoubleSpinBox->value());
            captureDataViewModel->maxRoll = static_cast<float>(ui->maxRollDoubleSpinBox->value());
            captureDataViewModel->maxYaw = static_cast<float>(ui->maxYawDoubleSpinBox->value());

            captureDataViewModel->notify();
        }
    }
    void QtCaptureDataView::onInitializeCamera() {
        if (isInitialized) {
            guiEventForwarder->initializeCamera();
        }
    }
    void QtCaptureDataView::onStartCapture() {
        if (isInitialized) {
            guiEventForwarder->startCaptureData();
        }
    }
    void QtCaptureDataView::onCancel() {
        if (isInitialized) {
            guiEventForwarder->cancel();
        }
    }
    void QtCaptureDataView::onVisualizationGroupChanged() {
        if (isInitialized) {
            captureDataViewModel->visualize = ui->visualizeCheckBox->isChecked();

            captureDataViewModel->notify();
        }
    }
}