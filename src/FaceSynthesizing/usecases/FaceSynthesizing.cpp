#include "FaceSynthesizing.h"

#include <iostream>

//TODO implement

namespace facesynthesizing::domain::usecases {
	FaceSynthesizing::~FaceSynthesizing()
	{
		currentTask = Task::NO_TASK;
	}
	void FaceSynthesizing::startInitializeCameraTask()
	{
		if (currentTask != Task::NO_TASK) {
			std::cout << "Cant initialize Camera when another Task is currently executed!" << std::endl;
			guiPresenter->newStatusMessage("Cant initialize Camera when another Task is currently executed!");
			return;
		}
		waitForTaskThreadTermination();

		currentTask = Task::INITIATE_CAMERA;
		guiPresenter->cameraInitializationStarted();
		taskThread = std::make_unique<std::thread>(&FaceSynthesizing::initializeCameraThreadMethod, this);
	}
	void FaceSynthesizing::initializeCameraThreadMethod()
	{
		try {
			initializeCamera();
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
			std::string message = "Exception during initialize Camera Task: " + std::string(e.what());
			guiPresenter->newStatusMessage(message);
			guiPresenter->endOfTask();
		}
	}
	void FaceSynthesizing::initializeCamera()
	{
		guiPresenter->newStatusMessage("Initialize Camera");
		camera->initiate();
		cameraIsInitiated = true;
		guiPresenter->cameraIsInitialized();
		guiPresenter->newStatusMessage("Camera has been initialized");

		while (currentTask == Task::INITIATE_CAMERA) {
			camera->captureImagePair();
			guiPresenter->visualizeImage(camera->getColorImage());
		}
	}

	void FaceSynthesizing::startCaptureDataTask(std::shared_ptr<CaptureDataInformation> captureDataInfo)
	{
		if (!cameraIsInitiated) {
			std::cout << "Cant Capture Data when another Task is currently executed!" << std::endl;
			guiPresenter->newStatusMessage("Cant Capture Data when another Task is currently executed!");
			return;
		}
		this->captureDataInfo = captureDataInfo;
		faceSynthesizer->setCaptureInfos(captureDataInfo);
		currentTask = Task::NO_TASK;
		waitForTaskThreadTermination();

		currentTask = Task::CAPTURE;
		guiPresenter->dataPairCaptureStarted();
		taskThread = std::make_unique<std::thread>(&FaceSynthesizing::captureDataThreadMethod, this);
	}
	void FaceSynthesizing::captureDataThreadMethod()
	{
		try {
			captureData();

			currentTask = Task::NO_TASK;
			camera->close();
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
			std::string message = "Exception during capture Data Task: " + std::string(e.what());
			guiPresenter->newStatusMessage(message);
			guiPresenter->endOfTask();
		}
	}
	void FaceSynthesizing::captureData()
	{
		guiPresenter->newStatusMessage("Capture Data Pairs");
		fileSystem->prepareSavingCaptureDataPairs(captureDataInfo->name);

		int dataPairsToCapture = captureDataInfo->train_images + captureDataInfo->evaluation_images;
		float evalDataPairRate = computeEvalDataPairRate(dataPairsToCapture);

		int counter = 0;
		float accumulatedEvalRate = evalDataPairRate;
		std::string message = "Saved Capture Data Pair: " + std::to_string(counter) + "/" + std::to_string(dataPairsToCapture);
		guiPresenter->newStatusMessage(message);
		while (currentTask == Task::CAPTURE && counter < dataPairsToCapture) {
			camera->captureImagePair();
			guiPresenter->visualizeImage(camera->getColorImage());

			CaptureDataPair dataPair = CaptureDataPair(
				camera->getColorImage(),
				camera->getDepthImage(),
				camera->getInfraredImage()
			);
			
			if (!faceSynthesizer->validateCapture(dataPair))
				continue;

			counter++;

			bool isEvalDataPair = counter == floor(accumulatedEvalRate);
			CaptureDataPairType dataPairType = isEvalDataPair ? CaptureDataPairType::Evaluation : CaptureDataPairType::Training;
			accumulatedEvalRate = isEvalDataPair ? accumulatedEvalRate + evalDataPairRate : accumulatedEvalRate;

			std::string filename = std::to_string(counter) + ".png";
			fileSystem->saveCaptureDataPair(dataPair, captureDataInfo->name, filename, dataPairType);
			counter, dataPairsToCapture;

			std::string message = "Saved Capture Data Pair: " + std::to_string(counter) + "/" + std::to_string(dataPairsToCapture);
			guiPresenter->updateStatusMessage(message);
		}

		if (currentTask == Task::CAPTURE) {
			currentTask = Task::NO_TASK;
			guiPresenter->endOfTask();
			guiPresenter->newStatusMessage("Finished Data Pair Capture.");
		}
	}
	float FaceSynthesizing::computeEvalDataPairRate(int imageCount)
	{
		if (captureDataInfo->evaluation_images > 0)
			return static_cast<float>(imageCount) / captureDataInfo->evaluation_images;
		else
			return static_cast<float>(INT_MAX);
	}

	void FaceSynthesizing::cancelTask()
	{
		currentTask = Task::NO_TASK;
		waitForTaskThreadTermination();

		guiPresenter->endOfTask();
		guiPresenter->newStatusMessage("Task Canceled!");

		if (cameraIsInitiated) {
			cameraIsInitiated = false;
			camera->close();
		}
	}
	void FaceSynthesizing::waitForTaskThreadTermination()
	{
		if (currentTask != Task::NO_TASK)
			throw "Cannot wait for an actively running Task!";

		if (taskThread.get() != nullptr && taskThread->joinable())
			taskThread->join();
	}

	void FaceSynthesizing::noImageAvailable(ImageType imageType)
	{
		guiPresenter->clearImage(imageType);
	}
	void FaceSynthesizing::visualizeImage(std::shared_ptr<Image> image)
	{
		guiPresenter->visualizeImage(image);
	}

	void FaceSynthesizing::setGuiPresenter(std::shared_ptr<FaceSynthesizingGUIPresenter> guiPresenter)
	{
		this->guiPresenter = guiPresenter;
	}
	void FaceSynthesizing::setCamera(std::shared_ptr<FaceSynthesizingCamera> camera)
	{
		this->camera = camera;
	}
	void FaceSynthesizing::setFaceSynthesizer(std::shared_ptr<FaceSynthesizingProject> faceSynthesizer)
	{
		this->faceSynthesizer = faceSynthesizer;
	}
	void FaceSynthesizing::setFileSystem(std::shared_ptr<FaceSynthesizingFileSystem> fileSystem)
	{
		this->fileSystem = fileSystem;
	}
}

