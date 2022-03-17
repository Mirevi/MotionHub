#pragma once

#include "MotionHubUtil/ConfigManager.h"
#include "FaceSynthesizingConstructor.h"
#include "GUIConstructor.h"
#include "QtGUIConstructor.h"
#include "CameraConstructor.h"
#include "AzureKinectConstructor.h"
#include "PythonConstructor.h"
#include "FileSystemConstructor.h"

#include <exception>
#include <format>
#include <memory>
#include <QtWidgets/QDialog>

namespace facesynthesizing::construction {
	class ModuleConstructionException : std::exception {
	public:
		explicit ModuleConstructionException(std::string message) {
			std::exception();
			this->message = message;
		};
		const char* what() const throw () {
			return message.c_str();
		}
	private:
		std::string message;
	};

	enum class FaceSynthesizingGUIType {
		Qt
	};
	std::string faceSynthesizingGUITypeToString(FaceSynthesizingGUIType type);
	FaceSynthesizingGUIType stringToFaceSynthesizingGUIType(std::string name);
	enum class FaceSynthesizingCameraType {
		AzureKinect
	};
	FaceSynthesizingCameraType stringToFaceSynthesizingCameraType(std::string name);
	std::string faceSynthesizingCameraTypeToString(FaceSynthesizingCameraType type);

	class __declspec(dllexport) FaceSynthesizingBuilder {
	public:
		void build(ConfigManager* configManager);
		std::shared_ptr<QDialog> getQtGUI();
	private:
		void createConstructors();
		void resolveDependencies();
		void construct();

		// Constructors
		std::shared_ptr<FaceSynthesizingConstructor> faceSynthesizingConstructor;
		std::shared_ptr<GUIConstructor> guiConstructor;
		std::shared_ptr<CameraConstructor> cameraConstructor;
		std::shared_ptr<PythonConstructor> pythonConstructor;
		std::shared_ptr<FileSystemConstructor> fileSystemConstructor;

		// Config values
		FaceSynthesizingGUIType guiType;
		FaceSynthesizingCameraType cameraType;
		std::string datasetRoot;
		std::string checkpointsRoot;
	};
}