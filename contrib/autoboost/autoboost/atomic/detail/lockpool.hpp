/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2011 Helge Bahmann
 * Copyright (c) 2013-2014 Andrey Semashev
 */
/*!
 * \file   atomic/detail/lockpool.hpp
 *
 * This header contains declaration of the lockpool used to emulate atomic ops.
 */

#ifndef AUTOBOOST_ATOMIC_DETAIL_LOCKPOOL_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_DETAIL_LOCKPOOL_HPP_INCLUDED_

#include <autoboost/atomic/detail/config.hpp>
#include <autoboost/atomic/detail/link.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace atomics {
namespace detail {

struct lockpool
{
    class scoped_lock
    {
        void* m_lock;

    public:
        explicit AUTOBOOST_ATOMIC_DECL scoped_lock(const volatile void* addr) AUTOBOOST_NOEXCEPT;
        AUTOBOOST_ATOMIC_DECL ~scoped_lock() AUTOBOOST_NOEXCEPT;

        AUTOBOOST_DELETED_FUNCTION(scoped_lock(scoped_lock const&))
        AUTOBOOST_DELETED_FUNCTION(scoped_lock& operator=(scoped_lock const&))
    };

    static AUTOBOOST_ATOMIC_DECL void thread_fence() AUTOBOOST_NOEXCEPT;
    static AUTOBOOST_ATOMIC_DECL void signal_fence() AUTOBOOST_NOEXCEPT;
};

} // namespace detail
} // namespace atomics
} // namespace autoboost

#endif // AUTOBOOST_ATOMIC_DETAIL_LOCKPOOL_HPP_INCLUDED_
