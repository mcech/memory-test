#include "gtest/gtest.h"

#define private public
#define protected public

#include "PoolAllocator.h"

namespace mcech::memory
{
    TEST(PoolAllocator, EmptySharedPool)
    {
        PoolAllocator<int>::SharedPool pool;

        ASSERT_EQ(pool.load(), nullptr);
    }

    TEST(PoolAllocator, TrySharedPool)
    {
        PoolAllocator<int>::SharedPool pool;
        PoolAllocator<int>::Node* a = static_cast<PoolAllocator<int>::Node*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        PoolAllocator<int>::Node* expected = nullptr;

        a->next = nullptr; pool.try_compare_exchange(expected, a);
        ASSERT_EQ(expected, nullptr);
        ASSERT_EQ(pool.load(), a);
    }

    TEST(PoolAllocator, SetSharedPool)
    {
        PoolAllocator<int>::SharedPool pool;
        PoolAllocator<int>::Node* a = static_cast<PoolAllocator<int>::Node*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        PoolAllocator<int>::Node* expected = nullptr;
        a->next = nullptr; pool.try_compare_exchange(expected, a);

        pool.compare_exchange(expected, nullptr);
        ASSERT_EQ(expected, a);
        ASSERT_EQ(pool.load(), nullptr);

        ::operator delete(static_cast<void*>(a));
    }

    TEST(PoolAllocator, EmptyLocalPool)
    {
        PoolAllocator<int>::SharedPool shared;
        PoolAllocator<int>::LocalPool local(shared);

        ASSERT_EQ(static_cast<PoolAllocator<int>::Node*>(local), nullptr);
    }

    TEST(PoolAllocator, UsedLocalPool)
    {
        PoolAllocator<int>::SharedPool shared;
        PoolAllocator<int>::LocalPool local(shared);
        PoolAllocator<int>::Node* a = static_cast<PoolAllocator<int>::Node*>(::operator new(sizeof(PoolAllocator<int>::Node)));

        a->next = nullptr; local = a;
        ASSERT_EQ(local, a);
    }

    TEST(PoolAllocator, UsedSharedPool)
    {
        PoolAllocator<int>::SharedPool shared;
        PoolAllocator<int>::Node* a = static_cast<PoolAllocator<int>::Node*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        PoolAllocator<int>::Node* b = static_cast<PoolAllocator<int>::Node*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        PoolAllocator<int>::Node* expected = nullptr;
        a->next = nullptr; shared.try_compare_exchange(expected, a);

        {
            PoolAllocator<int>::LocalPool local(shared);
            b->next = nullptr; local = b;
        }
        ASSERT_EQ(b, shared.load());
    }

    TEST(PoolAllocator, Allocate)
    {
        PoolAllocator<int> pool;

        int* a = pool.alloc(1);
        ASSERT_NE(a, nullptr);

        pool.dealloc(a, 1);
    }

    TEST(PoolAllocator, Reuse)
    {
        PoolAllocator<int> pool;
        int* a = static_cast<int*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        pool.add_to_shared_pool(a);

        pool.move_from_shared_to_local_pool();
        int* b = pool.reuse_from_local_pool();
        ASSERT_EQ(b, a);
    }

    TEST(PoolAllocator, FailReuse)
    {
        PoolAllocator<int> pool;

        int* a = pool.reuse_from_local_pool();
        ASSERT_EQ(a, nullptr);
    }

    TEST(PoolAllocator, FailMoveToLocalPool)
    {
        PoolAllocator<int> pool;
        int* a = static_cast<int*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        int* b = static_cast<int*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        pool.add_to_shared_pool(a);
        pool.move_from_shared_to_local_pool();
        pool.add_to_shared_pool(b);

        pool.move_from_shared_to_local_pool();
        int* c = pool.reuse_from_local_pool();
        ASSERT_EQ(c, a);
    }

    TEST(PoolAllocator, MoveBackToSharedPool)
    {
        PoolAllocator<int> pool;
        int* a = static_cast<int*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        int* b = static_cast<int*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        pool.add_to_shared_pool(a);
        pool.add_to_shared_pool(b);
        pool.move_from_shared_to_local_pool();
        b = pool.reuse_from_local_pool();

        pool.move_from_local_to_shared_pool();
        int* c = pool.reuse_from_local_pool();
        ASSERT_EQ(c, nullptr);
    }

    TEST(PoolAllocator, FailMoveBackToSharedPool)
    {
        PoolAllocator<int> pool;
        int* a = static_cast<int*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        int* b = static_cast<int*>(::operator new(sizeof(PoolAllocator<int>::Node)));
        pool.add_to_shared_pool(a);
        pool.move_from_shared_to_local_pool();
        pool.add_to_shared_pool(b);

        pool.move_from_local_to_shared_pool();
        int* c = pool.reuse_from_local_pool();
        ASSERT_EQ(c, a);
    }

    TEST(PoolAllocator, CopyConstructor)
    {
        PoolAllocator<short> s;
        PoolAllocator<int> i = s;

        ASSERT_EQ(s, i);
    }

    TEST(PoolAllocator, AllocateOneFallback)
    {
        PoolAllocator<long> alloc;

        long* p = alloc.allocate(1);
        EXPECT_TRUE(p != nullptr);

        alloc.deallocate(p, 1);
    }

    TEST(PoolAllocator, AllocateRuse)
    {
        PoolAllocator<long> alloc;
        long* p = alloc.allocate(1);
        alloc.deallocate(p, 1);

        p = alloc.allocate(1);
        EXPECT_TRUE(p != nullptr);

        alloc.deallocate(p, 1);
    }

    TEST(PoolAllocator, AllocateMany)
    {
        PoolAllocator<long> alloc;

        long* p = alloc.allocate(42);
        EXPECT_TRUE(p != nullptr);

        alloc.deallocate(p, 42);
    }

    TEST(PoolAllocator, Comparison)
    {
        PoolAllocator<short> s;
        PoolAllocator<int> i;

        ASSERT_EQ(s, i);
    }
}
