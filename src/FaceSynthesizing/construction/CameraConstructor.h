#pragma once

#include "./Constructor.h"
#include "../camera/CameraRGBDI.h"

#include <memory>

namespace camera = facesynthesizing::domain::adapters::camera;

namespace facesynthesizing::construction {
	class CameraConstructor: public Constructor {
	public:
		std::shared_ptr<camera::CameraRGBDI> getCamera();
	protected:
		std::shared_ptr<camera::CameraRGBDI> camera;
	};
}
