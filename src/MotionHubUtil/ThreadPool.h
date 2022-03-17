#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>

#include "ConfigDllExportMotionHubUtil.h"

class MotionHubUtil_DLL_import_export ThreadPool {
public:
	ThreadPool(int numberOfWorkers);
	~ThreadPool();

	void addJob(std::function<void()> job);
	void shutdown();
private:
	void workerLoopFunction();

	std::vector<std::thread> pool;
	std::queue<std::function<void()>> jobs;
	std::mutex poolLock;
	std::condition_variable dataCondition;
	bool terminated = false;
};