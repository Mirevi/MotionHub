#pragma once

#include "FaceSynthesizingDataStructures.h"

#include <memory>

namespace facesynthesizing::domain::usecases {
	enum class DataAlreadyExistsResult {
		No_Result,
		Continue,
		Overwrite
	};

	enum class GUIRequest {
		All_Capture_Names,
		All_Training_Dataset_Names,
		All_Checkpoint_Names,
	};

	class FaceSynthesizingGUIInteractor {
	public:
		virtual void startInitializeCameraTask() = 0;
		virtual void startCaptureDataTask(std::shared_ptr<CaptureDataInformation> captureDataInfo) = 0;
		virtual void startConvertDataTask(std::shared_ptr<ConvertDataInformation> convertDataInfo) = 0;
		virtual void startTrainingTask(std::shared_ptr<TrainingInformation> trainingInfo) = 0;
		virtual void cancelTask() = 0;
		virtual void setDataAlreadyExistsPromptResult(DataAlreadyExistsResult result) = 0;

		virtual void asyncGUIRequest(GUIRequest requestType) = 0;
	};
}