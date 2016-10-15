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
  parallel.parallel_for(0, size, [&a, &counts](int i) {
			if (is_prime(a[i]))
			{
				counts[SimpleParallel::Scheduler::getCurrentThreadIndex()]++;
			}
		});
}
```
