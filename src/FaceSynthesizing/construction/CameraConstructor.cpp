#include "CameraConstructor.h"

namespace facesynthesizing::construction {
	std::shared_ptr<camera::RGBDICamera> CameraConstructor::getCamera()
	{
		return camera;
	}
}
