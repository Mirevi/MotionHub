#pragma once

#include "PythonIncludeBoost.h"

#include <string>

namespace py = boost::python;

namespace facesynthesizing::infrastructure::python {
    py::object python_import_module(const std::string& moduleName, py::object& globals);
}