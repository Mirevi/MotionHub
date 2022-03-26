#include "PythonEnumUtils.h"

#include <iostream>

namespace facesynthesizing::infrastructure::python {
	const std::string boundingBoxAlgorithmToPythonName(usecases::BoundingBoxAlgorithm algorithm)
	{
		const std::map<usecases::BoundingBoxAlgorithm, std::string> mapping = {
			{usecases::BoundingBoxAlgorithm::SynergyNet, "SynergyNet"},
		};
		return usecases::getNameFromMapping<usecases::BoundingBoxAlgorithm>(algorithm, mapping);
	}
	const std::string faceAlignmentAlgorithmToPythonName(usecases::FaceAlignmentAlgorithm algorithm)
	{
		const std::map<usecases::FaceAlignmentAlgorithm, std::string> mapping = {
			{usecases::FaceAlignmentAlgorithm::SynergyNet, "SynergyNet"},
		};
		return usecases::getNameFromMapping<usecases::FaceAlignmentAlgorithm>(algorithm, mapping);
	}
	const std::string eyeTrackingAlgorithmToPythonName(usecases::EyeTrackingAlgorithm algorithm)
	{
		const std::map<usecases::EyeTrackingAlgorithm, std::string> mapping = {
			{usecases::EyeTrackingAlgorithm::Gradient_Based, "Gradient_Based"},
			{usecases::EyeTrackingAlgorithm::Infrared, "Infrared"},
		};
		return usecases::getNameFromMapping<usecases::EyeTrackingAlgorithm>(algorithm, mapping);
	}
	const std::string faceTrackingAlgorithmToPythonName(usecases::FaceTrackingAlgorithm algorithm)
	{
		const std::map<usecases::FaceTrackingAlgorithm, std::string> mapping = {
			{usecases::FaceTrackingAlgorithm::FAN, "FAN"},
			{usecases::FaceTrackingAlgorithm::dlib, "DLib"},
		};
		return usecases::getNameFromMapping<usecases::FaceTrackingAlgorithm>(algorithm, mapping);
	}
	const std::string depthFillingAlgorithmToPythonName(usecases::DepthFillingAlgorithm algorithm)
	{
		const std::map<usecases::DepthFillingAlgorithm, std::string> mapping = {
			{usecases::DepthFillingAlgorithm::FDCBOP, "FDCBOP"},
		};
		return usecases::getNameFromMapping<usecases::DepthFillingAlgorithm>(algorithm, mapping);
	}
	const std::string colorFormatToPythonName(usecases::ColorFormat format)
	{
		const std::map<usecases::ColorFormat, std::string> mapping = {
			{usecases::ColorFormat::RGB, "RGB"},
			{usecases::ColorFormat::BGR, "BGR"},
		};
		return usecases::getNameFromMapping<usecases::ColorFormat>(format, mapping);
	}
	const std::string modelTypeToPythonName(usecases::ModelType type)
	{
		const std::map<usecases::ModelType, std::string> mapping = {
			{usecases::ModelType::L1, "l1"},
			{usecases::ModelType::Pix2Pix, "pix2pix"},
			{usecases::ModelType::Pix2Pix_Extended, "pix2pix_extended"},
		};
		return usecases::getNameFromMapping<usecases::ModelType>(type, mapping);
	}
	const std::string generatorArchitectureToPythonName(usecases::GeneratorArchitecture architecture, int imageSize)
	{
		int highestPowerOfTwo = 2;
		while (highestPowerOfTwo < imageSize)
			highestPowerOfTwo *= 2;
		highestPowerOfTwo = static_cast<int>(highestPowerOfTwo / 2);

		const std::map<usecases::GeneratorArchitecture, std::string> mapping = {
			{usecases::GeneratorArchitecture::UNet, "unet_"},
			{usecases::GeneratorArchitecture::ResNet_6_Blocks, "resnet_6blocks"},
			{usecases::GeneratorArchitecture::ResNet_9_Blocks, "resnet_9blocks"},
		};

		std::string name = usecases::getNameFromMapping<usecases::GeneratorArchitecture>(architecture, mapping);

		if (architecture == usecases::GeneratorArchitecture::UNet)
			name += std::to_string(highestPowerOfTwo);

		return name;
	}
	const std::string discriminatorArchitectureToPythonName(usecases::DiscriminatorArchitecture architecture)
	{
		const std::map<usecases::DiscriminatorArchitecture, std::string> mapping = {
			{usecases::DiscriminatorArchitecture::Pixel, "pixel"},
			{usecases::DiscriminatorArchitecture::N_Layer, "n_layers"},
			{usecases::DiscriminatorArchitecture::Multiscale, "multiscale"},
		};
		return usecases::getNameFromMapping<usecases::DiscriminatorArchitecture>(architecture, mapping);
	}
	const std::string ganModeToPythonName(usecases::GANMode mode)
	{
		const std::map<usecases::GANMode, std::string> mapping = {
			{usecases::GANMode::Vanilla, "vanilla"},
			{usecases::GANMode::LSGAN, "lsgan"},
		};
		return usecases::getNameFromMapping<usecases::GANMode>(mode, mapping);
	}
	const std::string normalizationLayerTypeToPythonName(usecases::NormalizationLayerType type)
	{
		const std::map<usecases::NormalizationLayerType, std::string> mapping = {
			{usecases::NormalizationLayerType::None, "none"},
			{usecases::NormalizationLayerType::Batch, "batch"},
			{usecases::NormalizationLayerType::Instance, "instance"},
		};
		return usecases::getNameFromMapping<usecases::NormalizationLayerType>(type, mapping);
	}
	const std::string learningRatePolicyToPythonName(usecases::LearningRatePolicy policy)
	{
		const std::map<usecases::LearningRatePolicy, std::string> mapping = {
			{usecases::LearningRatePolicy::Linear, "linear"},
			{usecases::LearningRatePolicy::Step, "step"},
			{usecases::LearningRatePolicy::Plateau, "plateau"},
			{usecases::LearningRatePolicy::Cosine, "cosine"},
		};
		return usecases::getNameFromMapping<usecases::LearningRatePolicy>(policy, mapping);
	}
	const std::string weightInitTypeToPythonName(usecases::WeightInitType type)
	{
		const std::map<usecases::WeightInitType, std::string> mapping = {
			{usecases::WeightInitType::Normal, "normal"},
			{usecases::WeightInitType::Xavier, "xavier"},
			{usecases::WeightInitType::Kaimling, "kaiming"},
			{usecases::WeightInitType::Orthogonal, "orthogonal"},
		};
		return usecases::getNameFromMapping<usecases::WeightInitType>(type, mapping);
	}
	const std::string mappingNetworkToPythonName(usecases::MappingNetwork network)
	{
		const std::map<usecases::MappingNetwork, std::string> mapping = {
			{usecases::MappingNetwork::MLP_with_Conv_512, "mlp_with_conv_512"},
			{usecases::MappingNetwork::Residual, "residual"},
			{usecases::MappingNetwork::MLP_and_Residual_512, "mlp_and_residual_512"},
		};
		return usecases::getNameFromMapping<usecases::MappingNetwork>(network, mapping);
	}
	const usecases::ImageType pyImageIdentifierToImageType(std::string identifier)
	{
		const std::map<std::string, usecases::ImageType> mapping = {
			{"color", usecases::ImageType::Dataset_Color},
			{"depth", usecases::ImageType::Dataset_Depth},
			{"flm", usecases::ImageType::Dataset_FLM},
			{"face bounding box", usecases::ImageType::Convert_Face_Bounding_box},
			{"landmarks", usecases::ImageType::Convert_Landmarks},
			{"depth filling hole region", usecases::ImageType::Convert_Depth_Holes},
			{"real_feature", usecases::ImageType::Dataset_FLM},
			{"real_color", usecases::ImageType::Dataset_Color},
			{"fake_color", usecases::ImageType::Synthesized_Color},
			{"real_depth", usecases::ImageType::Dataset_Depth},
			{"fake_depth", usecases::ImageType::Synthesized_Depth},
		};

		try {
			return usecases::getEnumValueFromMapping<usecases::ImageType>(identifier, mapping);
		}
		catch (std::exception e){}

		return usecases::ImageType::Unknown;
	}
}