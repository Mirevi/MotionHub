#include "PythonBoostModules.h"

namespace facesynthesizing::infrastructure::python {
    BOOST_PYTHON_MODULE(mmh_wrapper)
    {

    }
	void addPythonModules()
	{
		if (PyImport_AppendInittab("mmh_wrapper", PyInit_mmh_wrapper) == -1)
			throw std::runtime_error("Failed to add mmh_wrapper module to the interpreter's "
				"builtin modules");
	}
}