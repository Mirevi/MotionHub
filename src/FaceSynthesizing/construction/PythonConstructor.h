#pragma once

#include "Constructor.h"
#include "../python/IncludeBoostPython.h"
#include "../python/FaceSynthesizer.h"
#include "../python/PythonBoostModules.h"

namespace python = facesynthesizing::infrastructure::python;
namespace py = boost::python;

namespace facesynthesizing::construction {
	class PythonConstructor : public Constructor {
	public:
		PythonConstructor();
		virtual void resolveDependencies();
		std::shared_ptr<python::FaceSynthesizer> getFaceSynthesizer();

		void addHigherLevelDependency(std::shared_ptr<usecases::FaceSynthesizingProjectInteractor> faceSynthesizingProjectInteractor);
	private:
		void initializePython();
		void addVenvSitePackages();
		void setCurrentWorkingDirectory();
	protected:
		std::shared_ptr<python::FaceSynthesizer> faceSynthesizer;
		py::object globals;
	};
}
