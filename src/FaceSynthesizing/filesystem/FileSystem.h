#pragma once

#include "../usecases/FaceSynthesizingFileSystem.h"
#include "MotionHubUtil/ThreadPool.h"

#include <filesystem>
#include <stdexcept>
#include <map>
#include <functional>

namespace usecases = facesynthesizing::domain::usecases;
namespace fs = std::filesystem;

namespace facesynthesizing::infrastructure::filesystem {
	class FileSystem : public usecases::FaceSynthesizingFileSystem {
	public:
		FileSystem();
		bool doesCaptureExists(std::string captureName);
		std::vector<std::string> getAllExistingCaptureNames();
		void prepareCaptureLocation(std::shared_ptr<usecases::CaptureDataInformation> captureInfos, bool overwrite);
		int countExistingCaptureDataPairs(std::string captureName);
		void saveCaptureDataPair(usecases::CaptureDataPair dataPair, std::string captureName, std::string filename, usecases::CaptureDataPairType type);

		bool doesDatasetExists(std::string datasetName);
		bool datasetContainsTrainingData(std::string datasetName);
		bool datasetContainsEvaluationData(std::string datasetName);
		std::vector<std::string> getAllExistingDatasetNames();

		bool doesCheckpointExists(std::string checkpointName);
		std::vector<std::string> getAllExistingCheckpointNames();

		// Construction purpose
		void setCaptureRoot(fs::path root);
		void setDatasetRoot(fs::path root);
		void setCheckpointsRoot(fs::path root);
	private:
		std::vector<std::string> allNames(fs::path root, std::function<bool(std::string)> filenameValidator);
		void saveImage(std::shared_ptr<usecases::Image> image, fs::path fullFilePath);
		int getAccordingCvMatType(std::shared_ptr<usecases::Image> image);

		std::unique_ptr<ThreadPool> threadPool;
		fs::path captureRoot;
		fs::path datasetRoot;
		fs::path checkpointsRoot;
	};
}