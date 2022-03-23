#pragma once

#include "GUIViewModel.h"
#include "../usecases/FaceSynthesizingEnums.h"
#include "../usecases/FaceSynthesizingDataStructures.h"

#include <memory>
#include <string>
#include <vector>

namespace usecases = facesynthesizing::domain::usecases;

namespace facesynthesizing::domain::adapters::gui {
	class TrainingViewModel : public ViewModel {
	public:
		// General Configuration
		std::string name = "new_data";
		usecases::ColorFormat colorFormat = usecases::ColorFormat::BGR;
		int continueEpoch = 999999999;
		
		// Dataset Configuration
		std::vector<std::string> allDatasetNames;
		std::string datasetName = "";
		int trainingBatchSize = 1;
		int evaluationBatchSize = 1;
		int dataLoaderThreads = 4;
		int datasetImageSize = 512;
		int maxDatasetSize = 999999999;
		bool useSerialBatches = false;
		bool useDepthMask = true;

		// Model Configuration
		usecases::ModelType modelType = usecases::ModelType::Pix2Pix_Extended;
		usecases::GeneratorArchitecture generatorArchitecture = usecases::GeneratorArchitecture::UNet;
		usecases::DiscriminatorArchitecture discriminatorArchitecture = usecases::DiscriminatorArchitecture::Multiscale;
		usecases::GANMode ganMode = usecases::GANMode::LSGAN;
		int generatorFilters = 64;
		int discriminatorFilters = 64;
		usecases::NormalizationLayerType normalizationLayerType = usecases::NormalizationLayerType::Batch;
		bool useDropout = true;
		int discriminators = 3;
		int discriminatorLayers = 4;
		bool useMappingNetwork = false;
		usecases::MappingNetwork mappingNetwork = usecases::MappingNetwork::MLP_with_Conv_512;
		
		// Training Configuration
		int lrConsistentEpochs = 30;
		int lrDecayingEpochs = 70;
		float beta1 = 0.5;
		float learningRate = 0.0002;
		usecases::LearningRatePolicy learningRatePolicy = usecases::LearningRatePolicy::Linear;
		int stepDecayIterations = 50;
		usecases::WeightInitType weightInitType = usecases::WeightInitType::Normal;
		float weightInitGain = 0.02;
		bool useL1Loss = true;
		bool useFeatureMatchingLoss = true;
		bool useLPIPSLoss = true;
		bool useCycleLoss = false;
		float lambdaL1 = 100.0;
		float lambdaFeatureMatching = 10.0;
		float lambdaLPIPS = 10.0;
		float lambdaCycleForward = 10.0;
		float lambdaCycleBackward = 10.0;
		int checkpointFrequency = 2;
		
		// Evaluation Configuration
		int pixelAccuracyMetricThreshold = 5;
		std::string visdomEnvironment = "main";
		int visdomImageColumns = 5;
		bool useVisdom = true;
		bool startNewVisdomServer = true;
		int visdomServerPort = 8097;
		std::string visdomServerURL = "http://localhost";

		// Tasks
		bool isTrainingButtonActivated = true;
		bool isCancelButtonActivated = false;

		// Visualization
		bool visualize = true;
		std::shared_ptr<usecases::Image> flmImage = nullptr;
		std::shared_ptr<usecases::Image> colorRealImage = nullptr;
		std::shared_ptr<usecases::Image> colorFakeImage = nullptr;
		std::shared_ptr<usecases::Image> depthRealImage = nullptr;
		std::shared_ptr<usecases::Image> depthFakeImage = nullptr;
	};
}