// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef AUTOBOOST_RANGE_CONFIG_HPP
#define AUTOBOOST_RANGE_CONFIG_HPP

#include <autoboost/detail/workaround.hpp>

#if defined(_MSC_VER)
# pragma once
#endif

#include <autoboost/config.hpp>

#ifdef AUTOBOOST_RANGE_DEDUCED_TYPENAME
#error "macro already defined!"
#endif

#if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x564))
# define AUTOBOOST_RANGE_DEDUCED_TYPENAME typename
#else
#define AUTOBOOST_RANGE_DEDUCED_TYPENAME AUTOBOOST_DEDUCED_TYPENAME
#endif

#ifdef AUTOBOOST_RANGE_NO_ARRAY_SUPPORT
#error "macro already defined!"
#endif

#if AUTOBOOST_WORKAROUND( __MWERKS__, <= 0x3003 )
#define AUTOBOOST_RANGE_NO_ARRAY_SUPPORT 1
#endif

#ifdef AUTOBOOST_RANGE_NO_ARRAY_SUPPORT
#define AUTOBOOST_RANGE_ARRAY_REF() (autoboost_range_array)
#define AUTOBOOST_RANGE_NO_STATIC_ASSERT
#else
#define AUTOBOOST_RANGE_ARRAY_REF() (&autoboost_range_array)
#endif

#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)))
#  define AUTOBOOST_RANGE_UNUSED __attribute__((unused))
#else
#  define AUTOBOOST_RANGE_UNUSED
#endif



#endif

