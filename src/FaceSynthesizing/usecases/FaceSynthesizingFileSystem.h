#pragma once

#include "FaceSynthesizingDataStructures.h"

#include <string>
#include <filesystem>

namespace facesynthesizing::domain::usecases {
	enum class CaptureDataPairType {
		Training, Evaluation
	};
	class FaceSynthesizingFileSystem {
	public:
		virtual void prepareSavingCaptureDataPairs(std::string captureName) = 0;
		virtual void saveCaptureDataPair(
			CaptureDataPair dataPair, 
			std::string captureName, 
			std::string filename,
			CaptureDataPairType type
		) = 0;
	};
}