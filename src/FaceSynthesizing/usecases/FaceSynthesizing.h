#pragma once

#include "FaceSynthesizingDataStructures.h"
#include "FaceSynthesizingGUIPresenter.h"
#include "FaceSynthesizingGUIInteractor.h"
#include "FaceSynthesizingCamera.h"
#include "FaceSynthesizingProjectInteractor.h"
#include "FaceSynthesizingProject.h"
#include "FaceSynthesizingFileSystem.h"
#include "MotionHubUtil/ThreadPool.h"

#include <memory>
#include <thread>
#include <mutex>
#include <cmath>
#include <functional>

namespace facesynthesizing::domain::usecases {

	class FaceSynthesizing : 
		public FaceSynthesizingGUIInteractor, 
		public FaceSynthesizingProjectInteractor {
	public: 
		FaceSynthesizing();
		~FaceSynthesizing();
		void startInitializeCameraTask();
		void startCaptureDataTask(std::shared_ptr<CaptureDataInformation> captureDataInfo);
		void cancelTask();
		void setDataAlreadyExistsPromptResult(DataAlreadyExistsResult result);
		void noImageAvailable(ImageType imageType);
		void visualizeImage(std::shared_ptr<Image> image);

		void asyncGUIRequest(GUIRequest requestType);
		void processAllCaptureNamesRequest();
		void processAllTrainingDatasetNames();
		void processAllCheckpointNames();

		// construction purpose
		void setGuiPresenter(std::shared_ptr<FaceSynthesizingGUIPresenter> guiPresenter);
		void setCamera(std::shared_ptr<FaceSynthesizingCamera> camera);
		void setFaceSynthesizer(std::shared_ptr<FaceSynthesizingProject> faceSynthesizer);
		void setFileSystem(std::shared_ptr<FaceSynthesizingFileSystem> fileSystem);
	private:
		void initializeCameraThreadMethod();
		void initializeCamera();
		void captureDataThreadMethod();
		void captureData();
		float computeEvalDataPairRate(int imageCount);
		void waitForTaskThreadTermination();

		std::shared_ptr<FaceSynthesizingGUIPresenter> guiPresenter;
		std::shared_ptr<FaceSynthesizingCamera> camera;
		std::shared_ptr<FaceSynthesizingProject> faceSynthesizer;
		std::shared_ptr<FaceSynthesizingFileSystem> fileSystem;

		Task currentTask = Task::NO_TASK;
		std::unique_ptr<std::thread> mainTaskThread;
		std::mutex threadLock;
		std::condition_variable dataAlreadyExistsCondition;
		std::unique_ptr<ThreadPool> minorTasksThreadPool;

		bool cameraIsInitiated = false;

		std::shared_ptr<CaptureDataInformation> captureDataInfo;
		DataAlreadyExistsResult dataAlreadyExistsResult = DataAlreadyExistsResult::No_Result;
	};
}