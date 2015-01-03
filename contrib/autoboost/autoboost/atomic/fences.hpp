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
 * \file   atomic/fences.hpp
 *
 * This header contains definition of \c atomic_thread_fence and \c atomic_signal_fence functions.
 */

#ifndef AUTOBOOST_ATOMIC_FENCES_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_FENCES_HPP_INCLUDED_

#include <autoboost/memory_order.hpp>
#include <autoboost/atomic/capabilities.hpp>
#include <autoboost/atomic/detail/operations.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

/*
 * IMPLEMENTATION NOTE: All interface functions MUST be declared with AUTOBOOST_FORCEINLINE,
 *                      see comment for convert_memory_order_to_gcc in ops_gcc_atomic.hpp.
 */

namespace autoboost {

namespace atomics {

#if AUTOBOOST_ATOMIC_THREAD_FENCE > 0
AUTOBOOST_FORCEINLINE void atomic_thread_fence(memory_order order) AUTOBOOST_NOEXCEPT
{
    detail::thread_fence(order);
}
#else
AUTOBOOST_FORCEINLINE void atomic_thread_fence(memory_order) AUTOBOOST_NOEXCEPT
{
    detail::lockpool::thread_fence();
}
#endif

#if AUTOBOOST_ATOMIC_SIGNAL_FENCE > 0
AUTOBOOST_FORCEINLINE void atomic_signal_fence(memory_order order) AUTOBOOST_NOEXCEPT
{
    detail::signal_fence(order);
}
#else
AUTOBOOST_FORCEINLINE void atomic_signal_fence(memory_order) AUTOBOOST_NOEXCEPT
{
    detail::lockpool::signal_fence();
}
#endif

} // namespace atomics

using atomics::atomic_thread_fence;
using atomics::atomic_signal_fence;

} // namespace autoboost

#endif // AUTOBOOST_ATOMIC_FENCES_HPP_INCLUDED_
