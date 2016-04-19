//  config.hpp  ---------------------------------------------------------------//

//  Copyright 2012 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#ifndef AUTOBOOST_RATIO_CONFIG_HPP
#define AUTOBOOST_RATIO_CONFIG_HPP

#include <autoboost/config.hpp>
#include <autoboost/cstdint.hpp>


#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 5) || !defined(__GXX_EXPERIMENTAL_CXX0X__)
#  if ! defined AUTOBOOST_NO_CXX11_U16STRING
#    define AUTOBOOST_NO_CXX11_U16STRING
#  endif
#  if ! defined AUTOBOOST_NO_CXX11_U32STRING
#    define AUTOBOOST_NO_CXX11_U32STRING
#  endif
#endif


#if !defined AUTOBOOST_RATIO_VERSION
#define AUTOBOOST_RATIO_VERSION 1
#else
#if AUTOBOOST_RATIO_VERSION!=1  && AUTOBOOST_RATIO_VERSION!=2
#error "AUTOBOOST_RATIO_VERSION must be 1 or 2"
#endif
#endif

#if AUTOBOOST_RATIO_VERSION==1
#if ! defined AUTOBOOST_RATIO_DONT_PROVIDE_DEPRECATED_FEATURES_SINCE_V2_0_0
#define AUTOBOOST_RATIO_PROVIDES_DEPRECATED_FEATURES_SINCE_V2_0_0
#endif
#endif

#if AUTOBOOST_RATIO_VERSION==2
#if ! defined AUTOBOOST_RATIO_PROVIDES_DEPRECATED_FEATURES_SINCE_V2_0_0
#define AUTOBOOST_RATIO_DONT_PROVIDE_DEPRECATED_FEATURES_SINCE_V2_0_0
#endif
#endif

#ifdef INTMAX_C
#define AUTOBOOST_RATIO_INTMAX_C(a) INTMAX_C(a)
#else
#define AUTOBOOST_RATIO_INTMAX_C(a) a##LL
#endif

#ifdef UINTMAX_C
#define AUTOBOOST_RATIO_UINTMAX_C(a) UINTMAX_C(a)
#else
#define AUTOBOOST_RATIO_UINTMAX_C(a) a##ULL
#endif

#define AUTOBOOST_RATIO_INTMAX_T_MAX (0x7FFFFFFFFFFFFFFELL)


#ifndef AUTOBOOST_NO_CXX11_STATIC_ASSERT
#define AUTOBOOST_RATIO_STATIC_ASSERT(CND, MSG, TYPES) static_assert(CND,MSG)
#elif defined(AUTOBOOST_RATIO_USES_STATIC_ASSERT)
#include <autoboost/static_assert.hpp>
#define AUTOBOOST_RATIO_STATIC_ASSERT(CND, MSG, TYPES) AUTOBOOST_STATIC_ASSERT(CND)
#elif defined(AUTOBOOST_RATIO_USES_MPL_ASSERT)
#include <autoboost/mpl/assert.hpp>
#include <autoboost/mpl/bool.hpp>
#define AUTOBOOST_RATIO_STATIC_ASSERT(CND, MSG, TYPES)                                 \
    AUTOBOOST_MPL_ASSERT_MSG(autoboost::mpl::bool_< (CND) >::type::value, MSG, TYPES)
#else
//~ #elif defined(AUTOBOOST_RATIO_USES_ARRAY_ASSERT)
#define AUTOBOOST_RATIO_CONCAT(A,B) A##B
#define AUTOBOOST_RATIO_NAME(A,B) AUTOBOOST_RATIO_CONCAT(A,B)
#define AUTOBOOST_RATIO_STATIC_ASSERT(CND, MSG, TYPES) static char AUTOBOOST_RATIO_NAME(__autoboost_ratio_test_,__LINE__)[(CND)?1:-1]
//~ #define AUTOBOOST_RATIO_STATIC_ASSERT(CND, MSG, TYPES)
#endif

#if !defined(AUTOBOOST_NO_CXX11_STATIC_ASSERT) || !defined(AUTOBOOST_RATIO_USES_MPL_ASSERT)
#define AUTOBOOST_RATIO_OVERFLOW_IN_ADD "overflow in ratio add"
#define AUTOBOOST_RATIO_OVERFLOW_IN_SUB "overflow in ratio sub"
#define AUTOBOOST_RATIO_OVERFLOW_IN_MUL "overflow in ratio mul"
#define AUTOBOOST_RATIO_OVERFLOW_IN_DIV "overflow in ratio div"
#define AUTOBOOST_RATIO_NUMERATOR_IS_OUT_OF_RANGE "ratio numerator is out of range"
#define AUTOBOOST_RATIO_DIVIDE_BY_0 "ratio divide by 0"
#define AUTOBOOST_RATIO_DENOMINATOR_IS_OUT_OF_RANGE "ratio denominator is out of range"
#endif


//#define AUTOBOOST_RATIO_EXTENSIONS

#endif  // header
