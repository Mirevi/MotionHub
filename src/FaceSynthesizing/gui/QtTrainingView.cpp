#include "QtTrainingView.h"
#include "ui_QtTrainingView.h"

#include<iostream>

namespace facesynthesizing::infrastructure::qtgui {
    QtTrainingView::QtTrainingView(QWidget* parent) : QWidget(parent), ui(new Ui::QtTrainingView) {
        ui->setupUi(this);
        connectSignalsAndSlots();
        addItemsToColorFormatComboBox();
        addItemsToModelTypesComboBox();
        addItemsToGeneratprArchitectureComboBox();
        addItemsToDiscriminatorArchitectureComboBox();
        addItemsToGANModeComboBox();
        addItemsToNormalizationLayerTypeComboBox();
        addItemsToMappingNetworkComboBox();
        addItemsToLearningRatePolicyComboBox();
        addItemsToWeightInitTypeComboBox();
    }
    void QtTrainingView::connectSignalsAndSlots()
    {
        QObject::connect(this, &QtTrainingView::updateGeneralConfigurationGroupAsync,
            this, &QtTrainingView::updateGeneralConfigurationGroup, Qt::QueuedConnection);
        QObject::connect(this, &QtTrainingView::updateDatasetConfigurationGroupAsync,
            this, &QtTrainingView::updateDatasetConfigurationGroup, Qt::QueuedConnection);
        QObject::connect(this, &QtTrainingView::updateModelConfigurationGroupAsync,
            this, &QtTrainingView::updateModelConfigurationGroup, Qt::QueuedConnection);
        QObject::connect(this, &QtTrainingView::updateTrainingConfigurationGroupAsync,
            this, &QtTrainingView::updateTrainingConfigurationGroup, Qt::QueuedConnection);
        QObject::connect(this, &QtTrainingView::updateEvaluationConfigurationGroupAsync,
            this, &QtTrainingView::updateEvaluationConfigurationGroup, Qt::QueuedConnection);
        QObject::connect(this, &QtTrainingView::updateTaskGroupAsync,
            this, &QtTrainingView::updateTaskGroup, Qt::QueuedConnection);
        QObject::connect(this, &QtTrainingView::updateVisualizationGroupAsync,
            this, &QtTrainingView::updateVisualizationGroup, Qt::QueuedConnection);
        QObject::connect(this, &QtTrainingView::updateMessagesAsync,
            this, &QtTrainingView::updateMessages, Qt::QueuedConnection);
    }
    void QtTrainingView::addItemsToColorFormatComboBox()
    {
        std::list<usecases::ColorFormat> allFormats = usecases::allColorFormats();
        for (usecases::ColorFormat format : allFormats) {
            std::string formatName = usecases::colorFormatToString(format);
            ui->colorFormatComboBox->addItem(QString::fromStdString(formatName));
        }
    }
    void QtTrainingView::addItemsToModelTypesComboBox()
    {
        std::list<usecases::ModelType> allModelTypes = usecases::allModelTypes();
        for (usecases::ModelType type : allModelTypes) {
            std::string typeName = usecases::modelTypeToString(type);
            ui->modelTypeComboBox->addItem(QString::fromStdString(typeName));
        }
    }
    void QtTrainingView::addItemsToGeneratprArchitectureComboBox()
    {
        std::list<usecases::GeneratorArchitecture> allArchitectures = usecases::allGeneratorArchitectures();
        for (usecases::GeneratorArchitecture architecture : allArchitectures) {
            std::string architectureName = usecases::generatorArchitectureToString(architecture);
            ui->generatorArchitectureComboBox->addItem(QString::fromStdString(architectureName));
        }
    }
    void QtTrainingView::addItemsToDiscriminatorArchitectureComboBox()
    {
        std::list<usecases::DiscriminatorArchitecture> allArchitectures = usecases::allDiscriminatorArchitectures();
        for (usecases::DiscriminatorArchitecture architecture : allArchitectures) {
            std::string architectureName = usecases::discriminatorArchitectureToString(architecture);
            ui->discriminatorArchitectureComboBox->addItem(QString::fromStdString(architectureName));
        }
    }
    void QtTrainingView::addItemsToGANModeComboBox()
    {
        std::list<usecases::GANMode> allModes = usecases::allGANModes();
        for (usecases::GANMode mode : allModes) {
            std::string modeName = usecases::ganModeToString(mode);
            ui->ganModeComboBox->addItem(QString::fromStdString(modeName));
        }
    }
    void QtTrainingView::addItemsToNormalizationLayerTypeComboBox()
    {
        std::list<usecases::NormalizationLayerType> allTypes = usecases::allNormalizationLayerTypes();
        for (usecases::NormalizationLayerType type : allTypes) {
            std::string typeName = usecases::normalizationLayerTypeToString(type);
            ui->normalizationLayerTypeComboBox->addItem(QString::fromStdString(typeName));
        }
    }
    void QtTrainingView::addItemsToMappingNetworkComboBox()
    {
        std::list<usecases::MappingNetwork> allNetworks = usecases::allMappingNetworks();
        for (usecases::MappingNetwork network : allNetworks) {
            std::string networkName = usecases::mappingNetworkToString(network);
            ui->mappingNetworkComboBox->addItem(QString::fromStdString(networkName));
        }
    }
    void QtTrainingView::addItemsToLearningRatePolicyComboBox()
    {
        std::list<usecases::LearningRatePolicy> allPolicies = usecases::allLearningRatePolicies();
        for (usecases::LearningRatePolicy policy : allPolicies) {
            std::string policyName = usecases::learningRatePolicyToString(policy);
            ui->LearningRatePolicyComboBox->addItem(QString::fromStdString(policyName));
        }
    }
    void QtTrainingView::addItemsToWeightInitTypeComboBox()
    {
        std::list<usecases::WeightInitType> allTypes = usecases::allWeightInitTypes();
        for (usecases::WeightInitType type : allTypes) {
            std::string typeName = usecases::weightInitTypeToString(type);
            ui->weightInitTypeComboBox->addItem(QString::fromStdString(typeName));
        }
    }
    QtTrainingView::~QtTrainingView() {
        delete ui;
    }

    void QtTrainingView::changeListenerTriggered(util::ChangeObservable* source)
    {
        if (!isInitialized) {
            // Synchronized !!!
            updateAllTrainModelElements();
            updateMessages();

            isInitialized = true;
        }
        else if (source == trainingViewModel.get()) {
            updateAllTrainModelElementsAsync();
        }
        else if (source == messageViewModel.get()) {
            updateMessagesAsync();
        }
        else if (source == tabViewModel.get()) {
            if (tabViewModel->currentTab == getTabType())
                guiEventForwarder->requestAllTrainingDatasetNames();
        }
    }
    void QtTrainingView::updateAllTrainModelElements() 
    {
        isUpdating = true;
        updateGeneralConfigurationGroup();
        updateDatasetConfigurationGroup();
        updateModelConfigurationGroup();
        updateTrainingConfigurationGroup();
        updateEvaluationConfigurationGroup();

        updateTaskGroup();
        updateVisualizationGroup();
        isUpdating = false;
    }
    void QtTrainingView::updateAllTrainModelElementsAsync()
    {
        updateGeneralConfigurationGroupAsync();
        updateDatasetConfigurationGroupAsync();
        updateModelConfigurationGroupAsync();
        updateTrainingConfigurationGroupAsync();
        updateEvaluationConfigurationGroupAsync();

        updateTaskGroupAsync();
        updateVisualizationGroupAsync();
    }
    void QtTrainingView::updateGeneralConfigurationGroup()
    {
        isUpdating = true;
        ui->nameLineEdit->setText(QString::fromStdString(trainingViewModel->name));
        auto colorFormatName = usecases::colorFormatToString(trainingViewModel->colorFormat);
        ui->colorFormatComboBox->setCurrentText(QString::fromStdString(colorFormatName));
        ui->continueEpochSpinBox->setValue(trainingViewModel->continueEpoch);

        ui->general->setEnabled(trainingViewModel->isConfigurationActivated);
        isUpdating = false;
    }
    void QtTrainingView::updateDatasetConfigurationGroup()
    {
        isUpdating = true;
        ui->datasetNameComboBox->clear();
        for (std::string datasetName : trainingViewModel->allDatasetNames)
            ui->datasetNameComboBox->addItem(QString::fromStdString(datasetName));
        if (!trainingViewModel->datasetName.empty())
            ui->datasetNameComboBox->setCurrentText(QString::fromStdString(trainingViewModel->datasetName));
        ui->batchSizeSpinBox->setValue(trainingViewModel->trainingBatchSize);
        ui->evalBatchSizeSpinBox->setValue(trainingViewModel->evaluationBatchSize);
        ui->datasetImageSizeSpinBox->setValue(trainingViewModel->datasetImageSize);
        ui->maxDatasetSizeSpinBox->setValue(trainingViewModel->maxDatasetSize);
        ui->useSerialBatchesCheckBox->setChecked(trainingViewModel->useSerialBatches);
        ui->useDepthMaskCheckBox->setChecked(trainingViewModel->useDepthMask);

        ui->dataset->setEnabled(trainingViewModel->isConfigurationActivated);
        isUpdating = false;
    }
    void QtTrainingView::updateModelConfigurationGroup()
    {
        isUpdating = true;
        auto modelTypeName = usecases::modelTypeToString(trainingViewModel->modelType);
        ui->modelTypeComboBox->setCurrentText(QString::fromStdString(modelTypeName));
        auto generatorArchitectureName = usecases::generatorArchitectureToString(trainingViewModel->generatorArchitecture);
        ui->generatorArchitectureComboBox->setCurrentText(QString::fromStdString(generatorArchitectureName));
        auto discriminatorArchitectureName = usecases::discriminatorArchitectureToString(trainingViewModel->discriminatorArchitecture);
        ui->discriminatorArchitectureComboBox->setCurrentText(QString::fromStdString(discriminatorArchitectureName));
        auto ganModeName = usecases::ganModeToString(trainingViewModel->ganMode);
        ui->ganModeComboBox->setCurrentText(QString::fromStdString(ganModeName));
        ui->generatorFiltersSpinBox->setValue(trainingViewModel->generatorFilters);
        ui->discriminatorFiltersSpinBox->setValue(trainingViewModel->discriminatorFilters);
        auto normalizationLayerName = usecases::normalizationLayerTypeToString(trainingViewModel->normalizationLayerType);
        ui->normalizationLayerTypeComboBox->setCurrentText(QString::fromStdString(normalizationLayerName));
        ui->useDropoutCheckBox->setChecked(trainingViewModel->useDropout);
        ui->numDiscriminatorsSpinBox->setValue(trainingViewModel->discriminators);
        ui->numDLayersSpinBox->setValue(trainingViewModel->discriminatorLayers);
        ui->useMappingNetworkCheckBox->setChecked(trainingViewModel->useMappingNetwork);
        auto mappingNetworkName = usecases::mappingNetworkToString(trainingViewModel->mappingNetwork);
        ui->mappingNetworkComboBox->setCurrentText(QString::fromStdString(mappingNetworkName));

        bool enableNumDiscriminator = trainingViewModel->discriminatorArchitecture == usecases::DiscriminatorArchitecture::Multiscale;
        ui->numDiscriminatorsSpinBox->setEnabled(enableNumDiscriminator);
        bool enableNLayer = 
            enableNumDiscriminator || trainingViewModel->discriminatorArchitecture == usecases::DiscriminatorArchitecture::N_Layer;
        ui->numDLayersSpinBox->setEnabled(enableNLayer);
        ui->mappingNetworkComboBox->setEnabled(trainingViewModel->useMappingNetwork);

        ui->model->setEnabled(trainingViewModel->isConfigurationActivated);
        isUpdating = false;
    }
    void QtTrainingView::updateTrainingConfigurationGroup()
    {
        isUpdating = true;
        ui->epochsSpinBox->setValue(trainingViewModel->lrConsistentEpochs);
        ui->decayingEpochsSpinBox->setValue(trainingViewModel->lrDecayingEpochs);
        ui->beta1SpinBox->setValue(trainingViewModel->beta1);
        ui->learningRateSpinBox->setValue(trainingViewModel->learningRate);
        auto learningRatePolicyName = usecases::learningRatePolicyToString(trainingViewModel->learningRatePolicy);
        ui->LearningRatePolicyComboBox->setCurrentText(QString::fromStdString(learningRatePolicyName));
        ui->stepDecayIterationsSpinBox->setValue(trainingViewModel->stepDecayIterations);
        auto weightInitTypeName = usecases::weightInitTypeToString(trainingViewModel->weightInitType);
        ui->weightInitTypeComboBox->setCurrentText(QString::fromStdString(weightInitTypeName));
        ui->weightInitGainSpinBox->setValue(trainingViewModel->weightInitGain);
        ui->useL1CheckBox->setChecked(trainingViewModel->useL1Loss);
        ui->useFeatureMatchingCheckBox->setChecked(trainingViewModel->useFeatureMatchingLoss);
        ui->useLPIPSCheckBox->setChecked(trainingViewModel->useLPIPSLoss);
        ui->useCycleCheckBox->setChecked(trainingViewModel->useCycleLoss);
        ui->lambdaL1SpinBox->setValue(trainingViewModel->lambdaL1);
        ui->lambdaFeatureMatchingSpinBox->setValue(trainingViewModel->lambdaFeatureMatching);
        ui->lambdaLPIPSSpinBox->setValue(trainingViewModel->lambdaLPIPS);
        ui->lambdaCycleForwardSpinBox->setValue(trainingViewModel->lambdaCycleForward);
        ui->lambdaCycleBackwardSpinBox->setValue(trainingViewModel->lambdaCycleBackward);
        ui->saveEpochFrequencySpinBox->setValue(trainingViewModel->checkpointFrequency);

        bool enableStepDecayIter = trainingViewModel->learningRatePolicy == usecases::LearningRatePolicy::Step;
        ui->stepDecayIterationsSpinBox->setEnabled(enableStepDecayIter);
        ui->lambdaL1SpinBox->setEnabled(trainingViewModel->useL1Loss);
        ui->lambdaFeatureMatchingSpinBox->setEnabled(trainingViewModel->useFeatureMatchingLoss);
        ui->lambdaLPIPSSpinBox->setEnabled(trainingViewModel->useLPIPSLoss);
        ui->lambdaCycleForwardSpinBox->setEnabled(trainingViewModel->useCycleLoss);
        ui->lambdaCycleBackwardSpinBox->setEnabled(trainingViewModel->useCycleLoss);

        ui->training->setEnabled(trainingViewModel->isConfigurationActivated);
        isUpdating = false;
    }
    void QtTrainingView::updateEvaluationConfigurationGroup()
    {
        isUpdating = true;
        ui->evaluationFrequencySpinBox->setValue(trainingViewModel->evaluationFrequency);
        ui->pixelAccuracyMetricThresholdSpinBox->setValue(trainingViewModel->pixelAccuracyMetricThreshold);
        ui->visdomEnvironmentLineEdit->setText(QString::fromStdString(trainingViewModel->visdomEnvironment));
        ui->visdomImageColumnsSpinBox->setValue(trainingViewModel->visdomImageColumns);
        ui->useVisdomCheckBox->setChecked(trainingViewModel->useVisdom);
        ui->visdomServerPortSpinBox->setValue(trainingViewModel->visdomServerPort);
        ui->visdomServerURLLineEdit->setText(QString::fromStdString(trainingViewModel->visdomServerURL));

        ui->visdomEnvironmentLineEdit->setEnabled(trainingViewModel->useVisdom);
        ui->visdomImageColumnsSpinBox->setEnabled(trainingViewModel->useVisdom);
        ui->visdomServerPortSpinBox->setEnabled(trainingViewModel->useVisdom);
        ui->visdomServerURLLineEdit->setEnabled(trainingViewModel->useVisdom);

        ui->evaluation->setEnabled(trainingViewModel->isConfigurationActivated);
        isUpdating = false;
    }
    void QtTrainingView::updateTaskGroup()
    {
        isUpdating = true;
        bool datasetIsValid = !trainingViewModel->datasetName.empty();
        ui->trainPushButton->setEnabled(trainingViewModel->isTrainingButtonActivated && datasetIsValid);
        ui->cancelPushButton->setEnabled(trainingViewModel->isCancelButtonActivated);
        isUpdating = false;
    }
    void QtTrainingView::updateVisualizationGroup()
    {
        isUpdating = true;
        ui->visualizeCheckBox->setChecked(trainingViewModel->visualize);
        if (trainingViewModel->visualize) {
            visualizeImage(trainingViewModel->flmImage, ui->imageFLMLabel, nullptr);
            visualizeImage(trainingViewModel->colorRealImage, ui->imageColorRealLabel, nullptr);
            visualizeImage(trainingViewModel->colorFakeImage, ui->imageColorFakeLabel, nullptr);
            visualizeImage(trainingViewModel->depthRealImage, ui->imageDepthRealLabel, nullptr);
            visualizeImage(trainingViewModel->depthFakeImage, ui->imageDepthFakeLabel, nullptr);
        }
        else {
            ui->imageFLMLabel->clear();
            ui->imageColorRealLabel->clear();
            ui->imageColorFakeLabel->clear();
            ui->imageDepthRealLabel->clear();
            ui->imageDepthFakeLabel->clear();
        }
        isUpdating = false;
    }
    void QtTrainingView::updateMessages()
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

    void QtTrainingView::onGeneralConfigurationGroupChanged()
    {
        if (isUpdating || !isInitialized)
            return;

        trainingViewModel->name = ui->nameLineEdit->text().toStdString();
        auto colorFormatName = ui->colorFormatComboBox->currentText().toStdString();
        trainingViewModel->colorFormat = usecases::stringToColorFormat(colorFormatName);
        trainingViewModel->continueEpoch = ui->continueEpochSpinBox->value();

        trainingViewModel->notify();
    }
    void QtTrainingView::onDatasetConfigurationGroupChanged()
    {
        if (isUpdating || !isInitialized)
            return;

        trainingViewModel->datasetName = ui->datasetNameComboBox->currentText().toStdString();
        trainingViewModel->trainingBatchSize = ui->batchSizeSpinBox->value();
        trainingViewModel->evaluationBatchSize = ui->evalBatchSizeSpinBox->value();
        trainingViewModel->datasetImageSize = ui->datasetImageSizeSpinBox->value();
        trainingViewModel->maxDatasetSize = ui->maxDatasetSizeSpinBox->value();
        trainingViewModel->useSerialBatches = ui->useSerialBatchesCheckBox->isChecked();
        trainingViewModel->useDepthMask = ui->useDepthMaskCheckBox->isChecked();

        trainingViewModel->notify();
    }
    void QtTrainingView::onModelConfigurationGroupChanged()
    {
        if (isUpdating || !isInitialized)
            return;

        auto modelTypeName = ui->modelTypeComboBox->currentText().toStdString();
        trainingViewModel->modelType = usecases::stringToModelType(modelTypeName);
        auto generatorArchitectureName = ui->generatorArchitectureComboBox->currentText().toStdString();
        trainingViewModel->generatorArchitecture = usecases::stringToGeneratorArchitecture(generatorArchitectureName);
        auto discriminatorArchitectureName = ui->discriminatorArchitectureComboBox->currentText().toStdString();
        trainingViewModel->discriminatorArchitecture = usecases::stringToDiscriminatorArchitecture(discriminatorArchitectureName);
        auto ganModeName = ui->ganModeComboBox->currentText().toStdString();
        trainingViewModel->ganMode = usecases::stringToGANMode(ganModeName);
        trainingViewModel->generatorFilters = ui->generatorFiltersSpinBox->value();
        trainingViewModel->discriminatorFilters = ui->discriminatorFiltersSpinBox->value();
        auto normalizationLayerName = ui->normalizationLayerTypeComboBox->currentText().toStdString();
        trainingViewModel->normalizationLayerType = usecases::stringToNormalizationLayerType(normalizationLayerName);
        trainingViewModel->useDropout = ui->useDropoutCheckBox->isChecked();
        trainingViewModel->discriminators = ui->numDiscriminatorsSpinBox->value();
        trainingViewModel->discriminatorLayers = ui->numDLayersSpinBox->value();
        trainingViewModel->useMappingNetwork = ui->useMappingNetworkCheckBox->isChecked();
        auto mappingNetworkName = ui->mappingNetworkComboBox->currentText().toStdString();
        trainingViewModel->mappingNetwork = usecases::stringToMappingNetwork(mappingNetworkName);

        trainingViewModel->notify();
    }
    void QtTrainingView::onTrainingConfigurationGroupChanged()
    {
        if (isUpdating || !isInitialized)
            return;

        trainingViewModel->lrConsistentEpochs = ui->epochsSpinBox->value();
        trainingViewModel->lrDecayingEpochs = ui->decayingEpochsSpinBox->value();
        trainingViewModel->beta1 = ui->beta1SpinBox->value();
        trainingViewModel->learningRate = ui->learningRateSpinBox->value();
        auto learningRatePolicyName = ui->LearningRatePolicyComboBox->currentText().toStdString();
        trainingViewModel->learningRatePolicy = usecases::stringToLearningRatePolicy(learningRatePolicyName);
        trainingViewModel->stepDecayIterations = ui->stepDecayIterationsSpinBox->value();
        auto weightInitTypeName = ui->weightInitTypeComboBox->currentText().toStdString();
        trainingViewModel->weightInitType = usecases::stringToWeightInitType(weightInitTypeName);
        trainingViewModel->weightInitGain = ui->weightInitGainSpinBox->value();
        trainingViewModel->useL1Loss = ui->useL1CheckBox->isChecked();
        trainingViewModel->useFeatureMatchingLoss = ui->useFeatureMatchingCheckBox->isChecked();
        trainingViewModel->useLPIPSLoss = ui->useLPIPSCheckBox->isChecked();
        trainingViewModel->useCycleLoss = ui->useCycleCheckBox->isChecked();
        trainingViewModel->lambdaL1 = ui->lambdaL1SpinBox->value();
        trainingViewModel->lambdaFeatureMatching = ui->lambdaFeatureMatchingSpinBox->value();
        trainingViewModel->lambdaLPIPS = ui->lambdaLPIPSSpinBox->value();
        trainingViewModel->lambdaCycleForward = ui->lambdaCycleForwardSpinBox->value();
        trainingViewModel->lambdaCycleBackward = ui->lambdaCycleBackwardSpinBox->value();
        trainingViewModel->checkpointFrequency = ui->saveEpochFrequencySpinBox->value();

        trainingViewModel->notify();
    }
    void QtTrainingView::onEvaluationConfigurationGroupChanged()
    {
        if (isUpdating || !isInitialized)
            return;

        trainingViewModel->evaluationFrequency = ui->evaluationFrequencySpinBox->value();
        trainingViewModel->pixelAccuracyMetricThreshold = ui->pixelAccuracyMetricThresholdSpinBox->value();
        trainingViewModel->visdomEnvironment = ui->visdomEnvironmentLineEdit->text().toStdString();
        trainingViewModel->visdomImageColumns = ui->visdomImageColumnsSpinBox->value();
        trainingViewModel->useVisdom = ui->useVisdomCheckBox->isChecked();
        trainingViewModel->visdomServerPort = ui->visdomServerPortSpinBox->value();
        trainingViewModel->visdomServerURL = ui->visdomServerURLLineEdit->text().toStdString();

        trainingViewModel->notify();
    }
    void QtTrainingView::onTrain()
    {
        if (isUpdating || !isInitialized)
            return;

        guiEventForwarder->startTraining();
    }
    void QtTrainingView::onCancel()
    {
        if (isUpdating || !isInitialized)
            return;

        guiEventForwarder->cancel();
    }
    void QtTrainingView::onVisualizationGroupChanged()
    {
        if (isUpdating || !isInitialized)
            return;

        trainingViewModel->visualize = ui->visualizeCheckBox->isChecked();
        trainingViewModel->notify();
    }
}