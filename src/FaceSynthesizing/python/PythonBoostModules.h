#pragma once

#include "IncludeBoostPython.h"
#include "PythonImageWrapper.h"
#include "FaceSynthesizer.h"

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