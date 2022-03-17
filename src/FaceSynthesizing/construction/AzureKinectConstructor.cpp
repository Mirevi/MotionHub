#include "AzureKinectConstructor.h"

namespace facesynthesizing::construction {
	AzureKinectConstructor::AzureKinectConstructor()
	{
		azureKinectCamera = std::make_shared<azurekinect::AzureKinectCamera>();
		camera = azureKinectCamera;
	}
}
