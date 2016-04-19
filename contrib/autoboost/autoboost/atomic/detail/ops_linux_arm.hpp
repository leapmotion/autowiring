/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2009, 2011 Helge Bahmann
 * Copyright (c) 2009 Phil Endecott
 * Copyright (c) 2013 Tim Blechmann
 * Linux-specific code by Phil Endecott
 * Copyright (c) 2014 Andrey Semashev
 */
/*!
 * \file   atomic/detail/ops_linux_arm.hpp
 *
 * This header contains implementation of the \c operations template.
 */

#ifndef AUTOBOOST_ATOMIC_DETAIL_OPS_LINUX_ARM_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_DETAIL_OPS_LINUX_ARM_HPP_INCLUDED_

#include <autoboost/memory_order.hpp>
#include <autoboost/atomic/detail/config.hpp>
#include <autoboost/atomic/detail/storage_type.hpp>
#include <autoboost/atomic/detail/operations_fwd.hpp>
#include <autoboost/atomic/capabilities.hpp>
#include <autoboost/atomic/detail/ops_cas_based.hpp>
#include <autoboost/atomic/detail/ops_extending_cas_based.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace atomics {
namespace detail {

// Different ARM processors have different atomic instructions.  In particular,
// architecture versions before v6 (which are still in widespread use, e.g. the
// Intel/Marvell XScale chips like the one in the NSLU2) have only atomic swap.
// On Linux the kernel provides some support that lets us abstract away from
// these differences: it provides emulated CAS and barrier functions at special
// addresses that are guaranteed not to be interrupted by the kernel.  Using
// this facility is slightly slower than inline assembler would be, but much
// faster than a system call.
//
// While this emulated CAS is "strong" in the sense that it does not fail
// "spuriously" (i.e.: it never fails to perform the exchange when the value
// found equals the value expected), it does not return the found value on
// failure. To satisfy the atomic API, compare_exchange_{weak|strong} must
// return the found value on failure, and we have to manually load this value
// after the emulated CAS reports failure. This in turn introduces a race
// between the CAS failing (due to the "wrong" value being found) and subsequently
// loading (which might turn up the "right" value). From an application's
// point of view this looks like "spurious failure", and therefore the
// emulated CAS is only good enough to provide compare_exchange_weak
// semantics.

struct linux_arm_cas_base
{
    static AUTOBOOST_FORCEINLINE void fence_before_store(memory_order order) AUTOBOOST_NOEXCEPT
    {
        if ((order & memory_order_release) != 0)
            hardware_full_fence();
    }

    static AUTOBOOST_FORCEINLINE void fence_after_store(memory_order order) AUTOBOOST_NOEXCEPT
    {
        if (order == memory_order_seq_cst)
            hardware_full_fence();
    }

    static AUTOBOOST_FORCEINLINE void fence_after_load(memory_order order) AUTOBOOST_NOEXCEPT
    {
        if ((order & (memory_order_consume | memory_order_acquire)) != 0)
            hardware_full_fence();
    }

    static AUTOBOOST_FORCEINLINE void hardware_full_fence() AUTOBOOST_NOEXCEPT
    {
        typedef void (*kernel_dmb_t)(void);
        ((kernel_dmb_t)0xffff0fa0)();
    }
};

template< bool Signed >
struct linux_arm_cas :
    public linux_arm_cas_base
{
    typedef typename make_storage_type< 4u, Signed >::type storage_type;
    typedef typename make_storage_type< 4u, Signed >::aligned aligned_storage_type;

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

    static AUTOBOOST_FORCEINLINE bool compare_exchange_strong(
        storage_type volatile& storage, storage_type& expected, storage_type desired, memory_order success_order, memory_order failure_order) AUTOBOOST_NOEXCEPT
    {
        while (true)
        {
            storage_type tmp = expected;
            if (compare_exchange_weak(storage, tmp, desired, success_order, failure_order))
                return true;
            if (tmp != expected)
            {
                expected = tmp;
                return false;
            }
        }
    }

    static AUTOBOOST_FORCEINLINE bool compare_exchange_weak(
        storage_type volatile& storage, storage_type& expected, storage_type desired, memory_order, memory_order) AUTOBOOST_NOEXCEPT
    {
        typedef storage_type (*kernel_cmpxchg32_t)(storage_type oldval, storage_type newval, volatile storage_type* ptr);

        if (((kernel_cmpxchg32_t)0xffff0fc0)(expected, desired, &storage) == 0)
        {
            return true;
        }
        else
        {
            expected = storage;
            return false;
        }
    }

    static AUTOBOOST_FORCEINLINE bool is_lock_free(storage_type const volatile&) AUTOBOOST_NOEXCEPT
    {
        return true;
    }
};

template< bool Signed >
struct operations< 1u, Signed > :
    public extending_cas_based_operations< cas_based_operations< cas_based_exchange< linux_arm_cas< Signed > > >, 1u, Signed >
{
};

template< bool Signed >
struct operations< 2u, Signed > :
    public extending_cas_based_operations< cas_based_operations< cas_based_exchange< linux_arm_cas< Signed > > >, 2u, Signed >
{
};

template< bool Signed >
struct operations< 4u, Signed > :
    public cas_based_operations< cas_based_exchange< linux_arm_cas< Signed > > >
{
};

AUTOBOOST_FORCEINLINE void thread_fence(memory_order order) AUTOBOOST_NOEXCEPT
{
    if (order != memory_order_relaxed)
        linux_arm_cas_base::hardware_full_fence();
}

AUTOBOOST_FORCEINLINE void signal_fence(memory_order order) AUTOBOOST_NOEXCEPT
{
    if (order != memory_order_relaxed)
        __asm__ __volatile__ ("" ::: "memory");
}

} // namespace detail
} // namespace atomics
} // namespace autoboost

#endif // AUTOBOOST_ATOMIC_DETAIL_OPS_LINUX_ARM_HPP_INCLUDED_
