#include "FileSystem.h"

#include <iostream>
#include <opencv2/opencv.hpp>

namespace facesynthesizing::infrastructure::filesystem {
	FileSystem::FileSystem()
	{
		threadPool = std::make_unique<ThreadPool>(6);
	}
	bool FileSystem::doesCaptureExists(std::string captureName)
	{
		if (captureName.empty())
			return false;

		fs::path captureRoot = datasetRoot / "unprocessed" / captureName;
		return fs::exists(captureRoot);
	}
	std::vector<std::string> FileSystem::getAllExistingCaptureNames()
	{
		std::cout << "datasetRoot: " << datasetRoot.string() << std::endl;
		fs::path unprocessedRoot = datasetRoot / "unprocessed";
		return allNames(unprocessedRoot,
			std::bind(&FileSystem::doesCaptureExists, this, std::placeholders::_1));
	}
	void FileSystem::prepareCaptureLocation(std::shared_ptr<usecases::CaptureDataInformation> captureInfos, bool overwrite)
	{
		fs::path captureRoot = datasetRoot / "unprocessed" / captureInfos->name;
		fs::path trainingRoot = captureRoot / "train";
		fs::path evaluationRoot = captureRoot / "eval";

		fs::path trainingColorImageDir = trainingRoot / "Color";
		fs::path trainingDepthImageDir = trainingRoot / "Depth";
		fs::path trainingInfraredImageDir = trainingRoot / "Infrared";
		fs::path evaluationColorImageDir = evaluationRoot / "Color";
		fs::path evaluationDepthImageDir = evaluationRoot / "Depth";
		fs::path evaluationInfraredImageDir = evaluationRoot / "Infrared";

		if (overwrite && fs::exists(captureRoot) && !fs::remove_all(captureRoot))
			throw "Could not remove root dir and content: " + captureRoot.string();
		if (captureInfos->train_images > 0) {
			fs::create_directories(trainingColorImageDir);
			fs::create_directories(trainingDepthImageDir);
			fs::create_directories(trainingInfraredImageDir);
		}
		if (captureInfos->evaluation_images > 0) {
			fs::create_directories(evaluationColorImageDir);
			fs::create_directories(evaluationDepthImageDir);
			fs::create_directories(evaluationInfraredImageDir);
		}
	}
	int FileSystem::countExistingCaptureDataPairs(std::string captureName)
	{
		if (!doesCaptureExists(captureName))
			return 0;

		fs::path trainColorDir = datasetRoot / "unprocessed" / captureName / "train" / "Color";
		fs::path evalColorDir = datasetRoot / "unprocessed" / captureName / "eval" / "Color";

		int trainDataPairs = 0;
		int evalDataPairs = 0;
		if (fs::exists(trainColorDir))
			trainDataPairs = std::distance(fs::directory_iterator(trainColorDir), fs::directory_iterator{});
		if (fs::exists(evalColorDir))
			evalDataPairs = std::distance(fs::directory_iterator(evalColorDir), fs::directory_iterator{});

		return trainDataPairs + evalDataPairs;
	}
	void FileSystem::saveCaptureDataPair(usecases::CaptureDataPair dataPair, std::string captureName, std::string filename, usecases::CaptureDataPairType type)
	{
		fs::path trainingRoot = datasetRoot / "unprocessed" / captureName / "train";
		fs::path evaluationRoot = datasetRoot / "unprocessed" / captureName / "eval";
		fs::path root = type == usecases::CaptureDataPairType::Training ? trainingRoot : evaluationRoot;

		fs::path colorImageFile = root / "Color" / filename;
		fs::path depthImageFile = root / "Depth" / filename;
		fs::path infraredImageFile = root / "Infrared" / filename;

		//saveImage(dataPair.colorImage, colorImageFile);

		threadPool->addJob(std::bind(&FileSystem::saveImage, this, dataPair.colorImage, colorImageFile));
		threadPool->addJob(std::bind(&FileSystem::saveImage, this, dataPair.depthImage, depthImageFile));
		threadPool->addJob(std::bind(&FileSystem::saveImage, this, dataPair.infraredImage, infraredImageFile));
	}

	bool FileSystem::doesDatasetExists(std::string datasetName)
	{
		if (datasetName.empty())
			return false;

		fs::path dataRoot = datasetRoot / "processed" / datasetName;
		return fs::exists(dataRoot);
	}
	bool FileSystem::datasetContainsTrainingData(std::string datasetName)
	{
		fs::path dataRoot = datasetRoot / "processed" / datasetName / "train";
		return fs::exists(dataRoot);
	}
	bool FileSystem::datasetContainsEvaluationData(std::string datasetName)
	{
		fs::path dataRoot = datasetRoot / "processed" / datasetName / "eval";
		return fs::exists(dataRoot);
	}
	std::vector<std::string> FileSystem::getAllExistingDatasetNames()
	{
		fs::path processedRoot = datasetRoot / "processed";
		return allNames(processedRoot,
			std::bind(&FileSystem::doesDatasetExists, this, std::placeholders::_1));
	}

	bool FileSystem::doesCheckpointExists(std::string checkpointName)
	{
		if (checkpointName.empty())
			return false;

		fs::path modelRoot = checkpointsRoot / checkpointName;
		return fs::exists(modelRoot);
	}
	std::vector<std::string> FileSystem::getAllExistingCheckpointNames()
	{
		return allNames(checkpointsRoot, 
			std::bind(&FileSystem::doesCheckpointExists, this, std::placeholders::_1));
	}
	
	std::vector<std::string> FileSystem::allNames(fs::path root, std::function<bool(std::string)> filenameValidator)
	{
		if (!fs::exists(root))
			return {};

		std::vector<std::string> allNames;
		for (auto& entry : fs::directory_iterator(root)) {
			if (entry.is_directory()) {
				std::string filename = entry.path().filename().string();
				if (filenameValidator(filename))
					allNames.push_back(filename);
			}
		}
		return allNames;
	}
	void FileSystem::saveImage(std::shared_ptr<usecases::Image> image, fs::path fullFilePath)
	{
		int cvtype = getAccordingCvMatType(image);

		cv::Mat cvimage(
			image->height,
			image->width,
			cvtype,
			(void*) image->data.get(),
			cv::Mat::AUTO_STEP
		);
		cv::imwrite(fullFilePath.string(), cvimage, std::vector<int>());
	}
	int FileSystem::getAccordingCvMatType(std::shared_ptr<usecases::Image> image)
	{
		int cvtype;
		if (image->bytesPerChannelValue == 1) {
			switch (image->channels)
			{
			case 1:
				cvtype = CV_8UC1;
				break;
			case 3:
				cvtype = CV_8UC3;
				break;
			case 4:
				cvtype = CV_8UC4;
				break;
			default:
				throw std::invalid_argument("Number of image channels is unsupported: " + image->channels);
				break;
			}
		}
		else if (image->bytesPerChannelValue == 2) {
			switch (image->channels)
			{
			case 1:
				cvtype = CV_16UC1;
				break;
			case 3:
				cvtype = CV_16UC3;
				break;
			case 4:
				cvtype = CV_16UC4;
				break;
			default:
				throw std::invalid_argument("Number of image channels is unsupported: " + image->channels);
				break;
			}
		}
		else {
			throw std::invalid_argument("Unsupported Number of bytes per channel value: " + image->bytesPerChannelValue);
		}

		return cvtype;
	}

	void FileSystem::setDatasetRoot(fs::path datasetRoot)
	{
		this->datasetRoot = datasetRoot;
	}
	void FileSystem::setCheckpointsRoot(fs::path checkpointsRoot)
	{
		this->checkpointsRoot = checkpointsRoot;
	}
}