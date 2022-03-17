#pragma once

#include "FaceSynthesizingDataStructures.h"

#include <string>

namespace facesynthesizing::domain::usecases {
	class FaceSynthesizingGUIPresenter {
	public:
		virtual void clearImage(ImageType imageType) = 0;
		virtual void visualizeImage(std::shared_ptr<Image> image) = 0;

		virtual void cameraInitializationStarted() = 0;
		virtual void cameraIsInitialized() = 0;
		virtual void dataPairCaptureStarted() = 0;
		virtual void endOfTask() = 0;

		virtual void addNote(std::string message) = 0;
		virtual void newStatusMessage(std::string message) = 0;
		virtual void updateStatusMessage(std::string message) = 0;
		virtual void statusMessageToNotes() = 0;
	};
}