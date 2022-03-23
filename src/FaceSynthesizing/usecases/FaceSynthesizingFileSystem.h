#pragma once

#include "FaceSynthesizingDataStructures.h"

#include <string>
#include <vector>
#include <memory>

namespace facesynthesizing::domain::usecases {
	enum class CaptureDataPairType {
		Training, Evaluation
	};
	class FaceSynthesizingFileSystem {
	public:
		virtual bool doesCaptureExists(std::string captureName) = 0;
		virtual std::vector<std::string> getAllExistingCaptureNames() = 0;
		virtual void prepareCaptureLocation(std::shared_ptr<CaptureDataInformation> captureInfos, bool overwrite) = 0;
		virtual int countExistingCaptureDataPairs(std::string captureName) = 0;
		virtual void saveCaptureDataPair(
			CaptureDataPair dataPair, 
			std::string captureName, 
			std::string filename,
			CaptureDataPairType type
		) = 0;

		virtual bool doesDatasetExists(std::string datasetName) = 0;
		virtual bool datasetContainsTrainingData(std::string datasetName) = 0;
		virtual bool datasetContainsEvaluationData(std::string datasetName) = 0;
		virtual std::vector<std::string> getAllExistingDatasetNames() = 0;

		virtual bool doesCheckpointExists(std::string checkpointName) = 0;
		virtual std::vector<std::string> getAllExistingCheckpointNames() = 0;
	};
}