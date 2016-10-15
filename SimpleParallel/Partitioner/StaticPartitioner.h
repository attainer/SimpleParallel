#pragma once

#include "IPartitioner.h"
#include <vector>

namespace SimpleParallel
{
	class StaticPartitioner : public IPartitioner
	{
	public:
		static StaticPartitioner* get();
	public:
		void ready(int start, int end, size_t numThreads) override;
		inline bool getNext(int& out) override;
	protected:
		StaticPartitioner();
		~StaticPartitioner();
	private:
		std::vector<int> m_currentIndexes;
		std::vector<int> m_endIndexes;
	};
}
