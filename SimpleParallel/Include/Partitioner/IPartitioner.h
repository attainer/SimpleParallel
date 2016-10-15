#pragma once

namespace SimpleParallel
{
	class IPartitioner
	{
	public:
		//This is called when parallel_for is be called on main thread.
		virtual void ready(int start, int end, size_t numThreads) = 0;
		
		//This is called to get next task index. You must design it thread safely.
		virtual inline bool getNext(int& out) = 0;
	};
}
