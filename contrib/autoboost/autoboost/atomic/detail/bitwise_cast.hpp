/*
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * Copyright (c) 2009 Helge Bahmann
 * Copyright (c) 2012 Tim Blechmann
 * Copyright (c) 2013 - 2014 Andrey Semashev
 */
/*!
 * \file   atomic/detail/bitwise_cast.hpp
 *
 * This header defines \c bitwise_cast used to convert between storage and value types
 */

#ifndef AUTOBOOST_ATOMIC_DETAIL_BITWISE_CAST_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_DETAIL_BITWISE_CAST_HPP_INCLUDED_

#include <autoboost/atomic/detail/config.hpp>
#if !defined(AUTOBOOST_ATOMIC_DETAIL_HAS_BUILTIN_MEMCPY)
#include <cstring>
#endif

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace atomics {
namespace detail {

template< typename T >
AUTOBOOST_FORCEINLINE T* addressof(T& value) AUTOBOOST_NOEXCEPT
{
    // Note: The point of using a local struct as the intermediate type instead of char is to avoid gcc warnings
    // if T is a const volatile char*:
    // warning: casting ‘const volatile char* const’ to ‘const volatile char&’ does not dereference pointer
    // The local struct makes sure T is not related to the cast target type.
    struct opaque_type;
    return reinterpret_cast< T* >(&const_cast< opaque_type& >(reinterpret_cast< const volatile opaque_type& >(value)));
}

template< typename To, typename From >
AUTOBOOST_FORCEINLINE To bitwise_cast(From const& from) AUTOBOOST_NOEXCEPT
{
    struct
    {
        To to;
    }
    value = {};
    AUTOBOOST_ATOMIC_DETAIL_MEMCPY
    (
        atomics::detail::addressof(value.to),
        atomics::detail::addressof(from),
        (sizeof(From) < sizeof(To) ? sizeof(From) : sizeof(To))
    );
    return value.to;
}

} // namespace detail
} // namespace atomics
} // namespace autoboost

#endif // AUTOBOOST_ATOMIC_DETAIL_BITWISE_CAST_HPP_INCLUDED_
