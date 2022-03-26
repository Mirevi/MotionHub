#pragma once

#include "PythonIncludeBoost.h"
#include "PythonImageWrapper.h"
#include "PythonFaceSynthesizer.h"

#include <memory>

namespace py = boost::python;

namespace facesynthesizing::infrastructure::python {
    void addPythonModules();
}