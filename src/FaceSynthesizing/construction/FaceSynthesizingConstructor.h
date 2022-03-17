#pragma once

#include "./Constructor.h"
#include "../usecases/FaceSynthesizing.h"
#include "GUIConstructor.h"
#include "CameraConstructor.h"
#include "PythonConstructor.h"
#include "FileSystemConstructor.h"

#include <memory>

namespace usecases = facesynthesizing::domain::usecases;

namespace facesynthesizing::construction {
	class FaceSynthesizingConstructor : public Constructor {
	public:
		FaceSynthesizingConstructor();
		void validateDependencies();
		void resolveDependencies();
		void setGUIConstructor(std::shared_ptr<GUIConstructor> guiConstructor);
		void setCameraConstructor(std::shared_ptr<CameraConstructor> cameraConstructor);
		void setPythonConstructor(std::shared_ptr<PythonConstructor> pythonConstructor);
		void setFileSystemConstructor(std::shared_ptr<FileSystemConstructor> fileSystemConstructor);

		std::shared_ptr<usecases::FaceSynthesizing> getFaceSynthesizing();
	private:
		std::shared_ptr<usecases::FaceSynthesizing> faceSynthesizing;

		// dependencies
		std::shared_ptr<GUIConstructor> guiConstructor;
		std::shared_ptr<CameraConstructor> cameraConstructor;
		std::shared_ptr<PythonConstructor> pythonConstructor;
		std::shared_ptr<FileSystemConstructor> fileSystemConstructor;
	};
}