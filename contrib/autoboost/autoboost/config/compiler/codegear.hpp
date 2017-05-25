//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright David Abrahams 2002 - 2003.
//  (C) Copyright Aleksey Gurtovoy 2002.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  CodeGear C++ compiler setup:

#if !defined( AUTOBOOST_WITH_CODEGEAR_WARNINGS )
// these warnings occur frequently in optimized template code
# pragma warn -8004 // var assigned value, but never used
# pragma warn -8008 // condition always true/false
# pragma warn -8066 // dead code can never execute
# pragma warn -8104 // static members with ctors not threadsafe
# pragma warn -8105 // reference member in class without ctors
#endif
//
// versions check:
// last known and checked version is 0x621
#if (__CODEGEARC__ > 0x621)
#  if defined(AUTOBOOST_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  else
#     pragma message( "Unknown compiler version - please run the configure tests and report the results")
#  endif
#endif

// CodeGear C++ Builder 2009
#if (__CODEGEARC__ <= 0x613)
#  define AUTOBOOST_NO_INTEGRAL_INT64_T
#  define AUTOBOOST_NO_DEPENDENT_NESTED_DERIVATIONS
#  define AUTOBOOST_NO_PRIVATE_IN_AGGREGATE
#  define AUTOBOOST_NO_USING_DECLARATION_OVERLOADS_FROM_TYPENAME_BASE
   // we shouldn't really need this - but too many things choke
   // without it, this needs more investigation:
#  define AUTOBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
#  define AUTOBOOST_SP_NO_SP_CONVERTIBLE
#endif

// CodeGear C++ Builder 2010
#if (__CODEGEARC__ <= 0x621)
#  define AUTOBOOST_NO_TYPENAME_WITH_CTOR    // Cannot use typename keyword when making temporaries of a dependant type
#  define AUTOBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#  define AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
#  define AUTOBOOST_NO_NESTED_FRIENDSHIP     // TC1 gives nested classes access rights as any other member
#  define AUTOBOOST_NO_USING_TEMPLATE
#  define AUTOBOOST_NO_TWO_PHASE_NAME_LOOKUP
// Temporary hack, until specific MPL preprocessed headers are generated
#  define AUTOBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS

// CodeGear has not yet completely implemented value-initialization, for
// example for array types, as I reported in 2010: Embarcadero Report 83751,
// "Value-initialization: arrays should have each element value-initialized",
// http://qc.embarcadero.com/wc/qcmain.aspx?d=83751
// Last checked version: Embarcadero C++ 6.21
// See also: http://www.boost.org/libs/utility/value_init.htm#compiler_issues
// (Niels Dekker, LKEB, April 2010)
#  define AUTOBOOST_NO_COMPLETE_VALUE_INITIALIZATION

#  if defined(NDEBUG) && defined(__cplusplus)
      // fix broken <cstring> so that Boost.test works:
#     include <cstring>
#     undef strcmp
#  endif
   // fix broken errno declaration:
#  include <errno.h>
#  ifndef errno
#     define errno errno
#  endif

#endif

// Reportedly, #pragma once is supported since C++ Builder 2010
#if (__CODEGEARC__ >= 0x620)
#  define AUTOBOOST_HAS_PRAGMA_ONCE
#endif

//
// C++0x macros:
//
#if (__CODEGEARC__ <= 0x620)
#define AUTOBOOST_NO_CXX11_STATIC_ASSERT
#else
#define AUTOBOOST_HAS_STATIC_ASSERT
#endif
#define AUTOBOOST_HAS_CHAR16_T
#define AUTOBOOST_HAS_CHAR32_T
#define AUTOBOOST_HAS_LONG_LONG
// #define AUTOBOOST_HAS_ALIGNOF
#define AUTOBOOST_HAS_DECLTYPE
#define AUTOBOOST_HAS_EXPLICIT_CONVERSION_OPS
// #define AUTOBOOST_HAS_RVALUE_REFS
#define AUTOBOOST_HAS_SCOPED_ENUM
// #define AUTOBOOST_HAS_STATIC_ASSERT
#define AUTOBOOST_HAS_STD_TYPE_TRAITS

#define AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS
#define AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#define AUTOBOOST_NO_CXX11_CONSTEXPR
#define AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#define AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS
#define AUTOBOOST_NO_CXX11_EXTERN_TEMPLATE
#define AUTOBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define AUTOBOOST_NO_CXX11_LAMBDAS
#define AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#define AUTOBOOST_NO_CXX11_NOEXCEPT
#define AUTOBOOST_NO_CXX11_NULLPTR
#define AUTOBOOST_NO_CXX11_RANGE_BASED_FOR
#define AUTOBOOST_NO_CXX11_RAW_LITERALS
#define AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#define AUTOBOOST_NO_SFINAE_EXPR
#define AUTOBOOST_NO_CXX11_SFINAE_EXPR
#define AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES
#define AUTOBOOST_NO_CXX11_UNICODE_LITERALS
#define AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
#define AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#define AUTOBOOST_NO_CXX11_USER_DEFINED_LITERALS
#define AUTOBOOST_NO_CXX11_ALIGNAS
#define AUTOBOOST_NO_CXX11_TRAILING_RESULT_TYPES
#define AUTOBOOST_NO_CXX11_INLINE_NAMESPACES
#define AUTOBOOST_NO_CXX11_REF_QUALIFIERS
#define AUTOBOOST_NO_CXX11_FINAL
#define AUTOBOOST_NO_CXX11_THREAD_LOCAL

// C++ 14:
#if !defined(__cpp_aggregate_nsdmi) || (__cpp_aggregate_nsdmi < 201304)
#  define AUTOBOOST_NO_CXX14_AGGREGATE_NSDMI
#endif
#if !defined(__cpp_binary_literals) || (__cpp_binary_literals < 201304)
#  define AUTOBOOST_NO_CXX14_BINARY_LITERALS
#endif
#if !defined(__cpp_constexpr) || (__cpp_constexpr < 201304)
#  define AUTOBOOST_NO_CXX14_CONSTEXPR
#endif
#if !defined(__cpp_decltype_auto) || (__cpp_decltype_auto < 201304)
#  define AUTOBOOST_NO_CXX14_DECLTYPE_AUTO
#endif
#if (__cplusplus < 201304) // There's no SD6 check for this....
#  define AUTOBOOST_NO_CXX14_DIGIT_SEPARATORS
#endif
#if !defined(__cpp_generic_lambdas) || (__cpp_generic_lambdas < 201304)
#  define AUTOBOOST_NO_CXX14_GENERIC_LAMBDAS
#endif
#if !defined(__cpp_init_captures) || (__cpp_init_captures < 201304)
#  define AUTOBOOST_NO_CXX14_INITIALIZED_LAMBDA_CAPTURES
#endif
#if !defined(__cpp_return_type_deduction) || (__cpp_return_type_deduction < 201304)
#  define AUTOBOOST_NO_CXX14_RETURN_TYPE_DEDUCTION
#endif
#if !defined(__cpp_variable_templates) || (__cpp_variable_templates < 201304)
#  define AUTOBOOST_NO_CXX14_VARIABLE_TEMPLATES
#endif

//
// TR1 macros:
//
#define AUTOBOOST_HAS_TR1_HASH
#define AUTOBOOST_HAS_TR1_TYPE_TRAITS
#define AUTOBOOST_HAS_TR1_UNORDERED_MAP
#define AUTOBOOST_HAS_TR1_UNORDERED_SET

#define AUTOBOOST_HAS_MACRO_USE_FACET

#define AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST

// On non-Win32 platforms let the platform config figure this out:
#ifdef _WIN32
#  define AUTOBOOST_HAS_STDINT_H
#endif

//
// __int64:
//
#if !defined(__STRICT_ANSI__)
#  define AUTOBOOST_HAS_MS_INT64
#endif
//
// check for exception handling support:
//
#if !defined(_CPPUNWIND) && !defined(AUTOBOOST_CPPUNWIND) && !defined(__EXCEPTIONS) && !defined(AUTOBOOST_NO_EXCEPTIONS)
#  define AUTOBOOST_NO_EXCEPTIONS
#endif
//
// all versions have a <dirent.h>:
//
#if !defined(__STRICT_ANSI__)
#  define AUTOBOOST_HAS_DIRENT_H
#endif
//
// all versions support __declspec:
//
#if defined(__STRICT_ANSI__)
// config/platform/win32.hpp will define AUTOBOOST_SYMBOL_EXPORT, etc., unless already defined
#  define AUTOBOOST_SYMBOL_EXPORT
#endif
//
// ABI fixing headers:
//
#ifndef AUTOBOOST_ABI_PREFIX
#  define AUTOBOOST_ABI_PREFIX "autoboost/config/abi/borland_prefix.hpp"
#endif
#ifndef AUTOBOOST_ABI_SUFFIX
#  define AUTOBOOST_ABI_SUFFIX "autoboost/config/abi/borland_suffix.hpp"
#endif
//
// Disable Win32 support in ANSI mode:
//
#  pragma defineonoption AUTOBOOST_DISABLE_WIN32 -A
//
// MSVC compatibility mode does some nasty things:
// TODO: look up if this doesn't apply to the whole 12xx range
//
#if defined(_MSC_VER) && (_MSC_VER <= 1200)
#  define AUTOBOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
#  define AUTOBOOST_NO_VOID_RETURNS
#endif

#define AUTOBOOST_COMPILER "CodeGear C++ version " AUTOBOOST_STRINGIZE(__CODEGEARC__)

