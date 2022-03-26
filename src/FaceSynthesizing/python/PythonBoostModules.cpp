#include "PythonBoostModules.h"

namespace facesynthesizing::infrastructure::python {
    BOOST_PYTHON_MODULE(mmh_wrapper)
    {
		py::class_<FaceSynthesizer>("MMHFaceSynthesizer")
			.def("new_python_image", &FaceSynthesizer::newPythonImage)
			.def("new_python_status", &FaceSynthesizer::newPythonStatus)
			.def("update_python_status", &FaceSynthesizer::updatePythonStatus)
			.def("is_execution_stop_requested", &FaceSynthesizer::isExecutionStopRequested);
    }
	void addPythonModules()
	{
		if (PyImport_AppendInittab("mmh_wrapper", PyInit_mmh_wrapper) == -1)
			throw std::runtime_error("Failed to add mmh_wrapper module to the interpreter's "
				"builtin modules");
	}
}