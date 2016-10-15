# SimpleParallel
This is a simple parallel programming library for c++. You can easily modify to fit your projects. 

# Usage

Scheduler::Scheduler([numThread = std::thread::hardware_concurrency])
* Make new parallel scheduler.
  * `size_t numThread` The number of threads. Threads are created when Scheduler's contructor be called.

```c++
#include "SimpleParallel.h"

static SimpleParallel::Scheduler parallel;
```

void Scheduller::parallel_for(start, end, task[, partitioner = DynamicPartitioner::get()])
* Run task in parallel.
  * `int start` Start Index of task
  * `int end` End Index + 1 of task.
  * `void(*task)(int)` Function to do. This function will be call in threads with integer parameters from **start** to **end - 1**
  * `IPartitioner partitioner` Partitioner object distribute indices to each threads. You can use `StaticPartitioner::get()` or `DynamicPartitioner::get()`
    * `StaticPartitioner` distribute tasks by count. All threads get same number of tasks. It spend very little time for scheduling. But  although there are sleeping threads and one thread is not ended, the sleeping threads just wait finishing.
    * `DynamicPartitioner` distribute tasks dynamically. It spend a little time for scheduling during parallel processing. But tasks will be distributed to a suitable thread dynamically.
    * Both partitioner use **lockfree** algorithm.

```c++
#include "SimpleParallel.h"

static SimpleParallel::Scheduler parallel;

int a[1000] = {1, 2, 3, 4, ....};
int b[1000] = {2, 3, 4, 5, ....};

parallel.parallel_for(0, 1000, [&a, &b](int i)
{
	a[i] = a[i] + b[i];
});

//now a is [3, 5, 7, 9, ....]
```

size_t Scheduler::getNumThreads();
* Return a number of threads.

```c++
#include "SimpleParallel.h"

static SimpleParallel::Scheduler parallel;

parallel.getNumThreads();

//return a number of threads. Default is the number of your processors;
```

static size_t Scheduler::getCurrentThreadIndex();
* Return a index of current thread

```c++
#include "SimpleParallel.h"

static SimpleParallel::Scheduler parallel;

SimpleParallel::Scheduler::getCurrentThreadIndex();
//return -1

parallel.parallel_for(0, 1000, [](int i)
{
	SimpleParallel::Scheduler::getCurrentThreadIndex();
	//return a index of current thread.
});
```

#Example
```c++

#include "SimpleParallel.h"

static SimpleParallel::Scheduler parallel;

main()
{
  
  
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

#Tips

* Make scheduler object as static. When scheduler be created, threads are created. When schduler be removed, threads are removed. If you want to get a better performance, Create scheduler just one time and do not remove until program be ended.
* If all tasks take similar time, Use StaticPartitioner. It spend a low time for scheduling indices than DynamicPartitioner.
