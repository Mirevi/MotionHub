#pragma once

#include <string>
#include <map>
#include <list>

namespace facesynthesizing::domain::usecases {
	enum class BoundingBoxAlgorithm {
		SynergyNet,
	};
	enum class FaceAlignmentAlgorithm {
		SynergyNet,
	};

	const std::list<BoundingBoxAlgorithm> allBoundingBoxAlgorithms();
	const std::string boundingBoxAlgorithmToString(BoundingBoxAlgorithm algorithm);
	const BoundingBoxAlgorithm stringToBoundingBoxAlgorithm(std::string algorithmName);

	const std::list<FaceAlignmentAlgorithm> allFaceAlignmentAlgorithm();
	const std::string faceAlignmentAlgorithmToString(FaceAlignmentAlgorithm algorithm);
	const FaceAlignmentAlgorithm stringToFaceAlignmentAlgorithm(std::string algorithmName);

	enum class Task {
		NO_TASK,
		INITIATE_CAMERA,
		CAPTURE,
	};

	enum class ImageType {
		Unknown,
		Camera_Color,
		Camera_Depth,
		Camera_Infrared,
		Capture_FaceAlignment,
	};
}