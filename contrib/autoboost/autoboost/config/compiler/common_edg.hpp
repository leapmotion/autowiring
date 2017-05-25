//  (C) Copyright John Maddock 2001 - 2002.
//  (C) Copyright Jens Maurer 2001.
//  (C) Copyright David Abrahams 2002.
//  (C) Copyright Aleksey Gurtovoy 2002.
//  (C) Copyright Markus Schoepflin 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//
// Options common to all edg based compilers.
//
// This is included from within the individual compiler mini-configs.

#ifndef  __EDG_VERSION__
#  error This file requires that __EDG_VERSION__ be defined.
#endif

#if (__EDG_VERSION__ <= 238)
#   define AUTOBOOST_NO_INTEGRAL_INT64_T
#   define AUTOBOOST_NO_SFINAE
#endif

#if (__EDG_VERSION__ <= 240)
#   define AUTOBOOST_NO_VOID_RETURNS
#endif

#if (__EDG_VERSION__ <= 241) && !defined(AUTOBOOST_NO_ARGUMENT_DEPENDENT_LOOKUP)
#   define AUTOBOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
#endif

#if (__EDG_VERSION__ <= 244) && !defined(AUTOBOOST_NO_TEMPLATE_TEMPLATES)
#   define AUTOBOOST_NO_TEMPLATE_TEMPLATES
#endif

#if (__EDG_VERSION__ < 300) && !defined(AUTOBOOST_NO_IS_ABSTRACT)
#   define AUTOBOOST_NO_IS_ABSTRACT
#endif

#if (__EDG_VERSION__ <= 303) && !defined(AUTOBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
#   define AUTOBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#endif

// See also kai.hpp which checks a Kai-specific symbol for EH
# if !defined(__KCC) && !defined(__EXCEPTIONS) && !defined(AUTOBOOST_NO_EXCEPTIONS)
#     define AUTOBOOST_NO_EXCEPTIONS
# endif

# if !defined(__NO_LONG_LONG)
#     define AUTOBOOST_HAS_LONG_LONG
# else
#     define AUTOBOOST_NO_LONG_LONG
# endif

// Not sure what version was the first to support #pragma once, but
// different EDG-based compilers (e.g. Intel) supported it for ages.
// Add a proper version check if it causes problems.
#define AUTOBOOST_HAS_PRAGMA_ONCE

//
// C++0x features
//
//   See above for AUTOBOOST_NO_LONG_LONG
//
#if (__EDG_VERSION__ < 310)
#  define AUTOBOOST_NO_CXX11_EXTERN_TEMPLATE
#endif
#if (__EDG_VERSION__ <= 310)
// No support for initializer lists
#  define AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#endif
#if (__EDG_VERSION__ < 400)
#  define AUTOBOOST_NO_CXX11_VARIADIC_MACROS
#endif

#define AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS
#define AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#define AUTOBOOST_NO_CXX11_CHAR16_T
#define AUTOBOOST_NO_CXX11_CHAR32_T
#define AUTOBOOST_NO_CXX11_CONSTEXPR
#define AUTOBOOST_NO_CXX11_DECLTYPE
#define AUTOBOOST_NO_CXX11_DECLTYPE_N3276
#define AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#define AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS
#define AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#define AUTOBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define AUTOBOOST_NO_CXX11_LAMBDAS
#define AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#define AUTOBOOST_NO_CXX11_NOEXCEPT
#define AUTOBOOST_NO_CXX11_NULLPTR
#define AUTOBOOST_NO_CXX11_RANGE_BASED_FOR
#define AUTOBOOST_NO_CXX11_RAW_LITERALS
#define AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#define AUTOBOOST_NO_CXX11_SCOPED_ENUMS
#define AUTOBOOST_NO_SFINAE_EXPR
#define AUTOBOOST_NO_CXX11_SFINAE_EXPR
#define AUTOBOOST_NO_CXX11_STATIC_ASSERT
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

#ifdef c_plusplus
// EDG has "long long" in non-strict mode
// However, some libraries have insufficient "long long" support
// #define AUTOBOOST_HAS_LONG_LONG
#endif
