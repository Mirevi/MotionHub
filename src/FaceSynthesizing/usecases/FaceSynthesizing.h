#pragma once

#include "FaceSynthesizingDataStructures.h"
#include "FaceSynthesizingGUIPresenter.h"
#include "FaceSynthesizingGUIInteractor.h"
#include "FaceSynthesizingCamera.h"
#include "FaceSynthesizingProjectInteractor.h"
#include "FaceSynthesizingProject.h"
#include "FaceSynthesizingFileSystem.h"

#include <memory>
#include <thread>
#include <cmath>

namespace facesynthesizing::domain::usecases {

	class FaceSynthesizing : 
		public FaceSynthesizingGUIInteractor, 
		public FaceSynthesizingProjectInteractor {
	public: 
		~FaceSynthesizing();
		void startInitializeCameraTask();
		void startCaptureDataTask(std::shared_ptr<CaptureDataInformation> captureDataInfo);
		void cancelTask();
		void noImageAvailable(ImageType imageType);
		void visualizeImage(std::shared_ptr<Image> image);

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
		std::unique_ptr<std::thread> taskThread;

		bool cameraIsInitiated = false;

		std::shared_ptr<CaptureDataInformation> captureDataInfo;
	};
}