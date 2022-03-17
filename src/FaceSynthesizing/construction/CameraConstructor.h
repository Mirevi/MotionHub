#pragma once

#include "./Constructor.h"
#include "../camera/RGBDICamera.h"

#include <memory>

namespace camera = facesynthesizing::domain::adapters::camera;

namespace facesynthesizing::construction {
	class CameraConstructor: public Constructor {
	public:
		std::shared_ptr<camera::RGBDICamera> getCamera();
	protected:
		std::shared_ptr<camera::RGBDICamera> camera;
	};
}
