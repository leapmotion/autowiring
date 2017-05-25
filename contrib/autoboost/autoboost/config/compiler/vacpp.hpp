//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Toon Knapen 2001 - 2003.
//  (C) Copyright Lie-Quan Lee 2001.
//  (C) Copyright Markus Schoepflin 2002 - 2003.
//  (C) Copyright Beman Dawes 2002 - 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Visual Age (IBM) C++ compiler setup:

#if __IBMCPP__ <= 501
#  define AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
#  define AUTOBOOST_NO_MEMBER_FUNCTION_SPECIALIZATIONS
#endif

#if (__IBMCPP__ <= 502)
// Actually the compiler supports inclass member initialization but it
// requires a definition for the class member and it doesn't recognize
// it as an integral constant expression when used as a template argument.
#  define AUTOBOOST_NO_INCLASS_MEMBER_INITIALIZATION
#  define AUTOBOOST_NO_INTEGRAL_INT64_T
#  define AUTOBOOST_NO_MEMBER_TEMPLATE_KEYWORD
#endif

#if (__IBMCPP__ <= 600) || !defined(AUTOBOOST_STRICT_CONFIG)
#  define AUTOBOOST_NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
#endif

#if (__IBMCPP__ <= 1110)
// XL C++ V11.1 and earlier versions may not always value-initialize
// a temporary object T(), when T is a non-POD aggregate class type.
// Michael Wong (IBM Canada Ltd) has confirmed this issue and gave it
// high priority. -- Niels Dekker (LKEB), May 2010.
#  define AUTOBOOST_NO_COMPLETE_VALUE_INITIALIZATION
#endif

//
// On AIX thread support seems to be indicated by _THREAD_SAFE:
//
#ifdef _THREAD_SAFE
#  define AUTOBOOST_HAS_THREADS
#endif

#define AUTOBOOST_COMPILER "IBM Visual Age version " AUTOBOOST_STRINGIZE(__IBMCPP__)

//
// versions check:
// we don't support Visual age prior to version 5:
#if __IBMCPP__ < 500
#error "Compiler not supported or configured - please reconfigure"
#endif
//
// last known and checked version is 1210:
#if (__IBMCPP__ > 1210)
#  if defined(AUTOBOOST_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  endif
#endif

// Some versions of the compiler have issues with default arguments on partial specializations
#if __IBMCPP__ <= 1010
#define AUTOBOOST_NO_PARTIAL_SPECIALIZATION_IMPLICIT_DEFAULT_ARGS
#endif

//
// C++0x features
//
//   See autoboost\config\suffix.hpp for AUTOBOOST_NO_LONG_LONG
//
#if ! __IBMCPP_AUTO_TYPEDEDUCTION
#  define AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS
#  define AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#endif
#if ! __IBMCPP_UTF_LITERAL__
#  define AUTOBOOST_NO_CXX11_CHAR16_T
#  define AUTOBOOST_NO_CXX11_CHAR32_T
#endif
#if ! __IBMCPP_CONSTEXPR
#  define AUTOBOOST_NO_CXX11_CONSTEXPR
#endif
#if ! __IBMCPP_DECLTYPE
#  define AUTOBOOST_NO_CXX11_DECLTYPE
#else
#  define AUTOBOOST_HAS_DECLTYPE
#endif
#define AUTOBOOST_NO_CXX11_DECLTYPE_N3276
#define AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#define AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS
#if ! __IBMCPP_EXPLICIT_CONVERSION_OPERATORS
#  define AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#endif
#if ! __IBMCPP_EXTERN_TEMPLATE
#  define AUTOBOOST_NO_CXX11_EXTERN_TEMPLATE
#endif
#if ! __IBMCPP_VARIADIC_TEMPLATES
// not enabled separately at this time
#  define AUTOBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#endif
#define AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#define AUTOBOOST_NO_CXX11_LAMBDAS
#define AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#define AUTOBOOST_NO_CXX11_NOEXCEPT
#define AUTOBOOST_NO_CXX11_NULLPTR
#define AUTOBOOST_NO_CXX11_RANGE_BASED_FOR
#define AUTOBOOST_NO_CXX11_RAW_LITERALS
#define AUTOBOOST_NO_CXX11_USER_DEFINED_LITERALS
#if ! __IBMCPP_RVALUE_REFERENCES
#  define AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#endif
#if ! __IBMCPP_SCOPED_ENUM
#  define AUTOBOOST_NO_CXX11_SCOPED_ENUMS
#endif
#define AUTOBOOST_NO_SFINAE_EXPR
#define AUTOBOOST_NO_CXX11_SFINAE_EXPR
#define AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#if ! __IBMCPP_STATIC_ASSERT
#  define AUTOBOOST_NO_CXX11_STATIC_ASSERT
#endif
#define AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES
#define AUTOBOOST_NO_CXX11_UNICODE_LITERALS
#if ! __IBMCPP_VARIADIC_TEMPLATES
#  define AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
#endif
#if ! __C99_MACRO_WITH_VA_ARGS
#  define AUTOBOOST_NO_CXX11_VARIADIC_MACROS
#endif
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
