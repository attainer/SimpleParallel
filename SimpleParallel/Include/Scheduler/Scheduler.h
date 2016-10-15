#pragma once

#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "Partitioner/DynamicPartitioner.h"

namespace SimpleParallel
{
	typedef std::function<void(int)> Task;

	class Scheduler
	{
	public:
		Scheduler(size_t nThread = std::thread::hardware_concurrency());
		~Scheduler();

		void parallel_for(int start, int end, Task task, IPartitioner* partitioner = DynamicPartitioner::get());

		size_t getNumThreads();
		static size_t getCurrentThreadIndex();

	private:
		void ready(int& start, int& end, Task& task, IPartitioner*& partitioner);
		void execute();
		void wait();

		void threadProc(size_t index);
	private:
		std::vector<std::thread> m_threads;

		std::mutex m_waitNewMutex;
		std::condition_variable m_waitNewCV;

		std::mutex m_endAllMutex;
		std::condition_variable m_endAllCV;

		Task m_task;

		IPartitioner* m_partitioner = DynamicPartitioner::get();

		std::atomic<int> m_nRunningThread = 0;

		std::atomic<bool> m_stop = false;
	};
}