#include "FaceSynthesizingEnums.h"

namespace facesynthesizing::domain::usecases {
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

		auto   it = mapping.find(algorithm);

		if (it == mapping.end())
			throw "Unknow BoundingBoxAlgorithm!";

		return it->second;
	}
	const BoundingBoxAlgorithm stringToBoundingBoxAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, BoundingBoxAlgorithm> mapping = {
			{"SynergyNet", BoundingBoxAlgorithm::SynergyNet},
		};

		auto   it = mapping.find(algorithmName);

		if (it == mapping.end())
			throw "Unknow BoundingBoxAlgorithm!";

		return it->second;
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

		auto   it = mapping.find(algorithm);

		if (it == mapping.end())
			throw "Unknow FaceAlignmentAlgorithm!";

		return it->second;
	}
	const FaceAlignmentAlgorithm stringToFaceAlignmentAlgorithm(std::string algorithmName)
	{
		const std::map<std::string, FaceAlignmentAlgorithm> mapping = {
			{"SynergyNet", FaceAlignmentAlgorithm::SynergyNet},
		};

		auto   it = mapping.find(algorithmName);

		if (it == mapping.end())
			throw "Unknow FaceAlignmentAlgorithm!";

		return it->second;
	}
}