#include "FileSystem.h"

#include <iostream>
#include <opencv2/opencv.hpp>

namespace facesynthesizing::infrastructure::filesystem {
	FileSystem::FileSystem()
	{
		threadPool = std::make_unique<ThreadPool>(6);
	}
	void FileSystem::prepareSavingCaptureDataPairs(std::string captureName)
	{
		fs::path trainingRoot = datasetRoot / "unprocessed" / captureName / "train";
		fs::path evaluationRoot = datasetRoot / "unprocessed" / captureName / "eval";

		fs::path trainingColorImageDir = trainingRoot / "Color";
		fs::path trainingDepthImageDir = trainingRoot / "Depth";
		fs::path trainingInfraredImageDir = trainingRoot / "Infrared";
		fs::path evaluationColorImageDir = evaluationRoot / "Color";
		fs::path evaluationDepthImageDir = evaluationRoot / "Depth";
		fs::path evaluationInfraredImageDir = evaluationRoot / "Infrared";

		fs::create_directories(trainingColorImageDir);
		fs::create_directories(trainingDepthImageDir);
		fs::create_directories(trainingInfraredImageDir);
		fs::create_directories(evaluationColorImageDir);
		fs::create_directories(evaluationDepthImageDir);
		fs::create_directories(evaluationInfraredImageDir);
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