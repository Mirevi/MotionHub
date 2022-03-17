#include "FaceSynthesizingConstructor.h"

namespace facesynthesizing::construction {
	FaceSynthesizingConstructor::FaceSynthesizingConstructor()
	{
		faceSynthesizing = std::make_shared<usecases::FaceSynthesizing>();
	}

	void FaceSynthesizingConstructor::validateDependencies()
	{
		if (this->guiConstructor == nullptr)
			throw ConstructionException("Dependency of GUIConstructor not set!");
		if (this->cameraConstructor == nullptr)
			throw ConstructionException("Dependency of CameraConstructor not set!");
		if (this->pythonConstructor == nullptr)
			throw ConstructionException("Dependency of PythonConstructor not set!");
	}

	void FaceSynthesizingConstructor::resolveDependencies()
	{
		// FaceSynthesizing
		faceSynthesizing->setGuiPresenter(guiConstructor->getPresenter());
		faceSynthesizing->setCamera(cameraConstructor->getCamera());
		faceSynthesizing->setFaceSynthesizer(pythonConstructor->getFaceSynthesizer());
		faceSynthesizing->setFileSystem(fileSystemConstructor->getFileSystem());

		// GUI Interactor
		guiConstructor->addHigherLevelDependency(faceSynthesizing);

		// Python
		pythonConstructor->addHigherLevelDependency(faceSynthesizing);

	}

	void FaceSynthesizingConstructor::setGUIConstructor(std::shared_ptr<GUIConstructor> guiConstructor)
	{
		this->guiConstructor = guiConstructor;
	}

	void FaceSynthesizingConstructor::setCameraConstructor(std::shared_ptr<CameraConstructor> cameraConstructor)
	{
		this->cameraConstructor = cameraConstructor;
	}
	void FaceSynthesizingConstructor::setPythonConstructor(std::shared_ptr<PythonConstructor> pythonConstructor)
	{
		this->pythonConstructor = pythonConstructor;
	}
	void FaceSynthesizingConstructor::setFileSystemConstructor(std::shared_ptr<FileSystemConstructor> fileSystemConstructor)
	{
		this->fileSystemConstructor = fileSystemConstructor;
	}
	std::shared_ptr<usecases::FaceSynthesizing> FaceSynthesizingConstructor::getFaceSynthesizing()
	{
		return faceSynthesizing;
	}
}