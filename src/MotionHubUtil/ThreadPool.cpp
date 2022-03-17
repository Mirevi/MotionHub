#include "ThreadPool.h"

#include <iostream>

ThreadPool::ThreadPool(int numberOfWorkers)
{
	for(int i = 0 ; i < numberOfWorkers ; i++)
		pool.push_back(std::thread(&ThreadPool::workerLoopFunction, this));
}

ThreadPool::~ThreadPool()
{
	shutdown();
}

void ThreadPool::workerLoopFunction()
{
	std::function<void()> currentJob;
	while (!terminated) {
		std::unique_lock<std::mutex> lock(poolLock);
		dataCondition.wait(lock, [this]() {return !jobs.empty() || terminated; });

		if (terminated)
			continue;


		currentJob = jobs.front();
		jobs.pop();
		lock.unlock();

		currentJob();
	}
}

void ThreadPool::addJob(std::function<void()> job)
{
	std::unique_lock<std::mutex> lock(poolLock);
	jobs.push(job);
	lock.unlock();
	dataCondition.notify_one();
}

void ThreadPool::shutdown()
{
	std::unique_lock<std::mutex> lock(poolLock);
	terminated = true;
	lock.unlock();

	dataCondition.notify_all();

	for (auto& thread : pool)
		if (thread.joinable())
			thread.join();

}
