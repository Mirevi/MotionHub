#pragma once

#include "CameraConstructor.h"
#include "../camera/AzureKinectCamera.h"

namespace azurekinect = facesynthesizing::infrastructure::azurekinect;

namespace facesynthesizing::construction {
	class AzureKinectConstructor: public CameraConstructor {
	public:
		AzureKinectConstructor();
	protected:
		std::shared_ptr<azurekinect::AzureKinectCamera> azureKinectCamera;
	};
}