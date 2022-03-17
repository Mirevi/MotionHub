#pragma once

#include "../usecases/FaceSynthesizingProject.h"
#include "../usecases/FaceSynthesizingProjectInteractor.h"
#include "IncludeBoostPython.h"
#include "PythonImporter.h"
#include "PythonImageWrapper.h"

#include <mutex>

namespace usecases = facesynthesizing::domain::usecases;
namespace py = boost::python;

namespace facesynthesizing::infrastructure::python {
	class FaceSynthesizer : public usecases::FaceSynthesizingProject {
	public:
		void setCaptureInfos(std::shared_ptr<usecases::CaptureDataInformation> infos);
		bool validateCapture(usecases::CaptureDataPair capture);

		// Construction purpose
		void setGlobals(py::object globals);
		void setFaceSynthesizingProjectInteractor(std::shared_ptr<usecases::FaceSynthesizingProjectInteractor> faceSynthesizing);
	private:
		void importPythonModules();
		void addStandardImportsToGlobals();
		void definePythonFunctions();
		void addRunnerToGlobals();

		py::object globals;
		py::object captureValidator;

		std::shared_ptr<usecases::FaceSynthesizingProjectInteractor> faceSynthesizing;
		std::shared_ptr<usecases::CaptureDataInformation> captureInfos;
	};
}