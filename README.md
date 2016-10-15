# SimpleParallel
This is a simple parallel programming library for c++. You can easily modify to fit your projects. 

# Usage

```c++
#include "SimpleParallel.h"

void Scheduler::parallel_for(int start, int end, std::function<void(int)> task, IPartitioner* partitioner = DynamicPartitioner::get());
size_t Scheduler::getNumThreads();
static size_t Scheduler::getCurrentThreadIndex();
```

#Example
```c++

#include "SimpleParallel.h"

main()
{
  SimpleParallel::Scheduler parallel;
  
  
  int* a = new int[size];
  ...
  //Initialize a
  ...
  
  std::vector<int> counts(parallel.getNumThreads());
 
  //parallel processing using dynamic parititioner 
  parallel.parallel_for(0, size, [&a, &counts](int i) {
    if (is_prime(a[i]))
    {
        counts[SimpleParallel::Scheduler::getCurrentThreadIndex()]++;
    }
  });
  
  //parallel processing using static partitioner
  parallel.parallel_for(0, size, [&a, &counts](int i) {
    if (is_prime(a[i]))
    {
        counts[SimpleParallel::Scheduler::getCurrentThreadIndex()]++;
    }
  }, SimpleParallel::StaticPartitioner::get());
  
}
```

#Partitioner

Partitioner make a decision how to distribute tasks.

```c++
class IPartitioner
	{
	public:
		virtual void ready(int start, int end, size_t numThreads) = 0;
		virtual inline bool getNext(int& out) = 0;
	};
```

You can easily make your own partitioner. Just inherit IPartitioner and implement 2 functions.

* ready() is called when parallel_for is be called on main thread.
* getNext() is called to get next task index. You must design it thread safely.

If you want to get sample codes. See StaticPartitioner.cpp and DynamicPartitioner.cpp

#Performance

You must check processing time before you use simpleParrallel. This is processing time for performance.cpp

This is a result on 3-core CPU.

```
Serial processing
primes: 123149
elapsed time: 6877ms

SimpleParallel
primes: 123149
elapsed time: 2266ms

OpenMP
primes: 123149
elapsed time: 2279ms

PPL
primes: 123149
elapsed time: 2255ms
```



