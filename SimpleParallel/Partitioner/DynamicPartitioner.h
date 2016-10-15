#pragma once
#include "IPartitioner.h"
#include <atomic>

namespace SimpleParallel
{
	class DynamicPartitioner : public IPartitioner
	{
	public:
		static DynamicPartitioner* get();
	public:
		void ready(int start, int end, size_t numThreads) override;
		inline bool getNext(int& out) override;
	protected:
		DynamicPartitioner();
		~DynamicPartitioner();
	private:
		std::atomic<int> m_currentIndex;
		int m_end;
	};
}


