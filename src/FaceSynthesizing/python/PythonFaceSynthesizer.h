#pragma once

#include "../usecases/FaceSynthesizingProject.h"
#include "../usecases/FaceSynthesizingProjectInteractor.h"
#include "PythonIncludeBoost.h"
#include "PythonImporter.h"
#include "PythonImageWrapper.h"
#include "PythonEnumUtils.h"

#include <string>
#include <mutex>
#include <filesystem>
#include <sstream>
#include <atomic>

namespace usecases = facesynthesizing::domain::usecases;
namespace py = boost::python;
namespace fs = std::filesystem;

namespace facesynthesizing::infrastructure::python {
	struct PythonErrorMetadata {
		std::string message;
		std::string filename;
		std::string functionName;
		long linenumber;
	};

	class PythonException : public std::exception {
	public:
		explicit PythonException(PythonErrorMetadata metadata)
		{
			this->metadata = metadata;
		};
		const char* what() const throw () {
			std::stringstream ss;

			ss << "Python Exception in file " << metadata.filename << " ";
			ss << "in function " << metadata.functionName << " ";
			ss << "in line " << std::to_string(metadata.linenumber) << " ";
			ss << "with Message : " << metadata.message;

			return ss.str().c_str();
		}
	private:
		PythonErrorMetadata metadata;
	};

	class FaceSynthesizer : public usecases::FaceSynthesizingProject {
	public:
		FaceSynthesizer();

		void setCaptureInfos(std::shared_ptr<usecases::CaptureDataInformation> infos);
		bool validateCapture(usecases::CaptureDataPair capture);
		void setConvertInfos(std::shared_ptr<usecases::ConvertDataInformation> infos);
		void convertData();
		void setTrainInfos(std::shared_ptr<usecases::TrainingInformation> infos);
		void train();
		void requestExecutionStopAsync();
		void resetExecutionStop();

		void newPythonImage(py::object image, std::string identifier);
		void newPythonStatus(std::string statusMessage);
		void updatePythonStatus(std::string statusMessage);
		bool isExecutionStopRequested();

		// Construction purpose
		void setGlobals(py::object globals);
		void setFaceSynthesizingProjectInteractor(std::shared_ptr<usecases::FaceSynthesizingProjectInteractor> faceSynthesizing);
		void setCaptureRoot(fs::path root);
		void setDatasetRoot(fs::path root);
		void setCheckpointsRoot(fs::path root);
	private:
		py::dict captureInfosToDict();
		void processFaceAlignmentImage();
		py::dict convertInfosToDict();
		py::dict trainingInfosToDict();
		void handlePythonError();
		PythonErrorMetadata getPythonErrorMetadata();
		void importPythonModules();
		void addStandardImportsToGlobals();
		void definePythonFunctions();
		void definePythonClasses();
		void addObjectsToGlobals();

		py::object globals;
		py::dict captureLocals;
		py::dict convertLocals;
		py::dict trainingLocals;

		std::shared_ptr<usecases::FaceSynthesizingProjectInteractor> faceSynthesizing;
		std::shared_ptr<usecases::CaptureDataInformation> captureInfos;
		std::shared_ptr<usecases::ConvertDataInformation> convertInfos;
		std::shared_ptr<usecases::TrainingInformation> trainingInfos;

		std::shared_ptr<std::atomic<bool>> executionStop;

		fs::path captureRoot;
		fs::path datasetRoot;
		fs::path checkpointsRoot;
	};
}