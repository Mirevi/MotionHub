#include "FileSystemConstructor.h"

#include <iostream>

namespace facesynthesizing::construction {
	FileSystemConstructor::FileSystemConstructor()
	{
		fileSystem = std::make_shared<filesystem::FileSystem>();
	}
	void FileSystemConstructor::setCaptureRoot(std::filesystem::path root)
	{
		this->captureRoot = std::shared_ptr<std::filesystem::path>(new fs::path(root));
	}
	void FileSystemConstructor::setDatasetRoot(std::filesystem::path root)
	{
		this->datasetRoot = std::shared_ptr<std::filesystem::path>(new fs::path(root));
	}
	void FileSystemConstructor::setCheckpointsRoot(std::filesystem::path root)
	{
		this->checkpointsRoot = std::shared_ptr<std::filesystem::path>(new fs::path(root));
	}
	void FileSystemConstructor::validateDependencies()
	{
		if (this->captureRoot == nullptr)
			throw ConstructionException("Dependency of FileSystemConstructor not set!");
		if (this->datasetRoot == nullptr)
			throw ConstructionException("Dependency of FileSystemConstructor not set!");
		if (this->checkpointsRoot == nullptr)
			throw ConstructionException("Dependency of FileSystemConstructor not set!");
	}
	void FileSystemConstructor::resolveDependencies()
	{
		fileSystem->setCaptureRoot(*captureRoot);
		fileSystem->setDatasetRoot(*datasetRoot);
		fileSystem->setCheckpointsRoot(*checkpointsRoot);
	}
	std::shared_ptr<filesystem::FileSystem> FileSystemConstructor::getFileSystem()
	{
		return fileSystem;
	}
}
