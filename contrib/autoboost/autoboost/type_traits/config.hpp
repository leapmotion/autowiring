
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_CONFIG_HPP_INCLUDED
#define AUTOBOOST_TT_CONFIG_HPP_INCLUDED

#ifndef AUTOBOOST_CONFIG_HPP
#include <autoboost/config.hpp>
#endif

#include <autoboost/detail/workaround.hpp>

//
// whenever we have a conversion function with ellipses
// it needs to be declared __cdecl to suppress compiler
// warnings from MS and Borland compilers (this *must*
// appear before we include is_same.hpp below):
#if defined(AUTOBOOST_MSVC) || (defined(__BORLANDC__) && !defined(AUTOBOOST_DISABLE_WIN32))
#   define AUTOBOOST_TT_DECL __cdecl
#else
#   define AUTOBOOST_TT_DECL /**/
#endif

# if (AUTOBOOST_WORKAROUND(__MWERKS__, < 0x3000)                         \
    || AUTOBOOST_WORKAROUND(__IBMCPP__, < 600 )                         \
    || AUTOBOOST_WORKAROUND(__BORLANDC__, < 0x5A0)                      \
    || defined(__ghs)                                               \
    || AUTOBOOST_WORKAROUND(__HP_aCC, < 60700)           \
    || AUTOBOOST_WORKAROUND(MPW_CPLUS, AUTOBOOST_TESTED_AT(0x890))          \
    || AUTOBOOST_WORKAROUND(__SUNPRO_CC, AUTOBOOST_TESTED_AT(0x580)))       \
    && defined(AUTOBOOST_NO_IS_ABSTRACT)

#   define AUTOBOOST_TT_NO_CONFORMING_IS_CLASS_IMPLEMENTATION 1

#endif

#ifndef AUTOBOOST_TT_NO_CONFORMING_IS_CLASS_IMPLEMENTATION
# define AUTOBOOST_TT_HAS_CONFORMING_IS_CLASS_IMPLEMENTATION 1
#endif

//
// define AUTOBOOST_TT_TEST_MS_FUNC_SIGS
// when we want to test __stdcall etc function types with is_function etc
// (Note, does not work with Borland, even though it does support __stdcall etc):
//
#if defined(_MSC_EXTENSIONS) && !defined(__BORLANDC__)
#  define AUTOBOOST_TT_TEST_MS_FUNC_SIGS
#endif

//
// define AUTOBOOST_TT_NO_CV_FUNC_TEST
// if tests for cv-qualified member functions don't
// work in is_member_function_pointer
//
#if AUTOBOOST_WORKAROUND(__MWERKS__, < 0x3000) || AUTOBOOST_WORKAROUND(__IBMCPP__, <= 600)
#  define AUTOBOOST_TT_NO_CV_FUNC_TEST
#endif

//
// Macros that have been deprecated, defined here for backwards compatibility:
//
#define AUTOBOOST_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION(x)
#define AUTOBOOST_TT_BROKEN_COMPILER_SPEC(x)

#endif // AUTOBOOST_TT_CONFIG_HPP_INCLUDED


