#pragma once

#include "../usecases/FaceSynthesizingCamera.h"

namespace usecases = facesynthesizing::domain::usecases;

namespace facesynthesizing::domain::adapters::camera {
	class RGBDICamera : public usecases::FaceSynthesizingCamera {
	public:
		virtual void initiate();
		virtual void close();
	private:
	};
}