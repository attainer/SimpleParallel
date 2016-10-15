# SimpleParallel
This is a simple parallel library. You can easilly modify to fit your projects.

# Usage

```cmake
#include "SimpleParallel.h"

void Scheduler::parallel_for(int start, int end, Task task, IPartitioner* partitioner = DynamicPartitioner::get());
size_t Scheduler::getNumThreads();
static size_t Scheduler::getCurrentThreadIndex();
```

#example
```cmake

#include "SimpleParallel.h"

main()
{
  SimpleParallel::Scheduler parallel;
  
  
  int* a = new int[size];
  ...
  Initialize a
  ...
  
  std::vector<int> counts(parallel.getNumThreads());
 
  //parallel processing using dynamic parititioner 
  parallel.parallel_for(0, size, [&a, &counts](int i) {
    if (is_prime(a[i]))
    {
        counts[SimpleParallel::Scheduler::getCurrentThreadIndex()]++;
    }
  });
  
  //or use can use static partitioner
  parallel.parallel_for(0, size, [&a, &counts](int i) {
    if (is_prime(a[i]))
    {
        counts[SimpleParallel::Scheduler::getCurrentThreadIndex()]++;
    }
  }, SimpleParallel::StaticPartitioner::get());
  
}
```

#partitioner

Partitional make a decision how to distribute tasks.

```cmake
class IPartitioner
	{
	public:
		virtual void ready(int start, int end, size_t numThreads) = 0;
		virtual inline bool getNext(int& out) = 0;
	};
```

You can easily make your own partitioner. Just inherit IPartitioner and implementation 2 functions.

ready() is called when parallel_for is be called on main thread.
getNext() is called to get next task index. You must design it thread safely.
