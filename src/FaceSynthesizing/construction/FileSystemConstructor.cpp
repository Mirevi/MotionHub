#include "FileSystemConstructor.h"

#include <iostream>

namespace facesynthesizing::construction {
	FileSystemConstructor::FileSystemConstructor(std::filesystem::path datasetRoot, std::filesystem::path checkpointsRoot)
	{
		this->datasetRoot = datasetRoot;
		this->checkpointsRoot = checkpointsRoot;
		fileSystem = std::make_shared<filesystem::FileSystem>();
	}
	void FileSystemConstructor::resolveDependencies()
	{
		fileSystem->setDatasetRoot(datasetRoot);
		fileSystem->setCheckpointsRoot(checkpointsRoot);
	}
	std::shared_ptr<filesystem::FileSystem> FileSystemConstructor::getFileSystem()
	{
		return fileSystem;
	}
}
