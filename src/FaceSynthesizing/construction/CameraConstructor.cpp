#include "CameraConstructor.h"

namespace facesynthesizing::construction {
	std::shared_ptr<camera::CameraRGBDI> CameraConstructor::getCamera()
	{
		return camera;
	}
}
