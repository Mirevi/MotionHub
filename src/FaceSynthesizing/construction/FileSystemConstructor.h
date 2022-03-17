#pragma once

#include "Constructor.h"
#include "../filesystem/FileSystem.h"

#include <memory>

namespace filesystem = facesynthesizing::infrastructure::filesystem;

namespace facesynthesizing::construction {
	class FileSystemConstructor : public Constructor {
	public:
		FileSystemConstructor(std::filesystem::path datasetRoot, std::filesystem::path checkpointsRoot);
		void resolveDependencies();

		std::shared_ptr<filesystem::FileSystem> getFileSystem();
	private:
		std::filesystem::path datasetRoot;
		std::filesystem::path checkpointsRoot;

		std::shared_ptr<filesystem::FileSystem> fileSystem;
	};
}
