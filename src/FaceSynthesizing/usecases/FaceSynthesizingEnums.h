#pragma once

#include <string>
#include <map>
#include <list>

namespace facesynthesizing::domain::usecases {
	template<class T>
	const std::string getNameFromMapping(T enumValue, std::map<T, std::string> mapping);
	template<class T>
	const T getEnumValueFromMapping(std::string name, std::map<std::string, T> mapping);

	enum class BoundingBoxAlgorithm {
		SynergyNet,
	};
	const std::list<BoundingBoxAlgorithm> allBoundingBoxAlgorithms();
	const std::string boundingBoxAlgorithmToString(BoundingBoxAlgorithm algorithm);
	const BoundingBoxAlgorithm stringToBoundingBoxAlgorithm(std::string algorithmName);

	enum class FaceAlignmentAlgorithm {
		SynergyNet,
	};
	const std::list<FaceAlignmentAlgorithm> allFaceAlignmentAlgorithm();
	const std::string faceAlignmentAlgorithmToString(FaceAlignmentAlgorithm algorithm);
	const FaceAlignmentAlgorithm stringToFaceAlignmentAlgorithm(std::string algorithmName);

	enum class EyeTrackingAlgorithm {
		Gradient_Based,
		Infrared,
	};
	const std::list<EyeTrackingAlgorithm> allEyeTrackingAlgorithms();
	const std::string eyeTrackingAlgorithmToString(EyeTrackingAlgorithm algorithm);
	const EyeTrackingAlgorithm stringToEyeTrackingAlgorithm(std::string algorithmName);

	enum class FaceTrackingAlgorithm {
		FAN,
		dlib,
	};
	const std::list<FaceTrackingAlgorithm> allFaceTrackingAlgorithms();
	const std::string faceTrackingAlgorithmToString(FaceTrackingAlgorithm algorithm);
	const FaceTrackingAlgorithm stringToFaceTrackingAlgorithm(std::string algorithmName);

	enum class DepthFillingAlgorithm {
		FDCBOP,
	};
	const std::list<DepthFillingAlgorithm> allDepthFillingAlgorithms();
	const std::string depthFillingAlgorithmToString(DepthFillingAlgorithm algorithm);
	const DepthFillingAlgorithm stringToDepthFillingAlgorithm(std::string algorithmName);

	enum class ColorFormat {
		RGB,
		BGR,
	};
	const std::list<ColorFormat> allColorFormats();
	const std::string colorFormatToString(ColorFormat format);
	const ColorFormat stringToColorFormat(std::string formatName);

	enum class ModelType {
		L1,
		Pix2Pix,
		Pix2Pix_Extended,
	};
	const std::list<ModelType> allModelTypes();
	const std::string modelTypeToString(ModelType type);
	const ModelType stringToModelType(std::string typeName);

	enum class GeneratorArchitecture {
		UNet,
		ResNet_6_Blocks,
		ResNet_9_Blocks,
	};
	const std::list<GeneratorArchitecture> allGeneratorArchitectures();
	const std::string generatorArchitectureToString(GeneratorArchitecture architecture);
	const GeneratorArchitecture stringToGeneratorArchitecture(std::string architectureName);

	enum class DiscriminatorArchitecture {
		Pixel,
		N_Layer,
		Multiscale,
	};
	const std::list<DiscriminatorArchitecture> allDiscriminatorArchitectures();
	const std::string discriminatorArchitectureToString(DiscriminatorArchitecture architecture);
	const DiscriminatorArchitecture stringToDiscriminatorArchitecture(std::string architectureName);

	enum class GANMode {
		Vanilla,
		LSGAN,
	};
	const std::list<GANMode> allGANModes();
	const std::string ganModeToString(GANMode mode);
	const GANMode stringToGANMode(std::string modeName);

	enum class NormalizationLayerType {
		Batch,
		Instance,
		None,
	};
	const std::list<NormalizationLayerType> allNormalizationLayerTypes();
	const std::string normalizationLayerTypeToString(NormalizationLayerType type);
	const NormalizationLayerType stringToNormalizationLayerType(std::string typeName);

	enum class LearningRatePolicy {
		Linear,
		Step,
		Plateau,
		Cosine,
	};
	const std::list<LearningRatePolicy> allLearningRatePolicies();
	const std::string learningRatePolicyToString(LearningRatePolicy policy);
	const LearningRatePolicy stringToLearningRatePolicy(std::string policyName);

	enum class WeightInitType {
		Normal,
		Xavier,
		Kaimling,
		Orthogonal,
	};
	const std::list<WeightInitType> allWeightInitTypes();
	const std::string weightInitTypeToString(WeightInitType type);
	const WeightInitType stringToWeightInitType(std::string typeName);

	enum class MappingNetwork {
		MLP_with_Conv_512,
		Residual,
		MLP_and_Residual_512,
	};
	const std::list<MappingNetwork> allMappingNetworks();
	const std::string mappingNetworkToString(MappingNetwork network);
	const MappingNetwork stringToMappingNetwork(std::string networkName);

	enum class ImageType {
		Unknown,
		Camera_Color,
		Camera_Depth,
		Camera_Infrared,
		Capture_FaceAlignment,
		Convert_Face_Bounding_box,
		Convert_Landmarks,
		Convert_Depth_Holes,
		Dataset_Color,
		Dataset_Depth,
		Dataset_FLM,
		Synthesized_Color,
		Synthesized_Depth,
	};
	const std::list<ImageType> allImageType();
	const std::string imageTypeToString(ImageType type);
	const ImageType stringToImageType(std::string typeName);

	enum class Task {
		NO_TASK,
		INITIATE_CAMERA,
		CAPTURE,
		CONVERT,
		TRAINING,
	};
}