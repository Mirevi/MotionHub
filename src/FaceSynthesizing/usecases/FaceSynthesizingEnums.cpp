#include "FaceSynthesizingEnums.h"

#include <iostream>

namespace facesynthesizing::domain::usecases {
	template<class T>
	const std::string getNameFromMapping(T enumValue, std::map<T, std::string> mapping) {
		auto it = mapping.find(enumValue);

		if (it == mapping.end())
			throw std::exception("Unknow Enum Value!");

		return it->second;
	};
	template<class T>
	const T getEnumValueFromMapping(std::string name, std::map<std::string, T> mapping) {
		auto it = mapping.find(name);

		if (it == mapping.end())
			throw std::exception("Unknow Name!");

		return it->second;
	};

	const std::list<BoundingBoxAlgorithm> allBoundingBoxAlgorithms() {
		return {
			BoundingBoxAlgorithm::SynergyNet,
		};
	}
	const std::string boundingBoxAlgorithmToString(BoundingBoxAlgorithm algorithm)
	{
		const std::map<BoundingBoxAlgorithm, std::string> mapping = {
			{BoundingBoxAlgorithm::SynergyNet, "SynergyNet"},
		};
		return getNameFromMapping<BoundingBoxAlgorithm>(algorithm, mapping);
	}
	const BoundingBoxAlgorithm stringToBoundingBoxAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, BoundingBoxAlgorithm> mapping = {
			{"SynergyNet", BoundingBoxAlgorithm::SynergyNet},
		};
		return getEnumValueFromMapping<BoundingBoxAlgorithm>(algorithmName, mapping);
	}

	const std::list<FaceAlignmentAlgorithm> allFaceAlignmentAlgorithm()
	{
		return {
			FaceAlignmentAlgorithm::SynergyNet,
		};
	}
	const std::string faceAlignmentAlgorithmToString(FaceAlignmentAlgorithm algorithm)
	{
		const std::map<FaceAlignmentAlgorithm, std::string> mapping = {
			{FaceAlignmentAlgorithm::SynergyNet, "SynergyNet"},
		};
		return getNameFromMapping<FaceAlignmentAlgorithm>(algorithm, mapping);
	}
	const FaceAlignmentAlgorithm stringToFaceAlignmentAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, FaceAlignmentAlgorithm> mapping = {
			{"SynergyNet", FaceAlignmentAlgorithm::SynergyNet},
		};
		return getEnumValueFromMapping<FaceAlignmentAlgorithm>(algorithmName, mapping);
	}
	
	const std::list<EyeTrackingAlgorithm> allEyeTrackingAlgorithms()
	{
		return {
			EyeTrackingAlgorithm::Gradient_Based,
			EyeTrackingAlgorithm::Infrared,
		};
	}
	const std::string eyeTrackingAlgorithmToString(EyeTrackingAlgorithm algorithm)
	{
		const std::map<EyeTrackingAlgorithm, std::string> mapping = {
			{EyeTrackingAlgorithm::Gradient_Based, "Gradient Based"},
			{EyeTrackingAlgorithm::Infrared, "Infrared"},
		};
		return getNameFromMapping<EyeTrackingAlgorithm>(algorithm, mapping);
	}
	const EyeTrackingAlgorithm stringToEyeTrackingAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, EyeTrackingAlgorithm> mapping = {
			{"Gradient Based", EyeTrackingAlgorithm::Gradient_Based},
			{"Infrared", EyeTrackingAlgorithm::Infrared},
		};
		return getEnumValueFromMapping<EyeTrackingAlgorithm>(algorithmName, mapping);
	}

	const std::list<FaceTrackingAlgorithm> allFaceTrackingAlgorithms()
	{
		return {
			FaceTrackingAlgorithm::FAN,
			FaceTrackingAlgorithm::dlib,
		};
	}
	const std::string faceTrackingAlgorithmToString(FaceTrackingAlgorithm algorithm)
	{
		const std::map<FaceTrackingAlgorithm, std::string> mapping = {
			{FaceTrackingAlgorithm::FAN, "FAN"},
			{FaceTrackingAlgorithm::dlib, "dlib"},
		};
		return getNameFromMapping<FaceTrackingAlgorithm>(algorithm, mapping);
	}
	const FaceTrackingAlgorithm stringToFaceTrackingAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, FaceTrackingAlgorithm> mapping = {
			{"FAN", FaceTrackingAlgorithm::FAN},
			{"dlib", FaceTrackingAlgorithm::dlib},
		};
		return getEnumValueFromMapping<FaceTrackingAlgorithm>(algorithmName, mapping);
	}

	const std::list<DepthFillingAlgorithm> allDepthFillingAlgorithms()
	{
		return {
			DepthFillingAlgorithm::FDCBOP,
		};
	}
	const std::string depthFillingAlgorithmToString(DepthFillingAlgorithm algorithm)
	{
		const std::map<DepthFillingAlgorithm, std::string> mapping = {
			{DepthFillingAlgorithm::FDCBOP, "FDCBOP"},
		};
		return getNameFromMapping<DepthFillingAlgorithm>(algorithm, mapping);
	}
	const DepthFillingAlgorithm stringToDepthFillingAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, DepthFillingAlgorithm> mapping = {
			{"FDCBOP", DepthFillingAlgorithm::FDCBOP},
		};
		return getEnumValueFromMapping<DepthFillingAlgorithm>(algorithmName, mapping);
	}
	
	const std::list<ColorFormat> allColorFormats()
	{
		return {
			ColorFormat::RGB,
			ColorFormat::BGR,
		};
	}
	const std::string colorFormatToString(ColorFormat format)
	{
		const std::map<ColorFormat, std::string> mapping = {
			{ColorFormat::RGB, "RGB"},
			{ColorFormat::BGR, "BGR"},
		};
		return getNameFromMapping<ColorFormat>(format, mapping);
	}
	const ColorFormat stringToColorFormat(std::string formatName)
	{
		const std::map<std::string, ColorFormat> mapping = {
			{"RGB", ColorFormat::RGB},
			{"BGR", ColorFormat::BGR},
		};
		return getEnumValueFromMapping<ColorFormat>(formatName, mapping);
	}

	const std::list<ModelType> allModelTypes()
	{
		return {
			ModelType::L1,
			ModelType::Pix2Pix,
			ModelType::Pix2Pix_Extended,
		};
	}
	const std::string modelTypeToString(ModelType type)
	{
		const std::map<ModelType, std::string> mapping = {
			{ModelType::L1, "L1"},
			{ModelType::Pix2Pix, "Pix2Pix"},
			{ModelType::Pix2Pix_Extended, "Pix2Pix_Extended"},
		};
		return getNameFromMapping<ModelType>(type, mapping);
	}
	const ModelType stringToModelType(std::string typeName)
	{
		const std::map<std::string, ModelType> mapping = {
			{"L1", ModelType::L1},
			{"Pix2Pix", ModelType::Pix2Pix},
			{"Pix2Pix_Extended", ModelType::Pix2Pix_Extended},
		};
		return getEnumValueFromMapping<ModelType>(typeName, mapping);
	}

	const std::list<GeneratorArchitecture> allGeneratorArchitectures()
	{
		return {
			GeneratorArchitecture::UNet,
			GeneratorArchitecture::ResNet_6_Blocks,
			GeneratorArchitecture::ResNet_9_Blocks,
		};
	}
	const std::string generatorArchitectureToString(GeneratorArchitecture architecture)
	{
		const std::map<GeneratorArchitecture, std::string> mapping = {
			{GeneratorArchitecture::UNet, "UNet"},
			{GeneratorArchitecture::ResNet_6_Blocks, "ResNet 6 Blocks"},
			{GeneratorArchitecture::ResNet_9_Blocks, "ResNet 9 Blocks"},
		};
		return getNameFromMapping<GeneratorArchitecture>(architecture, mapping);
	}
	const GeneratorArchitecture stringToGeneratorArchitecture(std::string architectureName)
	{
		const std::map<std::string, GeneratorArchitecture> mapping = {
			{"UNet", GeneratorArchitecture::UNet},
			{"ResNet 6 Blocks", GeneratorArchitecture::ResNet_6_Blocks},
			{"ResNet 9 Blocks", GeneratorArchitecture::ResNet_9_Blocks},
		};
		return getEnumValueFromMapping<GeneratorArchitecture>(architectureName, mapping);
	}

	const std::list<DiscriminatorArchitecture> allDiscriminatorArchitectures()
	{
		return {
			DiscriminatorArchitecture::Pixel,
			DiscriminatorArchitecture::N_Layer,
			DiscriminatorArchitecture::Multiscale,
		};
	}
	const std::string discriminatorArchitectureToString(DiscriminatorArchitecture architecture)
	{
		const std::map<DiscriminatorArchitecture, std::string> mapping = {
			{DiscriminatorArchitecture::Pixel, "Pixel"},
			{DiscriminatorArchitecture::N_Layer, "N_Layer"},
			{DiscriminatorArchitecture::Multiscale, "Multiscale"},
		};
		return getNameFromMapping<DiscriminatorArchitecture>(architecture, mapping);
	}
	const DiscriminatorArchitecture stringToDiscriminatorArchitecture(std::string architectureName)
	{
		const std::map<std::string, DiscriminatorArchitecture> mapping = {
			{"Pixel", DiscriminatorArchitecture::Pixel},
			{"N_Layer", DiscriminatorArchitecture::N_Layer},
			{"Multiscale", DiscriminatorArchitecture::Multiscale},
		};
		return getEnumValueFromMapping<DiscriminatorArchitecture>(architectureName, mapping);
	}

	const std::list<GANMode> allGANModes()
	{
		return {
			GANMode::Vanilla,
			GANMode::LSGAN,
		};
	}
	const std::string ganModeToString(GANMode mode)
	{
		const std::map<GANMode, std::string> mapping = {
			{GANMode::Vanilla, "Vanilla"},
			{GANMode::LSGAN, "LSGAN"},
		};
		return getNameFromMapping<GANMode>(mode, mapping);
	}
	const GANMode stringToGANMode(std::string modeName)
	{
		const std::map<std::string, GANMode> mapping = {
			{"Vanilla", GANMode::Vanilla},
			{"LSGAN", GANMode::LSGAN},
		};
		return getEnumValueFromMapping<GANMode>(modeName, mapping);
	}

	const std::list<NormalizationLayerType> allNormalizationLayerTypes()
	{
		return {
			NormalizationLayerType::None,
			NormalizationLayerType::Batch,
			NormalizationLayerType::Instance,
		};
	}
	const std::string normalizationLayerTypeToString(NormalizationLayerType type)
	{
		const std::map<NormalizationLayerType, std::string> mapping = {
			{NormalizationLayerType::None, "None"},
			{NormalizationLayerType::Batch, "Batch"},
			{NormalizationLayerType::Instance, "Instance"},
		};
		return getNameFromMapping<NormalizationLayerType>(type, mapping);
	}
	const NormalizationLayerType stringToNormalizationLayerType(std::string typeName)
	{
		const std::map<std::string, NormalizationLayerType> mapping = {
			{"None", NormalizationLayerType::None},
			{"Batch", NormalizationLayerType::Batch},
			{"Instance", NormalizationLayerType::Instance},
		};
		return getEnumValueFromMapping<NormalizationLayerType>(typeName, mapping);
	}

	const std::list<LearningRatePolicy> allLearningRatePolicies()
	{
		return {
			LearningRatePolicy::Linear,
			LearningRatePolicy::Step,
			LearningRatePolicy::Plateau,
			LearningRatePolicy::Cosine,
		};
	}
	const std::string learningRatePolicyToString(LearningRatePolicy policy)
	{
		const std::map<LearningRatePolicy, std::string> mapping = {
			{LearningRatePolicy::Linear, "Linear"},
			{LearningRatePolicy::Step, "Step"},
			{LearningRatePolicy::Plateau, "Plateau"},
			{LearningRatePolicy::Cosine, "Cosine"},
		};
		return getNameFromMapping<LearningRatePolicy>(policy, mapping);
	}
	const LearningRatePolicy stringToLearningRatePolicy(std::string policyName)
	{
		const std::map<std::string, LearningRatePolicy> mapping = {
			{"Linear", LearningRatePolicy::Linear},
			{"Step", LearningRatePolicy::Step},
			{"Plateau", LearningRatePolicy::Plateau},
			{"Cosine", LearningRatePolicy::Cosine},
		};
		return getEnumValueFromMapping<LearningRatePolicy>(policyName, mapping);
	}

	const std::list<WeightInitType> allWeightInitTypes()
	{
		return {
			WeightInitType::Normal,
			WeightInitType::Xavier,
			WeightInitType::Kaimling,
			WeightInitType::Orthogonal,
		};
	}
	const std::string weightInitTypeToString(WeightInitType type)
	{
		const std::map<WeightInitType, std::string> mapping = {
			{WeightInitType::Normal, "Normal"},
			{WeightInitType::Xavier, "Xavier"},
			{WeightInitType::Kaimling, "Kaimling"},
			{WeightInitType::Orthogonal, "Orthogonal"},
		};
		return getNameFromMapping<WeightInitType>(type, mapping);
	}
	const WeightInitType stringToWeightInitType(std::string typeName)
	{
		const std::map<std::string, WeightInitType> mapping = {
			{"Normal", WeightInitType::Normal},
			{"Xavier", WeightInitType::Xavier},
			{"Kaimling", WeightInitType::Kaimling},
			{"Orthogonal", WeightInitType::Orthogonal},
		};
		return getEnumValueFromMapping<WeightInitType>(typeName, mapping);
	}

	const std::list<MappingNetwork> allMappingNetworks()
	{
		return {
			MappingNetwork::MLP_with_Conv_512,
			MappingNetwork::Residual,
			MappingNetwork::MLP_and_Residual_512,
		};
	}
	const std::string mappingNetworkToString(MappingNetwork network)
	{
		const std::map<MappingNetwork, std::string> mapping = {
			{MappingNetwork::MLP_with_Conv_512, "MLP_with_Conv_512"},
			{MappingNetwork::Residual, "Residual"},
			{MappingNetwork::MLP_and_Residual_512, "MLP_and_Residual_512"},
		};
		return getNameFromMapping<MappingNetwork>(network, mapping);
	}
	const MappingNetwork stringToMappingNetwork(std::string networkName)
	{
		const std::map<std::string, MappingNetwork> mapping = {
			{"MLP_with_Conv_512", MappingNetwork::MLP_with_Conv_512},
			{"Residual", MappingNetwork::Residual},
			{"MLP_and_Residual_512", MappingNetwork::MLP_and_Residual_512},
		};
		return getEnumValueFromMapping<MappingNetwork>(networkName, mapping);
	}
	const std::list<ImageType> allImageType()
	{
		return {
			ImageType::Unknown,
			ImageType::Camera_Color,
			ImageType::Camera_Depth,
			ImageType::Camera_Infrared,
			ImageType::Capture_FaceAlignment,
			ImageType::Convert_Face_Bounding_box,
			ImageType::Convert_Landmarks,
			ImageType::Dataset_Color,
			ImageType::Dataset_Depth,
			ImageType::Dataset_FLM,
			ImageType::Synthesized_Color,
			ImageType::Synthesized_Depth,
		};
	}
	const std::string imageTypeToString(ImageType type)
	{
		const std::map<ImageType, std::string> mapping = {
			{ImageType::Unknown, "Unknown"},
			{ImageType::Camera_Color, "Camera Color"},
			{ImageType::Camera_Depth, "Camera Depth"},
			{ImageType::Camera_Infrared, "Camera Infrared"},
			{ImageType::Capture_FaceAlignment, "Capture FaceAlignment"},
			{ImageType::Convert_Face_Bounding_box, "Convert Face Bounding Box"},
			{ImageType::Convert_Landmarks, "Convert Landmarks"},
			{ImageType::Convert_Depth_Holes, "Convert Depth Holes"},
			{ImageType::Dataset_Color, "Dataset Color"},
			{ImageType::Dataset_Depth, "Dataset Depth"},
			{ImageType::Dataset_FLM, "Dataset FLM"},
			{ImageType::Synthesized_Color, "Synthesized Color"},
			{ImageType::Synthesized_Depth, "Synthesized Depth"},
		};
		return getNameFromMapping<ImageType>(type, mapping);
	}
	const ImageType stringToImageType(std::string typeName)
	{
		const std::map<std::string, ImageType> mapping = {
			{"Unknown", ImageType::Unknown},
			{"Camera Color", ImageType::Camera_Color},
			{"Camera Depth", ImageType::Camera_Depth},
			{"Camera Infrared", ImageType::Camera_Infrared},
			{"Capture FaceAlignment", ImageType::Capture_FaceAlignment},
			{"Convert Face Bounding Box", ImageType::Convert_Face_Bounding_box},
			{"Convert Landmarks", ImageType::Convert_Landmarks},
			{"Convert Depth Holes", ImageType::Convert_Depth_Holes},
			{"Dataset Color", ImageType::Dataset_Color},
			{"Dataset Depth", ImageType::Dataset_Depth},
			{"Dataset FLM", ImageType::Dataset_FLM},
			{"Synthesized Color", ImageType::Synthesized_Color},
			{"Synthesized Depth", ImageType::Synthesized_Depth},
		};

		return getEnumValueFromMapping<ImageType>(typeName, mapping);
	}
}