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

		//Main thread wait other threads being ready.
		std::condition_variable m_readyCV;

		//Other threads wait main thread call run.
		std::mutex m_waitNewMutex;
		std::condition_variable m_waitNewCV;

		//Main thread wait other threads being end.
		std::mutex m_endMutex;
		std::condition_variable m_endCV;

		Task m_task;

		IPartitioner* m_partitioner = DynamicPartitioner::get();

		std::atomic<int> m_nRunningThreads = 0;
		std::atomic<int> m_nReadyThreads = 0;


		std::atomic<bool> m_stop = false;
	};
}