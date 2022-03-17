#pragma once

#include "FaceSynthesizingDataStructures.h"

#include <memory>

namespace facesynthesizing::domain::usecases {
	class FaceSynthesizingGUIInteractor {
	public:
		virtual void startInitializeCameraTask() = 0;
		virtual void startCaptureDataTask(std::shared_ptr<CaptureDataInformation> captureDataInfo) = 0;
		virtual void cancelTask() = 0;
	};
}