#include "Scheduler/Scheduler.h"
#include <assert.h>

namespace SimpleParallel
{
	thread_local size_t tls_threadIndex;

	Scheduler::Scheduler(size_t nThread)
	{
		assert(nThread > 0);
		for (size_t i = 0; i < nThread; i++)
		{
			m_threads.emplace_back(&Scheduler::threadProc, this, i);
		}
	}

	Scheduler::~Scheduler()
	{
		m_stop = true;

		execute();

		for (auto& thread : m_threads)
		{
			thread.join();
		}
	}

	void Scheduler::parallel_for(int start, int end, Task task, IPartitioner* partitioner)
	{
		ready(start, end, task, partitioner);
		execute();
		wait();
	}

	void Scheduler::ready(int& start, int& end, Task& task, IPartitioner*& partitioner)
	{
		m_nRunningThread = getNumThreads();

		m_task = task;

		m_partitioner = partitioner;
		m_partitioner->ready(start, end, getNumThreads());
	}

	void Scheduler::execute()
	{
		std::unique_lock<std::mutex> lock(m_waitNewMutex);
		m_waitNewCV.notify_all();
	}

	void Scheduler::wait()
	{
		std::unique_lock<std::mutex> lock(m_endAllMutex);
		m_endAllCV.wait(lock, [this]() {return m_nRunningThread == 0; });
	}

	size_t Scheduler::getNumThreads()
	{
		return m_threads.size();
	}

	size_t Scheduler::getCurrentThreadIndex()
	{
		return tls_threadIndex;
	}


	void Scheduler::threadProc(size_t index)
	{
		int current = 0;
		tls_threadIndex = index;

		while (true)
		{
			{
				std::unique_lock<std::mutex> lock(m_waitNewMutex);
				m_waitNewCV.wait(lock);
			}


			while (m_partitioner->getNext(current) && !m_stop)
			{
				m_task(current);
			}

			if (m_stop)
				break;

			{
				std::unique_lock<std::mutex> lock(m_endAllMutex);
				m_nRunningThread--;
				m_endAllCV.notify_all();
			}
		}
	}
}