//  underlying_type.hpp  ---------------------------------------------------------//

//  Copyright Beman Dawes, 2009
//  Copyright (C) 2011-2012 Vicente J. Botet Escriba
//  Copyright (C) 2012 Anthony Williams
//  Copyright (C) 2014 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#ifndef AUTOBOOST_CORE_UNDERLYING_TYPE_HPP
#define AUTOBOOST_CORE_UNDERLYING_TYPE_HPP

#include <autoboost/config.hpp>

// GCC 4.7 and later seem to provide std::underlying_type
#if !defined(AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS) || (defined(AUTOBOOST_GCC) && AUTOBOOST_GCC >= 40700 && defined(__GXX_EXPERIMENTAL_CXX0X__))
#include <type_traits>
#define AUTOBOOST_DETAIL_HAS_STD_UNDERLYING_TYPE
#endif

#ifdef AUTOBOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace autoboost {

namespace detail {

template< typename EnumType, typename Void = void >
struct underlying_type_impl;

#if defined(AUTOBOOST_NO_CXX11_SCOPED_ENUMS)

// Support for autoboost/core/scoped_enum.hpp
template< typename EnumType >
struct underlying_type_impl< EnumType, typename EnumType::is_autoboost_scoped_enum_tag >
{
    /**
     * The member typedef type names the underlying type of EnumType. It is EnumType::underlying_type when the EnumType is an emulated scoped enum,
     */
    typedef typename EnumType::underlying_type type;
};

#endif

#if defined(AUTOBOOST_DETAIL_HAS_STD_UNDERLYING_TYPE)

template< typename EnumType, typename Void >
struct underlying_type_impl
{
    typedef typename std::underlying_type< EnumType >::type type;
};

#endif

} // namespace detail

#if !defined(AUTOBOOST_NO_CXX11_SCOPED_ENUMS) && !defined(AUTOBOOST_DETAIL_HAS_STD_UNDERLYING_TYPE)
#define AUTOBOOST_NO_UNDERLYING_TYPE
#endif

/**
 * Meta-function to get the underlying type of a scoped enum.
 *
 * Requires EnumType must be an enum type or the emulation of a scoped enum.
 * If AUTOBOOST_NO_UNDERLYING_TYPE is defined, the implementation will not be able
 * to deduce the underlying type of enums. The user is expected to specialize
 * this trait in this case.
 */
template< typename EnumType >
struct underlying_type :
    public detail::underlying_type_impl< EnumType >
{
};

} // namespace autoboost

#endif  // AUTOBOOST_CORE_UNDERLYING_TYPE_HPP
