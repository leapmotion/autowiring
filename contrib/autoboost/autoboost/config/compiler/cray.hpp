//  (C) Copyright John Maddock 2011.
//  (C) Copyright Cray, Inc. 2013
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Greenhills C compiler setup:

#define AUTOBOOST_COMPILER "Cray C version " AUTOBOOST_STRINGIZE(_RELEASE)

#if _RELEASE < 8
#  error "Boost is not configured for Cray compilers prior to version 8, please try the configure script."
#endif

//
// Check this is a recent EDG based compiler, otherwise we don't support it here:
//
#ifndef __EDG_VERSION__
#  error "Unsupported Cray compiler, please try running the configure script."
#endif

#include "autoboost/config/compiler/common_edg.hpp"


//
//
#define AUTOBOOST_NO_CXX11_STATIC_ASSERT
#define AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS
#define AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#define AUTOBOOST_HAS_NRVO
#define AUTOBOOST_NO_CXX11_VARIADIC_MACROS
#define AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
#define AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#define AUTOBOOST_NO_CXX11_UNICODE_LITERALS
#define AUTOBOOST_NO_TWO_PHASE_NAME_LOOKUP
#define AUTOBOOST_HAS_NRVO
#define AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES
#define AUTOBOOST_NO_CXX11_STATIC_ASSERT
#define AUTOBOOST_NO_SFINAE_EXPR
#define AUTOBOOST_NO_CXX11_SCOPED_ENUMS
#define AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#define AUTOBOOST_NO_CXX11_RANGE_BASED_FOR
#define AUTOBOOST_NO_CXX11_RAW_LITERALS
#define AUTOBOOST_NO_CXX11_NULLPTR
#define AUTOBOOST_NO_CXX11_NOEXCEPT
#define AUTOBOOST_NO_CXX11_LAMBDAS
#define AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#define AUTOBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#define AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS
#define AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#define AUTOBOOST_NO_CXX11_DECLTYPE_N3276
#define AUTOBOOST_NO_CXX11_DECLTYPE
#define AUTOBOOST_NO_CXX11_CONSTEXPR
#define AUTOBOOST_NO_CXX11_USER_DEFINED_LITERALS
#define AUTOBOOST_NO_COMPLETE_VALUE_INITIALIZATION
#define AUTOBOOST_NO_CXX11_CHAR32_T
#define AUTOBOOST_NO_CXX11_CHAR16_T
#define AUTOBOOST_NO_CXX11_REF_QUALIFIERS
#define AUTOBOOST_NO_CXX11_FINAL


//#define AUTOBOOST_BCB_PARTIAL_SPECIALIZATION_BUG
#define AUTOBOOST_MATH_DISABLE_STD_FPCLASSIFY
//#define AUTOBOOST_HAS_FPCLASSIFY

#define AUTOBOOST_SP_USE_PTHREADS
#define AUTOBOOST_AC_USE_PTHREADS

/* everything that follows is working around what are thought to be
 * compiler shortcomings.  Revist all of these regularly.
 */

//#define AUTOBOOST_USE_ENUM_STATIC_ASSERT
//#define AUTOBOOST_BUGGY_INTEGRAL_CONSTANT_EXPRESSIONS //(this may be implied by the previous #define

// These constants should be provided by the
// compiler, at least when -hgnu is asserted on the command line.

#ifndef __ATOMIC_RELAXED
#define __ATOMIC_RELAXED 0
#define __ATOMIC_CONSUME 1
#define __ATOMIC_ACQUIRE 2
#define __ATOMIC_RELEASE 3
#define __ATOMIC_ACQ_REL 4
#define __ATOMIC_SEQ_CST 5
#endif



