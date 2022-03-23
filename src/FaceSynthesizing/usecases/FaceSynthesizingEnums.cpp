#include "FaceSynthesizingEnums.h"

namespace facesynthesizing::domain::usecases {
	template<class T>
	const std::string getAlgorithmNameFromMapping(T algorithm, std::map<T, std::string> mapping) {
		auto   it = mapping.find(algorithm);

		if (it == mapping.end())
			throw "Unknow Algorithm!";

		return it->second;
	};
	template<class T>
	const T getAlgorithmFromMapping(std::string algorithmName, std::map<std::string, T> mapping) {
		auto   it = mapping.find(algorithmName);

		if (it == mapping.end())
			throw "Unknow Algorithm!";

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
		return getAlgorithmNameFromMapping<BoundingBoxAlgorithm>(algorithm, mapping);
	}
	const BoundingBoxAlgorithm stringToBoundingBoxAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, BoundingBoxAlgorithm> mapping = {
			{"SynergyNet", BoundingBoxAlgorithm::SynergyNet},
		};
		return getAlgorithmFromMapping<BoundingBoxAlgorithm>(algorithmName, mapping);
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
		return getAlgorithmNameFromMapping<FaceAlignmentAlgorithm>(algorithm, mapping);
	}
	const FaceAlignmentAlgorithm stringToFaceAlignmentAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, FaceAlignmentAlgorithm> mapping = {
			{"SynergyNet", FaceAlignmentAlgorithm::SynergyNet},
		};
		return getAlgorithmFromMapping<FaceAlignmentAlgorithm>(algorithmName, mapping);
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
		return getAlgorithmNameFromMapping<EyeTrackingAlgorithm>(algorithm, mapping);
	}
	const EyeTrackingAlgorithm stringToEyeTrackingAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, EyeTrackingAlgorithm> mapping = {
			{"Gradient Based", EyeTrackingAlgorithm::Gradient_Based},
			{"Infrared", EyeTrackingAlgorithm::Infrared},
		};
		return getAlgorithmFromMapping<EyeTrackingAlgorithm>(algorithmName, mapping);
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
		return getAlgorithmNameFromMapping<FaceTrackingAlgorithm>(algorithm, mapping);
	}
	const FaceTrackingAlgorithm stringToFaceTrackingAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, FaceTrackingAlgorithm> mapping = {
			{"FAN", FaceTrackingAlgorithm::FAN},
			{"dlib", FaceTrackingAlgorithm::dlib},
		};
		return getAlgorithmFromMapping<FaceTrackingAlgorithm>(algorithmName, mapping);
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
		return getAlgorithmNameFromMapping<DepthFillingAlgorithm>(algorithm, mapping);
	}
	const DepthFillingAlgorithm stringToDepthFillingAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, DepthFillingAlgorithm> mapping = {
			{"FDCBOP", DepthFillingAlgorithm::FDCBOP},
		};
		return getAlgorithmFromMapping<DepthFillingAlgorithm>(algorithmName, mapping);
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
		return getAlgorithmNameFromMapping<ColorFormat>(format, mapping);
	}
	const ColorFormat stringToColorFormat(std::string formatName)
	{
		const std::map<std::string, ColorFormat> mapping = {
			{"RGB", ColorFormat::RGB},
			{"BGR", ColorFormat::BGR},
		};
		return getAlgorithmFromMapping<ColorFormat>(formatName, mapping);
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
		return getAlgorithmNameFromMapping<ModelType>(type, mapping);
	}
	const ModelType stringToModelType(std::string typeName)
	{
		const std::map<std::string, ModelType> mapping = {
			{"L1", ModelType::L1},
			{"Pix2Pix", ModelType::Pix2Pix},
			{"Pix2Pix_Extended", ModelType::Pix2Pix_Extended},
		};
		return getAlgorithmFromMapping<ModelType>(typeName, mapping);
	}

	const std::list<GeneratorArchitecture> allGeneratorArchitectures()
	{
		return {
			GeneratorArchitecture::UNet,
			GeneratorArchitecture::ResNet,
		};
	}
	const std::string generatorArchitectureToString(GeneratorArchitecture architecture)
	{
		const std::map<GeneratorArchitecture, std::string> mapping = {
			{GeneratorArchitecture::UNet, "UNet"},
			{GeneratorArchitecture::ResNet, "ResNet"},
		};
		return getAlgorithmNameFromMapping<GeneratorArchitecture>(architecture, mapping);
	}
	const GeneratorArchitecture stringToGeneratorArchitecture(std::string architectureName)
	{
		const std::map<std::string, GeneratorArchitecture> mapping = {
			{"UNet", GeneratorArchitecture::UNet},
			{"ResNet", GeneratorArchitecture::ResNet},
		};
		return getAlgorithmFromMapping<GeneratorArchitecture>(architectureName, mapping);
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
		return getAlgorithmNameFromMapping<DiscriminatorArchitecture>(architecture, mapping);
	}
	const DiscriminatorArchitecture stringToDiscriminatorArchitecture(std::string architectureName)
	{
		const std::map<std::string, DiscriminatorArchitecture> mapping = {
			{"Pixel", DiscriminatorArchitecture::Pixel},
			{"N_Layer", DiscriminatorArchitecture::N_Layer},
			{"Multiscale", DiscriminatorArchitecture::Multiscale},
		};
		return getAlgorithmFromMapping<DiscriminatorArchitecture>(architectureName, mapping);
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
		return getAlgorithmNameFromMapping<GANMode>(mode, mapping);
	}
	const GANMode stringToGANMode(std::string modeName)
	{
		const std::map<std::string, GANMode> mapping = {
			{"Vanilla", GANMode::Vanilla},
			{"LSGAN", GANMode::LSGAN},
		};
		return getAlgorithmFromMapping<GANMode>(modeName, mapping);
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
		return getAlgorithmNameFromMapping<NormalizationLayerType>(type, mapping);
	}
	const NormalizationLayerType stringToNormalizationLayerType(std::string typeName)
	{
		const std::map<std::string, NormalizationLayerType> mapping = {
			{"None", NormalizationLayerType::None},
			{"Batch", NormalizationLayerType::Batch},
			{"Instance", NormalizationLayerType::Instance},
		};
		return getAlgorithmFromMapping<NormalizationLayerType>(typeName, mapping);
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
		return getAlgorithmNameFromMapping<LearningRatePolicy>(policy, mapping);
	}
	const LearningRatePolicy stringToLearningRatePolicy(std::string policyName)
	{
		const std::map<std::string, LearningRatePolicy> mapping = {
			{"Linear", LearningRatePolicy::Linear},
			{"Step", LearningRatePolicy::Step},
			{"Plateau", LearningRatePolicy::Plateau},
			{"Cosine", LearningRatePolicy::Cosine},
		};
		return getAlgorithmFromMapping<LearningRatePolicy>(policyName, mapping);
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
		return getAlgorithmNameFromMapping<WeightInitType>(type, mapping);
	}
	const WeightInitType stringToWeightInitType(std::string typeName)
	{
		const std::map<std::string, WeightInitType> mapping = {
			{"Normal", WeightInitType::Normal},
			{"Xavier", WeightInitType::Xavier},
			{"Kaimling", WeightInitType::Kaimling},
			{"Orthogonal", WeightInitType::Orthogonal},
		};
		return getAlgorithmFromMapping<WeightInitType>(typeName, mapping);
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
		return getAlgorithmNameFromMapping<MappingNetwork>(network, mapping);
	}
	const MappingNetwork stringToMappingNetwork(std::string networkName)
	{
		const std::map<std::string, MappingNetwork> mapping = {
			{"MLP_with_Conv_512", MappingNetwork::MLP_with_Conv_512},
			{"Residual", MappingNetwork::Residual},
			{"MLP_and_Residual_512", MappingNetwork::MLP_and_Residual_512},
		};
		return getAlgorithmFromMapping<MappingNetwork>(networkName, mapping);
	}
}