#include "DynamicPartitioner.h"


namespace SimpleParallel
{
	DynamicPartitioner::DynamicPartitioner()
	{
	}


	DynamicPartitioner::~DynamicPartitioner()
	{
	}

	DynamicPartitioner* DynamicPartitioner::get()
	{
		static DynamicPartitioner partitioner;
		return &partitioner;
	}

	void DynamicPartitioner::ready(int start, int end, size_t numThreads)
	{
		m_currentIndex = start;
		m_end = end;
	}

	inline bool DynamicPartitioner::getNext(int& out)
	{
		int tmp;
		do
		{
			tmp = m_currentIndex;
		} while (!m_currentIndex.compare_exchange_weak(tmp, tmp + 1) && tmp < m_end); //CAS. Lockfree.

		if (tmp < m_end)
		{
			out = tmp;
			return true;
		}
		else
		{
			return false;
		}
	}
}

