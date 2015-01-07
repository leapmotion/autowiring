/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Tim Blechmann
 * (C) Copyright 2013 Andrey Semashev
 */

#ifndef AUTOBOOST_ATOMIC_DETAIL_PAUSE_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_DETAIL_PAUSE_HPP_INCLUDED_

#include <autoboost/atomic/detail/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_IX86))
extern "C" void _mm_pause(void);
#if defined(AUTOBOOST_MSVC)
#pragma intrinsic(_mm_pause)
#endif
#endif

namespace autoboost {
namespace atomics {
namespace detail {

AUTOBOOST_FORCEINLINE void pause() AUTOBOOST_NOEXCEPT
{
#if defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_IX86))
    _mm_pause();
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
    __asm__ __volatile__("pause;");
#endif
}

} // namespace detail
} // namespace atomics
} // namespace autoboost

#endif // AUTOBOOST_ATOMIC_DETAIL_PAUSE_HPP_INCLUDED_
