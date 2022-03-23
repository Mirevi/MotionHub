#pragma once

#include "PythonIncludeBoost.h"
#include "../usecases/FaceSynthesizingDataStructures.h"

#include <memory>

namespace usecases = facesynthesizing::domain::usecases;
namespace py = boost::python;
namespace np = boost::python::numpy;

namespace facesynthesizing::infrastructure::python {
	np::ndarray imageToNpImage(std::shared_ptr<usecases::Image> image);
	std::shared_ptr<usecases::Image> npImageToImage(np::ndarray npArray);
}