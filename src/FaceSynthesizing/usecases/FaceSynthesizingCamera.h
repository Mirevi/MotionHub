#pragma once

#include "../usecases/FaceSynthesizingDataStructures.h"

#include <memory>

namespace facesynthesizing::domain::usecases {
	class FaceSynthesizingCamera {
	public:
		virtual void initiate() = 0;
		virtual void close() = 0;
		virtual void captureImagePair() = 0;
		virtual std::shared_ptr<Image> getColorImage() = 0;
		virtual std::shared_ptr<Image> getDepthImage() = 0;
		virtual std::shared_ptr<Image> getInfraredImage() = 0;
	};
}