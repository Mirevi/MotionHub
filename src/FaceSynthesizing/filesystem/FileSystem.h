#pragma once

#include "../usecases/FaceSynthesizingFileSystem.h"
#include "MotionHubUtil/ThreadPool.h"

#include <filesystem>
#include <stdexcept>
#include <map>

namespace usecases = facesynthesizing::domain::usecases;
namespace fs = std::filesystem;

namespace facesynthesizing::infrastructure::filesystem {
	class FileSystem : public usecases::FaceSynthesizingFileSystem {
	public:
		FileSystem();
		void prepareSavingCaptureDataPairs(std::string captureName);
		void saveCaptureDataPair(usecases::CaptureDataPair dataPair, std::string captureName, std::string filename, usecases::CaptureDataPairType type);

		// Construction purpose
		void setDatasetRoot(fs::path root);
		void setCheckpointsRoot(fs::path root);
	private:
		void saveImage(std::shared_ptr<usecases::Image> image, fs::path fullFilePath);
		int getAccordingCvMatType(std::shared_ptr<usecases::Image> image);

		std::unique_ptr<ThreadPool> threadPool;
		fs::path datasetRoot;
		fs::path checkpointsRoot;
	};
}