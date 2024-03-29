#include "thread/ThreadManager.hpp"

namespace arterra {

	ThreadManager::ThreadManager()
		: _systemThreadCount{ std::thread::hardware_concurrency() }
		  , _awaitingShutdown{ false } { }

	ThreadManager::~ThreadManager() { TerminateThreads(); }

	void ThreadManager::PushJob(ThreadJob& threadJob)
	{
		{
			std::unique_lock<std::mutex> lock(_threadJobLock);
			_jobQueue.emplace(std::move(threadJob));
		}
		_threadCondition.notify_one();
	}

	void ThreadManager::CreateThreads(const size_t threadCount)
	{
		for (std::size_t i = 0; i < std::min(threadCount, _systemThreadCount - 1); ++i) {
			_threadPool.emplace_back(std::thread([this] { ThreadFunction(); }));
		}
	}

	void ThreadManager::TerminateThreads()
	{
		std::unique_lock<std::mutex> lock(_threadJobLock);
		_awaitingShutdown = true;
		_threadCondition.notify_all();

		for (auto& t : _threadPool) {
			t.join();
		}
	}

	void ThreadManager::ThreadFunction()
	{
		while (true) {
			{
				ThreadJob j;
				{
					std::unique_lock<std::mutex> lock(_threadJobLock);
					_threadCondition.wait(lock, [this] { return _awaitingShutdown | !_jobQueue.empty(); });
					j = _jobQueue.top();
					_jobQueue.pop();
				}
				j();
			}
		}
	}


}
