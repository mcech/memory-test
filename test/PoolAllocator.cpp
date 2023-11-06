#include "gtest/gtest.h"

#define private public
#define protected public

#include "PoolAllocator.h"
#include <cstdlib>

bool fail_malloc = false;

void* operator new(size_t n)
{
    if (fail_malloc)
    {
        throw std::bad_alloc();
    }

    void* ptr = malloc(n);
    if (ptr == nullptr)
    {
        throw std::bad_alloc();
    }
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    free(ptr);
}

void operator delete(void* ptr, size_t) noexcept
{
    free(ptr);
}

namespace mcech::memory
{
    TEST(PoolAllocator, RebindConstructor)
    {
        PoolAllocator<short> s;
        PoolAllocator<int> i = s;

        EXPECT_EQ(i, s);
    }

    TEST(PoolAllocator, AllocateOne)
    {
        PoolAllocator<int> alloc;

        int* p = alloc.allocate(1);

        EXPECT_TRUE(p != nullptr);

        alloc.deallocate(p, 1);
        alloc.clear();
    }

    TEST(PoolAllocator, Reuse)
    {
        PoolAllocator<int> alloc;
        int* pointer1 = alloc.allocate(1);
        alloc.deallocate(pointer1, 1);

        int* pointer2 = alloc.allocate(1);

        EXPECT_EQ(pointer2, pointer1);

        alloc.deallocate(pointer2, 1);
        alloc.clear();
    }

    TEST(PoolAllocator, AllocateMany)
    {
        PoolAllocator<int> alloc;

        int* p = alloc.allocate(42);

        EXPECT_TRUE(p != nullptr);

        alloc.deallocate(p, 42);
        alloc.clear();
    }

    TEST(PoolAllocator, FailToAllocate)
    {
        PoolAllocator<int> alloc;
        fail_malloc = true;

        EXPECT_THROW((void)alloc.allocate(1), std::bad_alloc);

        fail_malloc = false;
    }

    TEST(PoolAllocator, DeallocateNull)
    {
        PoolAllocator<int> alloc;
        int* p = alloc.allocate(1);
        void* sentinel = alloc.pool.sentinel;

        alloc.deallocate(nullptr, 1);
        alloc.deallocate(nullptr, 42);
        alloc.deallocate(p, 0);

        EXPECT_EQ(alloc.pool.sentinel, sentinel);

        alloc.deallocate(p, 1);
        alloc.clear();
    }

    TEST(PoolAllocator, InterruptAllocate)
    {
        PoolAllocator<int> alloc;
        int* p = alloc.allocate(1);
        alloc.deallocate(p, 1); p = nullptr;

        typename PoolAllocator<int>::Node* node = alloc.pool.load();  // Thread 1
        p = alloc.allocate(1);                                        // Thread 2
        alloc.pool.compare_exchange(node, node->next);                // Thread 1

        alloc.deallocate(p, 1); p = nullptr;
        alloc.clear();
    }

    TEST(PoolAllocator, InterruptDeallocate)
    {
        PoolAllocator<int> alloc;
        int* p = alloc.allocate(1);
        typename PoolAllocator<int>::Node* node = new PoolAllocator<int>::Node;
        node->pool = &alloc.pool;

        node->next = node->pool->load();                 // Thread 1
        alloc.deallocate(p, 1); p = nullptr;             // Thread 2
        node->pool->compare_exchange(node->next, node);  // Thread 1

        alloc.clear();
    }
}
