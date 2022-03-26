#pragma once

#include "../usecases/FaceSynthesizingEnums.h"
#include "PythonIncludeBoost.h"

namespace usecases = facesynthesizing::domain::usecases;
namespace py = boost::python;

namespace facesynthesizing::infrastructure::python {
	const std::string boundingBoxAlgorithmToPythonName(usecases::BoundingBoxAlgorithm algorithm);
	const std::string faceAlignmentAlgorithmToPythonName(usecases::FaceAlignmentAlgorithm algorithm);
	const std::string eyeTrackingAlgorithmToPythonName(usecases::EyeTrackingAlgorithm algorithm);
	const std::string faceTrackingAlgorithmToPythonName(usecases::FaceTrackingAlgorithm algorithm);
	const std::string depthFillingAlgorithmToPythonName(usecases::DepthFillingAlgorithm algorithm);
	const std::string colorFormatToPythonName(usecases::ColorFormat format);
	const std::string modelTypeToPythonName(usecases::ModelType type);
	const std::string generatorArchitectureToPythonName(usecases::GeneratorArchitecture architecture, int imageSize);
	const std::string discriminatorArchitectureToPythonName(usecases::DiscriminatorArchitecture architecture);
	const std::string ganModeToPythonName(usecases::GANMode mode);
	const std::string normalizationLayerTypeToPythonName(usecases::NormalizationLayerType type);
	const std::string learningRatePolicyToPythonName(usecases::LearningRatePolicy policy);
	const std::string weightInitTypeToPythonName(usecases::WeightInitType type);
	const std::string mappingNetworkToPythonName(usecases::MappingNetwork network);
	const usecases::ImageType pyImageIdentifierToImageType(std::string identifier);
}