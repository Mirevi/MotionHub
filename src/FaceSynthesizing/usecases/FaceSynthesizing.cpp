#include "FaceSynthesizing.h"

#include <iostream>

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
		std::unique_lock<std::mutex> lockTaskStart(taskStartLock);
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
			guiPresenter->newStatusMessage("Initialize Camera");

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
		std::unique_lock<std::mutex> lockTaskStart(taskStartLock);
		if (!cameraIsInitiated) {
			std::cout << "Cant Capture Data when another Task is currently executed!" << std::endl;
			guiPresenter->newStatusMessage("Cant Capture Data when another Task is currently executed!");
			return;
		}
		this->captureDataInfo = captureDataInfo;
		currentTask = Task::NO_TASK;
		waitForTaskThreadTermination();

		currentTask = Task::CAPTURE;
		guiPresenter->dataPairCaptureStarted();
		mainTaskThread = std::make_unique<std::thread>(&FaceSynthesizing::captureDataThreadMethod, this);
	}
	void FaceSynthesizing::captureDataThreadMethod()
	{
		try {
			guiPresenter->newStatusMessage("Capture Data Pairs");

			captureData();

			if (currentTask == Task::CAPTURE)
				guiPresenter->newStatusMessage("Finished Data Pair Capture.");

			guiPresenter->endOfTask();
			camera->close();
			currentTask = Task::NO_TASK;
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
		faceSynthesizer->setCaptureInfos(captureDataInfo);

		bool overwrite = true;
		if (fileSystem->doesCaptureExists(captureDataInfo->name)) {
			std::cout << "Capture Already Exists" << std::endl;
			showDataAlreadyExistsPrompt();

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
	}
	float FaceSynthesizing::computeEvalDataPairRate(int imageCount)
	{
		if (captureDataInfo->evaluation_images > 0)
			return static_cast<float>(imageCount) / captureDataInfo->evaluation_images;
		else
			return static_cast<float>(INT_MAX);
	}

	void FaceSynthesizing::startConvertDataTask(std::shared_ptr<ConvertDataInformation> convertDataInfo)
	{
		std::unique_lock<std::mutex> lockTaskStart(taskStartLock);
		if (currentTask != Task::NO_TASK) {
			std::cout << "Cant start Converting Data when another Task is currently executed!" << std::endl;
			guiPresenter->newStatusMessage("Cant start Converting Data when another Task is currently executed!");
			return;
		}
		this->convertDataInfo = convertDataInfo;
		waitForTaskThreadTermination();

		currentTask = Task::CONVERT;
		guiPresenter->dataPairConversionStarted();
		mainTaskThread = std::make_unique<std::thread>(&FaceSynthesizing::convertDataThreadMethod, this);
	}
	void FaceSynthesizing::convertDataThreadMethod()
	{
		try {
			guiPresenter->newStatusMessage("Convert Data Pairs");

			convertData();

			if (currentTask == Task::CONVERT)
				guiPresenter->newStatusMessage("Finished Data Pair Conversion.");

			guiPresenter->endOfTask();
			currentTask = Task::NO_TASK;
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
			std::string message = "Exception during Convert Data Task: " + std::string(e.what());
			currentTask = Task::NO_TASK;
			guiPresenter->newStatusMessage(message);
			guiPresenter->endOfTask();
		}
	}
	void FaceSynthesizing::convertData()
	{
		if (fileSystem->doesDatasetExists(convertDataInfo->name)) {
			std::cout << "Dataset Already Exists" << std::endl;
			showDataAlreadyExistsPrompt();

			if (dataAlreadyExistsResult == DataAlreadyExistsResult::No_Result)
				return;
			convertDataInfo->overwrite = dataAlreadyExistsResult == DataAlreadyExistsResult::Overwrite;
			convertDataInfo->continueProcess = dataAlreadyExistsResult == DataAlreadyExistsResult::Continue;
		}
		else {
			convertDataInfo->overwrite = true;
			convertDataInfo->continueProcess = false;
		}

		faceSynthesizer->setConvertInfos(convertDataInfo);
		faceSynthesizer->convertData();
	}

	void FaceSynthesizing::startTrainingTask(std::shared_ptr<TrainingInformation> trainingInfo)
	{
		std::unique_lock<std::mutex> lockTaskStart(taskStartLock);
		if (currentTask != Task::NO_TASK) {
			std::cout << "Cant start Training when another Task is currently executed!" << std::endl;
			guiPresenter->newStatusMessage("Cant initialize Camera when another Task is currently executed!");
			return;
		}
		this->trainingInfo = trainingInfo;
		waitForTaskThreadTermination();

		currentTask = Task::TRAINING;
		guiPresenter->trainingStarted();
		mainTaskThread = std::make_unique<std::thread>(&FaceSynthesizing::trainingThreadMethod, this);
	}
	void FaceSynthesizing::trainingThreadMethod()
	{
		try {
			guiPresenter->newStatusMessage("Train Model");

			train();

			guiPresenter->endOfTask();
			currentTask = Task::NO_TASK;
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
			std::string message = "Exception during Training Task: " + std::string(e.what());
			currentTask = Task::NO_TASK;
			guiPresenter->newStatusMessage(message);
			guiPresenter->endOfTask();
		}
	}
	void FaceSynthesizing::train()
	{
		if (fileSystem->doesCheckpointExists(trainingInfo->name)) {
			std::cout << "Checkpoint Already Exists" << std::endl;
			showDataAlreadyExistsPrompt();

			if (dataAlreadyExistsResult == DataAlreadyExistsResult::No_Result)
				return;
			trainingInfo->overwrite = dataAlreadyExistsResult == DataAlreadyExistsResult::Overwrite;
			trainingInfo->continueProcess = dataAlreadyExistsResult == DataAlreadyExistsResult::Continue;
		}
		else {
			trainingInfo->overwrite = true;
			trainingInfo->continueProcess = false;
		}

		faceSynthesizer->setTrainInfos(trainingInfo);
		faceSynthesizer->train();
	}

	void FaceSynthesizing::showDataAlreadyExistsPrompt()
	{
		dataAlreadyExistsResult = DataAlreadyExistsResult::No_Result;
		guiPresenter->dataAlreadyExistsUserPrompt();

		std::unique_lock<std::mutex> lock(threadLock);
		dataAlreadyExistsCondition.wait(lock, [this]() {
			return currentTask == Task::NO_TASK || dataAlreadyExistsResult != DataAlreadyExistsResult::No_Result; });
	}

	void FaceSynthesizing::cancelTask()
	{
		std::unique_lock<std::mutex> lockTaskStart(taskStartLock);
		std::unique_lock<std::mutex> lock(threadLock);
		currentTask = Task::NO_TASK;
		dataAlreadyExistsCondition.notify_all();
		faceSynthesizer->requestExecutionStopAsync();
		lock.unlock();

		waitForTaskThreadTermination();

		faceSynthesizer->resetExecutionStop();
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
	void FaceSynthesizing::newProjectStatusMessage(std::string statusMessage)
	{
		guiPresenter->newStatusMessage(statusMessage);
	}
	void FaceSynthesizing::updateProjectStatusMessage(std::string statusMessage)
	{
		guiPresenter->updateStatusMessage(statusMessage);
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
		guiPresenter->presentAllCheckpointNames(allNames);
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

