#include "StaticPartitioner.h"
#include "../Scheduler/Scheduler.h"
#include <assert.h>

namespace SimpleParallel
{
	StaticPartitioner::StaticPartitioner()
	{
	}


	StaticPartitioner::~StaticPartitioner()
	{
	}

	StaticPartitioner* StaticPartitioner::get()
	{
		static StaticPartitioner partitioner;
		return &partitioner;
	}

	void StaticPartitioner::ready(int start, int end, size_t numThreads)
	{
		m_numThreads = numThreads;

		if (m_currentIndexes.size() < numThreads)
		{
			m_currentIndexes.resize(numThreads);
		}

		if (m_endIndexes.size() < numThreads)
		{
			m_endIndexes.resize(numThreads);
		}

		int length = end - start;
		assert(length > 0);
		int division = length / numThreads;
		int remainder = length % numThreads;


		int count = start;

		for (size_t i = 0; i < numThreads; i++)
		{
			m_currentIndexes[i] = count;

			count += (division + ((remainder > static_cast<int>(i)) ? 1 : 0));

			m_endIndexes[i] = count;
		}
	}

	inline bool StaticPartitioner::getNext(int& out)
	{
		int threadIndex = Scheduler::getCurrentThreadIndex();
		if (m_currentIndexes[threadIndex] < m_endIndexes[threadIndex])
		{
			out = m_currentIndexes[threadIndex]++;
			return true;
		}
		else
		{
			return false;
		}
	}
}

