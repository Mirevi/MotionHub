#pragma once

#include "Constructor.h"
#include "../python/PythonIncludeBoost.h"
#include "../python/PythonFaceSynthesizer.h"
#include "../python/PythonBoostModules.h"

#include <filesystem>

namespace python = facesynthesizing::infrastructure::python;
namespace py = boost::python;
namespace fs = std::filesystem;

namespace facesynthesizing::construction {
	class PythonConstructor : public Constructor {
	public:
		PythonConstructor();
		void validateDependencies();
		void resolveDependencies();

		void setCaptureRoot(fs::path root);
		void setDatasetRoot(fs::path root);
		void setCheckpointsRoot(fs::path root);

		std::shared_ptr<python::FaceSynthesizer> getFaceSynthesizer();

		void addHigherLevelDependency(std::shared_ptr<usecases::FaceSynthesizingProjectInteractor> faceSynthesizingProjectInteractor);
	private:
		void initializePython();
		void addVenvSitePackages();
		void setCurrentWorkingDirectory();
	protected:
		std::shared_ptr<fs::path> captureRoot;
		std::shared_ptr<fs::path> datasetRoot;
		std::shared_ptr<fs::path> checkpointsRoot;

		std::shared_ptr<python::FaceSynthesizer> faceSynthesizer;
		py::object globals;
	};
}
