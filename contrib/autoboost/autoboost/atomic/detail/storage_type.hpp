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
 * \file   atomic/detail/storage_type.hpp
 *
 * This header defines underlying types used as storage
 */

#ifndef AUTOBOOST_ATOMIC_DETAIL_STORAGE_TYPE_HPP_INCLUDED_
#define AUTOBOOST_ATOMIC_DETAIL_STORAGE_TYPE_HPP_INCLUDED_

#include <cstring>
#include <autoboost/cstdint.hpp>
#include <autoboost/atomic/detail/config.hpp>

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {
namespace atomics {
namespace detail {

template< unsigned int Size >
struct buffer_storage
{
    unsigned char data[Size];

    AUTOBOOST_FORCEINLINE bool operator! () const AUTOBOOST_NOEXCEPT
    {
        bool result = true;
        for (unsigned int i = 0; i < Size && result; ++i)
        {
            result &= data[i] == 0;
        }
        return result;
    }

    AUTOBOOST_FORCEINLINE bool operator== (buffer_storage const& that) const AUTOBOOST_NOEXCEPT
    {
        return std::memcmp(data, that.data, Size) == 0;
    }

    AUTOBOOST_FORCEINLINE bool operator!= (buffer_storage const& that) const AUTOBOOST_NOEXCEPT
    {
        return std::memcmp(data, that.data, Size) != 0;
    }
};

template< unsigned int Size, bool Signed >
struct make_storage_type
{
    typedef buffer_storage< Size > type;
};

template< >
struct make_storage_type< 1u, false >
{
    typedef autoboost::uint8_t type;
};

template< >
struct make_storage_type< 1u, true >
{
    typedef autoboost::int8_t type;
};

template< >
struct make_storage_type< 2u, false >
{
    typedef autoboost::uint16_t type;
};

template< >
struct make_storage_type< 2u, true >
{
    typedef autoboost::int16_t type;
};

template< >
struct make_storage_type< 4u, false >
{
    typedef autoboost::uint32_t type;
};

template< >
struct make_storage_type< 4u, true >
{
    typedef autoboost::int32_t type;
};

template< >
struct make_storage_type< 8u, false >
{
    typedef autoboost::uint64_t type;
};

template< >
struct make_storage_type< 8u, true >
{
    typedef autoboost::int64_t type;
};

#if defined(AUTOBOOST_HAS_INT128)

template< >
struct make_storage_type< 16u, false >
{
    typedef autoboost::uint128_type type;
};

template< >
struct make_storage_type< 16u, true >
{
    typedef autoboost::int128_type type;
};

#elif !defined(AUTOBOOST_NO_ALIGNMENT)

struct AUTOBOOST_ALIGNMENT(16) storage128_t
{
    autoboost::uint64_t data[2];

    AUTOBOOST_FORCEINLINE bool operator! () const AUTOBOOST_NOEXCEPT
    {
        return data[0] == 0 && data[1] == 0;
    }
};

AUTOBOOST_FORCEINLINE bool operator== (storage128_t const& left, storage128_t const& right) AUTOBOOST_NOEXCEPT
{
    return left.data[0] == right.data[0] && left.data[1] == right.data[1];
}
AUTOBOOST_FORCEINLINE bool operator!= (storage128_t const& left, storage128_t const& right) AUTOBOOST_NOEXCEPT
{
    return !(left == right);
}

template< bool Signed >
struct make_storage_type< 16u, Signed >
{
    typedef storage128_t type;
};

#endif

template< typename T >
struct storage_size_of
{
    enum _
    {
        size = sizeof(T),
        value = (size == 3 ? 4 : (size >= 5 && size <= 7 ? 8 : (size >= 9 && size <= 15 ? 16 : size)))
    };
};

} // namespace detail
} // namespace atomics
} // namespace autoboost

#endif // AUTOBOOST_ATOMIC_DETAIL_STORAGE_TYPE_HPP_INCLUDED_
