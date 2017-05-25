/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2011 Helge Bahmann
 * Copyright (c) 2013 Tim Blechmann
 * Copyright (c) 2014 Andrey Semashev
 */
/*!
 * \file   atomic/detail/ops_gcc_sync.hpp
 *
 * This header contains implementation of the \c operations template.
 */

#ifndef AUTOBOOST_ATOMIC_DETAIL_OPS_GCC_SYNC_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_DETAIL_OPS_GCC_SYNC_HPP_INCLUDED_

#include <autoboost/memory_order.hpp>
#include <autoboost/atomic/detail/config.hpp>
#include <autoboost/atomic/detail/storage_type.hpp>
#include <autoboost/atomic/detail/operations_fwd.hpp>
#include <autoboost/atomic/detail/ops_extending_cas_based.hpp>
#include <autoboost/atomic/capabilities.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace atomics {
namespace detail {

struct gcc_sync_operations_base
{
    static AUTOBOOST_CONSTEXPR_OR_CONST bool is_always_lock_free = true;

    static AUTOBOOST_FORCEINLINE void fence_before_store(memory_order order) AUTOBOOST_NOEXCEPT
    {
        if ((order & memory_order_release) != 0)
            __sync_synchronize();
    }

    static AUTOBOOST_FORCEINLINE void fence_after_store(memory_order order) AUTOBOOST_NOEXCEPT
    {
        if (order == memory_order_seq_cst)
            __sync_synchronize();
    }

    static AUTOBOOST_FORCEINLINE void fence_after_load(memory_order order) AUTOBOOST_NOEXCEPT
    {
        if ((order & (memory_order_acquire | memory_order_consume)) != 0)
            __sync_synchronize();
    }
};

template< typename T >
struct gcc_sync_operations :
    public gcc_sync_operations_base
{
    typedef T storage_type;

    static AUTOBOOST_FORCEINLINE void store(storage_type volatile& storage, storage_type v, memory_order order) AUTOBOOST_NOEXCEPT
    {
        fence_before_store(order);
        storage = v;
        fence_after_store(order);
    }

    static AUTOBOOST_FORCEINLINE storage_type load(storage_type const volatile& storage, memory_order order) AUTOBOOST_NOEXCEPT
    {
        storage_type v = storage;
        fence_after_load(order);
        return v;
    }

    static AUTOBOOST_FORCEINLINE storage_type fetch_add(storage_type volatile& storage, storage_type v, memory_order) AUTOBOOST_NOEXCEPT
    {
        return __sync_fetch_and_add(&storage, v);
    }

    static AUTOBOOST_FORCEINLINE storage_type fetch_sub(storage_type volatile& storage, storage_type v, memory_order) AUTOBOOST_NOEXCEPT
    {
        return __sync_fetch_and_sub(&storage, v);
    }

    static AUTOBOOST_FORCEINLINE storage_type exchange(storage_type volatile& storage, storage_type v, memory_order order) AUTOBOOST_NOEXCEPT
    {
        // GCC docs mention that not all architectures may support full exchange semantics for this intrinsic. However, GCC's implementation of
        // std::atomic<> uses this intrinsic unconditionally. We do so as well. In case if some architectures actually don't support this, we can always
        // add a check here and fall back to a CAS loop.
        if ((order & memory_order_release) != 0)
            __sync_synchronize();
        return __sync_lock_test_and_set(&storage, v);
    }

    static AUTOBOOST_FORCEINLINE bool compare_exchange_strong(
        storage_type volatile& storage, storage_type& expected, storage_type desired, memory_order, memory_order) AUTOBOOST_NOEXCEPT
    {
        storage_type expected2 = expected;
        storage_type old_val = __sync_val_compare_and_swap(&storage, expected2, desired);

        if (old_val == expected2)
        {
            return true;
        }
        else
        {
            expected = old_val;
            return false;
        }
    }

    static AUTOBOOST_FORCEINLINE bool compare_exchange_weak(
        storage_type volatile& storage, storage_type& expected, storage_type desired, memory_order success_order, memory_order failure_order) AUTOBOOST_NOEXCEPT
    {
        return compare_exchange_strong(storage, expected, desired, success_order, failure_order);
    }

    static AUTOBOOST_FORCEINLINE storage_type fetch_and(storage_type volatile& storage, storage_type v, memory_order) AUTOBOOST_NOEXCEPT
    {
        return __sync_fetch_and_and(&storage, v);
    }

    static AUTOBOOST_FORCEINLINE storage_type fetch_or(storage_type volatile& storage, storage_type v, memory_order) AUTOBOOST_NOEXCEPT
    {
        return __sync_fetch_and_or(&storage, v);
    }

    static AUTOBOOST_FORCEINLINE storage_type fetch_xor(storage_type volatile& storage, storage_type v, memory_order) AUTOBOOST_NOEXCEPT
    {
        return __sync_fetch_and_xor(&storage, v);
    }

    static AUTOBOOST_FORCEINLINE bool test_and_set(storage_type volatile& storage, memory_order order) AUTOBOOST_NOEXCEPT
    {
        if ((order & memory_order_release) != 0)
            __sync_synchronize();
        return !!__sync_lock_test_and_set(&storage, 1);
    }

    static AUTOBOOST_FORCEINLINE void clear(storage_type volatile& storage, memory_order order) AUTOBOOST_NOEXCEPT
    {
        __sync_lock_release(&storage);
        if (order == memory_order_seq_cst)
            __sync_synchronize();
    }

    static AUTOBOOST_FORCEINLINE bool is_lock_free(storage_type const volatile&) AUTOBOOST_NOEXCEPT
    {
        return true;
    }
};

#if AUTOBOOST_ATOMIC_INT8_LOCK_FREE > 0
template< bool Signed >
struct operations< 1u, Signed > :
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1)
    public gcc_sync_operations< typename make_storage_type< 1u, Signed >::type >
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2)
    public extending_cas_based_operations< gcc_sync_operations< typename make_storage_type< 2u, Signed >::type >, 1u, Signed >
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
    public extending_cas_based_operations< gcc_sync_operations< typename make_storage_type< 4u, Signed >::type >, 1u, Signed >
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
    public extending_cas_based_operations< gcc_sync_operations< typename make_storage_type< 8u, Signed >::type >, 1u, Signed >
#else
    public extending_cas_based_operations< gcc_sync_operations< typename make_storage_type< 16u, Signed >::type >, 1u, Signed >
#endif
{
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1)
    typedef typename make_storage_type< 1u, Signed >::aligned aligned_storage_type;
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2)
    typedef typename make_storage_type< 2u, Signed >::aligned aligned_storage_type;
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
    typedef typename make_storage_type< 4u, Signed >::aligned aligned_storage_type;
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
    typedef typename make_storage_type< 8u, Signed >::aligned aligned_storage_type;
#else
    typedef typename make_storage_type< 16u, Signed >::aligned aligned_storage_type;
#endif
};
#endif

#if AUTOBOOST_ATOMIC_INT16_LOCK_FREE > 0
template< bool Signed >
struct operations< 2u, Signed > :
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2)
    public gcc_sync_operations< typename make_storage_type< 2u, Signed >::type >
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
    public extending_cas_based_operations< gcc_sync_operations< typename make_storage_type< 4u, Signed >::type >, 2u, Signed >
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
    public extending_cas_based_operations< gcc_sync_operations< typename make_storage_type< 8u, Signed >::type >, 2u, Signed >
#else
    public extending_cas_based_operations< gcc_sync_operations< typename make_storage_type< 16u, Signed >::type >, 2u, Signed >
#endif
{
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2)
    typedef typename make_storage_type< 2u, Signed >::aligned aligned_storage_type;
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
    typedef typename make_storage_type< 4u, Signed >::aligned aligned_storage_type;
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
    typedef typename make_storage_type< 8u, Signed >::aligned aligned_storage_type;
#else
    typedef typename make_storage_type< 16u, Signed >::aligned aligned_storage_type;
#endif
};
#endif

#if AUTOBOOST_ATOMIC_INT32_LOCK_FREE > 0
template< bool Signed >
struct operations< 4u, Signed > :
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
    public gcc_sync_operations< typename make_storage_type< 4u, Signed >::type >
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
    public extending_cas_based_operations< gcc_sync_operations< typename make_storage_type< 8u, Signed >::type >, 4u, Signed >
#else
    public extending_cas_based_operations< gcc_sync_operations< typename make_storage_type< 16u, Signed >::type >, 4u, Signed >
#endif
{
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
    typedef typename make_storage_type< 4u, Signed >::aligned aligned_storage_type;
#elif defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
    typedef typename make_storage_type< 8u, Signed >::aligned aligned_storage_type;
#else
    typedef typename make_storage_type< 16u, Signed >::aligned aligned_storage_type;
#endif
};
#endif

#if AUTOBOOST_ATOMIC_INT64_LOCK_FREE > 0
template< bool Signed >
struct operations< 8u, Signed > :
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
    public gcc_sync_operations< typename make_storage_type< 8u, Signed >::type >
#else
    public extending_cas_based_operations< gcc_sync_operations< typename make_storage_type< 16u, Signed >::type >, 8u, Signed >
#endif
{
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
    typedef typename make_storage_type< 8u, Signed >::aligned aligned_storage_type;
#else
    typedef typename make_storage_type< 16u, Signed >::aligned aligned_storage_type;
#endif
};
#endif

#if AUTOBOOST_ATOMIC_INT128_LOCK_FREE > 0
template< bool Signed >
struct operations< 16u, Signed > :
    public gcc_sync_operations< typename make_storage_type< 16u, Signed >::type >
{
    typedef typename make_storage_type< 16u, Signed >::aligned aligned_storage_type;
};
#endif

AUTOBOOST_FORCEINLINE void thread_fence(memory_order order) AUTOBOOST_NOEXCEPT
{
    if (order != memory_order_relaxed)
        __sync_synchronize();
}

AUTOBOOST_FORCEINLINE void signal_fence(memory_order order) AUTOBOOST_NOEXCEPT
{
    if (order != memory_order_relaxed)
        __asm__ __volatile__ ("" ::: "memory");
}

} // namespace detail
} // namespace atomics
} // namespace autoboost

#endif // AUTOBOOST_ATOMIC_DETAIL_OPS_GCC_SYNC_HPP_INCLUDED_
