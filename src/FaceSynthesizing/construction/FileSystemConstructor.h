#pragma once

#include "Constructor.h"
#include "../filesystem/FileSystem.h"

#include <memory>

namespace filesystem = facesynthesizing::infrastructure::filesystem;

namespace facesynthesizing::construction {
	class FileSystemConstructor : public Constructor {
	public:
		FileSystemConstructor();
		void validateDependencies();
		void resolveDependencies();

		void setCaptureRoot(std::filesystem::path root);
		void setDatasetRoot(std::filesystem::path root);
		void setCheckpointsRoot(std::filesystem::path root);

		std::shared_ptr<filesystem::FileSystem> getFileSystem();
	private:
		std::shared_ptr<std::filesystem::path> captureRoot;
		std::shared_ptr<std::filesystem::path> datasetRoot;
		std::shared_ptr<std::filesystem::path> checkpointsRoot;

		std::shared_ptr<filesystem::FileSystem> fileSystem;
	};
}
