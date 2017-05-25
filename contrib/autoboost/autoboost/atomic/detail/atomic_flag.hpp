/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2014 Andrey Semashev
 */
/*!
 * \file   atomic/detail/atomic_flag.hpp
 *
 * This header contains interface definition of \c atomic_flag.
 */

#ifndef AUTOBOOST_ATOMIC_DETAIL_ATOMIC_FLAG_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_DETAIL_ATOMIC_FLAG_HPP_INCLUDED_

#include <autoboost/assert.hpp>
#include <autoboost/memory_order.hpp>
#include <autoboost/atomic/detail/config.hpp>
#include <autoboost/atomic/detail/operations_lockfree.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

/*
 * IMPLEMENTATION NOTE: All interface functions MUST be declared with AUTOBOOST_FORCEINLINE,
 *                      see comment for convert_memory_order_to_gcc in ops_gcc_atomic.hpp.
 */

namespace autoboost {
namespace atomics {

#if defined(AUTOBOOST_NO_CXX11_CONSTEXPR) || defined(AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX)
#define AUTOBOOST_ATOMIC_NO_ATOMIC_FLAG_INIT
#else
#define AUTOBOOST_ATOMIC_FLAG_INIT {}
#endif

struct atomic_flag
{
    typedef atomics::detail::operations< 1u, false > operations;
    typedef operations::storage_type storage_type;

    operations::aligned_storage_type m_storage;

    AUTOBOOST_FORCEINLINE AUTOBOOST_CONSTEXPR atomic_flag() AUTOBOOST_NOEXCEPT : m_storage(0)
    {
    }

    AUTOBOOST_FORCEINLINE bool test_and_set(memory_order order = memory_order_seq_cst) volatile AUTOBOOST_NOEXCEPT
    {
        return operations::test_and_set(m_storage.value, order);
    }

    AUTOBOOST_FORCEINLINE void clear(memory_order order = memory_order_seq_cst) volatile AUTOBOOST_NOEXCEPT
    {
        AUTOBOOST_ASSERT(order != memory_order_acquire);
        AUTOBOOST_ASSERT(order != memory_order_acq_rel);
        operations::clear(m_storage.value, order);
    }

    AUTOBOOST_DELETED_FUNCTION(atomic_flag(atomic_flag const&))
    AUTOBOOST_DELETED_FUNCTION(atomic_flag& operator= (atomic_flag const&))
};

} // namespace atomics
} // namespace autoboost

#endif // AUTOBOOST_ATOMIC_DETAIL_ATOMIC_FLAG_HPP_INCLUDED_
