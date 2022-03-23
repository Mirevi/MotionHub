#include "FaceSynthesizing.h"

#include <iostream>

//TODO implement

namespace facesynthesizing::domain::usecases {
	FaceSynthesizing::FaceSynthesizing()
	{
		minorTasksThreadPool = std::make_unique<ThreadPool>(1);
	}
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
		mainTaskThread = std::make_unique<std::thread>(&FaceSynthesizing::initializeCameraThreadMethod, this);
	}
	void FaceSynthesizing::initializeCameraThreadMethod()
	{
		try {
			initializeCamera();

			currentTask = Task::NO_TASK;
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
			std::string message = "Exception during initialize Camera Task: " + std::string(e.what());
			currentTask = Task::NO_TASK;
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
		mainTaskThread = std::make_unique<std::thread>(&FaceSynthesizing::captureDataThreadMethod, this);
	}
	void FaceSynthesizing::captureDataThreadMethod()
	{
		try {
			captureData();

			currentTask = Task::NO_TASK;
			camera->close();
			cameraIsInitiated = false;
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
			std::string message = "Exception during capture Data Task: " + std::string(e.what());
			currentTask = Task::NO_TASK;
			guiPresenter->newStatusMessage(message);
			guiPresenter->endOfTask();
		}
	}
	void FaceSynthesizing::captureData()
	{
		guiPresenter->newStatusMessage("Capture Data Pairs");
		bool overwrite = true;
		if (fileSystem->doesCaptureExists(captureDataInfo->name)) {
			std::cout << "Data Already Exists" << std::endl;
			dataAlreadyExistsResult = DataAlreadyExistsResult::No_Result;
			guiPresenter->dataAlreadyExistsUserPrompt();

			std::unique_lock<std::mutex> lock(threadLock);
			dataAlreadyExistsCondition.wait(lock, [this]() {
				return currentTask != Task::CAPTURE || dataAlreadyExistsResult != DataAlreadyExistsResult::No_Result; });

			if (dataAlreadyExistsResult == DataAlreadyExistsResult::No_Result)
				return;

			overwrite = dataAlreadyExistsResult == DataAlreadyExistsResult::Overwrite;
		}

		fileSystem->prepareCaptureLocation(captureDataInfo, overwrite);

		int dataPairsToCapture = captureDataInfo->train_images + captureDataInfo->evaluation_images;
		float evalDataPairRate = computeEvalDataPairRate(dataPairsToCapture);

		int counter = overwrite ? 0 : fileSystem->countExistingCaptureDataPairs(captureDataInfo->name) - 1;
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
		std::unique_lock<std::mutex> lock(threadLock);
		currentTask = Task::NO_TASK;
		dataAlreadyExistsCondition.notify_all();
		lock.unlock();

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

		if (mainTaskThread.get() != nullptr && mainTaskThread->joinable())
			mainTaskThread->join();
	}
	void FaceSynthesizing::setDataAlreadyExistsPromptResult(DataAlreadyExistsResult result)
	{
		std::unique_lock<std::mutex> lock(threadLock);
		dataAlreadyExistsResult = result;
		dataAlreadyExistsCondition.notify_all();
	}

	void FaceSynthesizing::noImageAvailable(ImageType imageType)
	{
		guiPresenter->clearImage(imageType);
	}
	void FaceSynthesizing::visualizeImage(std::shared_ptr<Image> image)
	{
		guiPresenter->visualizeImage(image);
	}

	void FaceSynthesizing::asyncGUIRequest(GUIRequest requestType)
	{
		switch (requestType) {
		case GUIRequest::All_Capture_Names:
			minorTasksThreadPool->addJob(std::bind(&FaceSynthesizing::processAllCaptureNamesRequest, this));
			break;
		case GUIRequest::All_Training_Dataset_Names:
			minorTasksThreadPool->addJob(std::bind(&FaceSynthesizing::processAllTrainingDatasetNames, this));
			break;
		case GUIRequest::All_Checkpoint_Names:
			minorTasksThreadPool->addJob(std::bind(&FaceSynthesizing::processAllCheckpointNames, this));
			break;
		default:
			throw "Unknown/Not implemented GUIRequest.";
		}
	}

	void FaceSynthesizing::processAllCaptureNamesRequest()
	{
		std::vector<std::string> allNames = fileSystem->getAllExistingCaptureNames();
		guiPresenter->presentAllCaptureNames(allNames);
	}
	void FaceSynthesizing::processAllTrainingDatasetNames()
	{
		std::vector<std::string> allNames = fileSystem->getAllExistingDatasetNames();

		// remove datasets without training data
		for (auto it = allNames.begin(); it != allNames.end(); it++) {
			if (!fileSystem->datasetContainsTrainingData(*it))
				allNames.erase(it--);
		}

		guiPresenter->presentTrainingDatasetNames(allNames);
	}
	void FaceSynthesizing::processAllCheckpointNames()
	{
		std::vector<std::string> allNames = fileSystem->getAllExistingCheckpointNames();
		//guiPresenter->presentAllCheckpointNames(allNames);
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

