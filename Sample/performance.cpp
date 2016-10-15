
#include "SimpleParallel.h"
#include <iostream>
#include <random>
#include <chrono>
#include <ppl.h>

bool is_prime(int n)
{
	if (n < 2)
		return false;
	for (int i = 2; i < n; i++)
	{
		if ((n % i) == 0)
			return false;
	}
	return true;
}

void main()
{
	const size_t size = 1000000;

	int* a = new int[size];

	std::mt19937 gen;
	for (size_t i = 0; i < size; i++) {
		a[i] = gen() % 10000;
	}

	std::chrono::time_point<std::chrono::system_clock> start, end;

	{
		std::cout << "Serial processing" << std::endl;
		start = std::chrono::system_clock::now();

		int count = 0;
		for (int i = 0; i < size; i++)
		{
			if (is_prime(a[i]))
			{
				count++;
			}
		}
		end = std::chrono::system_clock::now();

		std::cout << "primes: " << count << std::endl;
		std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n" << std::endl;
	}

	{
		SimpleParallel::Scheduler parallel;

		std::cout << "SimpleParallel" << std::endl;
		start = std::chrono::system_clock::now();

		std::vector<int> counts(parallel.getNumThreads());
		parallel.parallel_for(0, size, [&a, &counts](int i) {
			if (is_prime(a[i]))
			{
				counts[SimpleParallel::Scheduler::getCurrentThreadIndex()]++;
			}
		});

		int count = 0;
		for (size_t i = 0; i < counts.size(); i++)
		{
			count += counts[i];
		}

		end = std::chrono::system_clock::now();

		std::cout << "primes: " << count << std::endl;
		std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n" << std::endl;
	}

	{
		std::cout << "OpenMP" << std::endl;
		start = std::chrono::system_clock::now();

		size_t count = 0;
#pragma omp parallel for
		for (int i = 0; i < static_cast<int>(size); ++i)
		{
			if (is_prime(a[i])) {
#pragma omp atomic
				++count;
			}
		}

		end = std::chrono::system_clock::now();

		std::cout << "primes: " << count << std::endl;
		std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n" << std::endl;
	}

	{
		std::cout << "PPL" << std::endl;
		start = std::chrono::system_clock::now();

		Concurrency::combinable<size_t> counts;
		Concurrency::parallel_for<size_t>(0, size, [&](size_t i)
		{
			if (is_prime(a[i])) {
				counts.local()++;
			}
		});

		int count = counts.combine(std::plus<size_t>());

		end = std::chrono::system_clock::now();

		std::cout << "primes: " << count << std::endl;
		std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n" << std::endl;
	}
}