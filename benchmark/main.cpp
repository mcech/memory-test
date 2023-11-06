#include "PoolAllocator.h"
#include <chrono>
#include <forward_list>
#include <iostream>
#include <thread>

template <class Alloc>
void run()
{
    std::forward_list<int, Alloc> lst;
    for (int i = 0; i < 4096; ++i)
    {
        int m = rand() % 4096;
        for (int j = 0; j < m; ++j)
        {
            lst.emplace_after(lst.before_begin(), j);
        }
        lst.clear();
    }
}

template <class Alloc>
std::chrono::milliseconds benchmark()
{
    const size_t N = std::thread::hardware_concurrency();
    std::thread t[N];
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    for (size_t i = 0; i < N; ++i)
    {
        t[i] = std::thread(run<Alloc>);
    }
    for (size_t i = 0; i < N; ++i)
    {
        t[i].join();
    }

    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

int main()
{
    constexpr size_t RUNS = 16;

    std::chrono::milliseconds default_allocator = {};
    for (size_t i = 0; i < RUNS; ++i)
    {
        default_allocator += benchmark<std::allocator<int>>();
    }
    default_allocator /= RUNS;
    std::cout << "Default Allocator: " << default_allocator << std::endl;

    std::chrono::milliseconds pool_allocator = {};
    for (size_t i = 0; i < RUNS; ++i)
    {
        pool_allocator += benchmark<mcech::memory::PoolAllocator<int>>();
    }
    pool_allocator /= RUNS;
    std::cout << "Pool Allocator:    " << pool_allocator << std::endl;

    return 0;
}
