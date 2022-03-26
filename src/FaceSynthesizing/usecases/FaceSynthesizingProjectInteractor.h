#pragma once

#include "FaceSynthesizingDataStructures.h"

#include <memory>

namespace facesynthesizing::domain::usecases {
	class FaceSynthesizingProjectInteractor {
	public:
		virtual void noImageAvailable(ImageType imageType) = 0;
		virtual void visualizeImage(std::shared_ptr<Image> image) = 0;
		virtual void newProjectStatusMessage(std::string statusMessage) = 0;
		virtual void updateProjectStatusMessage(std::string statusMessage) = 0;
	};
}