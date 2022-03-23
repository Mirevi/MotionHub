#pragma once

#include "PythonIncludeBoost.h"
#include "PythonImageWrapper.h"
#include "PythonFaceSynthesizer.h"

#include <memory>
#include <vector>

#pragma push_macro("slots") // otherwise python slots crashes with qt slots
#undef slots
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#pragma pop_macro("slots")

namespace py = boost::python;

namespace facesynthesizing::infrastructure::python {
    void addPythonModules();
}