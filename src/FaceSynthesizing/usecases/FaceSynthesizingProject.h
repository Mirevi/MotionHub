#pragma once

#include "FaceSynthesizingDataStructures.h"

namespace facesynthesizing::domain::usecases {
	enum class CaptureType {
		Train,
		Eval,
	};

	class FaceSynthesizingProject {
	public:
		virtual void setCaptureInfos(std::shared_ptr<CaptureDataInformation> infos) = 0;
		virtual bool validateCapture(CaptureDataPair capture) = 0;
	};
}