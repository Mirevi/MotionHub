#include "PythonImporter.h"

namespace facesynthesizing::infrastructure::python {
    py::object python_import_module(const std::string& moduleName, py::object& globals)
    {
        py::dict locals;
        locals["module_name"] = moduleName;

        py::exec(
            "import importlib\n"
            "new_module = importlib.import_module(module_name)\n"
            "print('imported python module: {}'.format(module_name))",
            globals,
            locals);
        return locals["new_module"];
    }
}