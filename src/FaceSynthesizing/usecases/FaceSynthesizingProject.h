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
		virtual void setConvertInfos(std::shared_ptr<ConvertDataInformation> infos) = 0;
		virtual void convertData() = 0;
		virtual void setTrainInfos(std::shared_ptr<TrainingInformation> infos) = 0;
		virtual void train() = 0;

		virtual void requestExecutionStopAsync() = 0;
		virtual void resetExecutionStop() = 0;
	};
}