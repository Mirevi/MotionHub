#include "./FaceSynthesizingBuilder.h"

namespace facesynthesizing::construction {
	std::string faceSynthesizingGUITypeToString(FaceSynthesizingGUIType type) {
		switch (type)
		{
		case FaceSynthesizingGUIType::Qt:
			return "Qt";
		default:
			throw "Cant convert enum value from FaceSynthesizingGUIType to string!";
		}
	}
	FaceSynthesizingGUIType stringToFaceSynthesizingGUIType(std::string name)
	{
		if (name == "Qt") 
			return FaceSynthesizingGUIType::Qt;
		else
			throw "Cant convert string value to FaceSynthesizingGUIType: " + name;
	}
	std::string faceSynthesizingCameraTypeToString(FaceSynthesizingCameraType type) {
		switch (type)
		{
		case FaceSynthesizingCameraType::AzureKinect:
			return "AzureKinect";
		default:
			throw "Cant convert enum value from FaceSynthesizingCameraType to string!";
		}
	}
	FaceSynthesizingCameraType stringToFaceSynthesizingCameraType(std::string name)
	{
		if (name == "AzureKinect")
			return FaceSynthesizingCameraType::AzureKinect;
		else
			throw "Cant convert string value to FaceSynthesizingCameraType: " + name;
	}

	void FaceSynthesizingBuilder::build(ConfigManager* configManager) {
		datasetRoot = configManager->getStringFromStartupConfig("faceSynthesizingDatasetRoot");
		checkpointsRoot = configManager->getStringFromStartupConfig("faceSynthesizingCheckpointsRoot");
		cameraType = stringToFaceSynthesizingCameraType(configManager->getStringFromStartupConfig("faceSynthesizingCameraType"));
		guiType = FaceSynthesizingGUIType::Qt;

		createConstructors();
		resolveDependencies();
		construct();
	}

	void FaceSynthesizingBuilder::createConstructors()
	{
		//Use Cases
		faceSynthesizingConstructor = std::make_shared<FaceSynthesizingConstructor>();

		//gui
		switch (guiType) {
		case FaceSynthesizingGUIType::Qt:
			guiConstructor = std::make_shared<QtGUIConstructor>();
			break;
		default:
			throw ModuleConstructionException(
				"GUI Type" + faceSynthesizingGUITypeToString(guiType) + "is not implemented!");
		}

		//camera
		switch (cameraType) {
		case FaceSynthesizingCameraType::AzureKinect:
			cameraConstructor = std::make_shared<AzureKinectConstructor>();
			break;
		default:
			throw ModuleConstructionException(
				"Camera Type" + faceSynthesizingCameraTypeToString(cameraType) + "is not implemented!");
		}

		//python
		pythonConstructor = std::make_shared<PythonConstructor>();

		//filesystem
		fileSystemConstructor = std::make_shared<FileSystemConstructor>(datasetRoot, checkpointsRoot);
	}

	void FaceSynthesizingBuilder::resolveDependencies() {
		// use cases
		this->faceSynthesizingConstructor->setGUIConstructor(this->guiConstructor);
		this->faceSynthesizingConstructor->setCameraConstructor(this->cameraConstructor);
		this->faceSynthesizingConstructor->setPythonConstructor(this->pythonConstructor);
		this->faceSynthesizingConstructor->setFileSystemConstructor(this->fileSystemConstructor);
	}

	void FaceSynthesizingBuilder::construct() {
		this->faceSynthesizingConstructor->construct();
		this->guiConstructor->construct();
		this->cameraConstructor->construct();
		this->pythonConstructor->construct();
		this->fileSystemConstructor->construct();
	}

	std::shared_ptr<QDialog> FaceSynthesizingBuilder::getQtGUI() {
		if (guiType != FaceSynthesizingGUIType::Qt)
			throw ModuleConstructionException("Cant get Qt GUI if Qt isnt set as guiType in config!");

		std::shared_ptr<GuiMainWindow> generalizedMainWindow = this->guiConstructor->getMainWindow();
		QtGuiMainWindow* mainWindowHolder = static_cast<QtGuiMainWindow*>(generalizedMainWindow.get());
		std::shared_ptr<QDialog> dialog = mainWindowHolder->mainWindow;

		return dialog;
	}
}