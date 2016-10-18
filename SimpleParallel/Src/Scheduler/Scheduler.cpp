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
		m_task = task;

		m_partitioner = partitioner;
		m_partitioner->ready(start, end, getNumThreads());

		m_nRunningThreads = static_cast<int>(getNumThreads());
	}

	void Scheduler::execute()
	{
		std::unique_lock<std::mutex> lock(m_waitNewMutex);
		m_readyCV.wait(lock, [this]() {return m_nReadyThreads == getNumThreads(); });
		m_waitNewCV.notify_all();
	}

	void Scheduler::wait()
	{
		std::unique_lock<std::mutex> lock(m_endMutex);
		m_endCV.wait(lock, [this]() {return m_nRunningThreads == 0; });
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
			//Notify Ready and Wait Run
			{
				std::unique_lock<std::mutex> lock(m_waitNewMutex);

				m_nReadyThreads++;
				m_readyCV.notify_all();
				m_waitNewCV.wait(lock);
				
				m_nReadyThreads--;
			}


			while (!m_stop && m_partitioner->getNext(current))
			{
				m_task(current);
			}

			if (m_stop)
				break;

			//Notify End
			{
				std::unique_lock<std::mutex> lock(m_endMutex);
				m_nRunningThreads--;
				m_endCV.notify_all();
			}
		}
	}
}