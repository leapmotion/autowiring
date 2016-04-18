//  (C) Copyright John Maddock 2001.
//  (C) Copyright Darin Adler 2001.
//  (C) Copyright Peter Dimov 2001.
//  (C) Copyright David Abrahams 2001 - 2002.
//  (C) Copyright Beman Dawes 2001 - 2003.
//  (C) Copyright Stefan Slapeta 2004.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Metrowerks C++ compiler setup:

// locale support is disabled when linking with the dynamic runtime
#   ifdef _MSL_NO_LOCALE
#     define AUTOBOOST_NO_STD_LOCALE
#   endif

#   if __MWERKS__ <= 0x2301  // 5.3
#     define AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING
#     define AUTOBOOST_NO_POINTER_TO_MEMBER_CONST
#     define AUTOBOOST_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS
#     define AUTOBOOST_NO_MEMBER_TEMPLATE_KEYWORD
#   endif

#   if __MWERKS__ <= 0x2401  // 6.2
//#     define AUTOBOOST_NO_FUNCTION_TEMPLATE_ORDERING
#   endif

#   if(__MWERKS__ <= 0x2407)  // 7.x
#     define AUTOBOOST_NO_MEMBER_FUNCTION_SPECIALIZATIONS
#     define AUTOBOOST_NO_UNREACHABLE_RETURN_DETECTION
#   endif

#   if(__MWERKS__ <= 0x3003)  // 8.x
#     define AUTOBOOST_NO_SFINAE
#    endif

// the "|| !defined(AUTOBOOST_STRICT_CONFIG)" part should apply to the last
// tested version *only*:
#   if(__MWERKS__ <= 0x3207) || !defined(AUTOBOOST_STRICT_CONFIG) // 9.6
#     define AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
#     define AUTOBOOST_NO_IS_ABSTRACT
#    endif

#if !__option(wchar_type)
#   define AUTOBOOST_NO_INTRINSIC_WCHAR_T
#endif

#if !__option(exceptions) && !defined(AUTOBOOST_NO_EXCEPTIONS)
#   define AUTOBOOST_NO_EXCEPTIONS
#endif

#if (__INTEL__ && _WIN32) || (__POWERPC__ && macintosh)
#   if __MWERKS__ == 0x3000
#     define AUTOBOOST_COMPILER_VERSION 8.0
#   elif __MWERKS__ == 0x3001
#     define AUTOBOOST_COMPILER_VERSION 8.1
#   elif __MWERKS__ == 0x3002
#     define AUTOBOOST_COMPILER_VERSION 8.2
#   elif __MWERKS__ == 0x3003
#     define AUTOBOOST_COMPILER_VERSION 8.3
#   elif __MWERKS__ == 0x3200
#     define AUTOBOOST_COMPILER_VERSION 9.0
#   elif __MWERKS__ == 0x3201
#     define AUTOBOOST_COMPILER_VERSION 9.1
#   elif __MWERKS__ == 0x3202
#     define AUTOBOOST_COMPILER_VERSION 9.2
#   elif __MWERKS__ == 0x3204
#     define AUTOBOOST_COMPILER_VERSION 9.3
#   elif __MWERKS__ == 0x3205
#     define AUTOBOOST_COMPILER_VERSION 9.4
#   elif __MWERKS__ == 0x3206
#     define AUTOBOOST_COMPILER_VERSION 9.5
#   elif __MWERKS__ == 0x3207
#     define AUTOBOOST_COMPILER_VERSION 9.6
#   else
#     define AUTOBOOST_COMPILER_VERSION __MWERKS__
#   endif
#else
#  define AUTOBOOST_COMPILER_VERSION __MWERKS__
#endif

//
// C++0x features
//
//   See autoboost\config\suffix.hpp for AUTOBOOST_NO_LONG_LONG
//
#if __MWERKS__ > 0x3206 && __option(rvalue_refs)
#  define AUTOBOOST_HAS_RVALUE_REFS
#else
#  define AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
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
#define AUTOBOOST_NO_CXX11_EXTERN_TEMPLATE
#define AUTOBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#define AUTOBOOST_NO_CXX11_LAMBDAS
#define AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#define AUTOBOOST_NO_CXX11_NOEXCEPT
#define AUTOBOOST_NO_CXX11_NULLPTR
#define AUTOBOOST_NO_CXX11_RANGE_BASED_FOR
#define AUTOBOOST_NO_CXX11_RAW_LITERALS
#define AUTOBOOST_NO_CXX11_SCOPED_ENUMS
#define AUTOBOOST_NO_SFINAE_EXPR
#define AUTOBOOST_NO_CXX11_STATIC_ASSERT
#define AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES
#define AUTOBOOST_NO_CXX11_UNICODE_LITERALS
#define AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
#define AUTOBOOST_NO_CXX11_VARIADIC_MACROS
#define AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#define AUTOBOOST_NO_CXX11_USER_DEFINED_LITERALS
#define AUTOBOOST_NO_CXX11_ALIGNAS
#define AUTOBOOST_NO_CXX11_TRAILING_RESULT_TYPES
#define AUTOBOOST_NO_CXX11_INLINE_NAMESPACES
#define AUTOBOOST_NO_CXX11_REF_QUALIFIERS
#define AUTOBOOST_NO_CXX11_FINAL

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

#define AUTOBOOST_COMPILER "Metrowerks CodeWarrior C++ version " AUTOBOOST_STRINGIZE(AUTOBOOST_COMPILER_VERSION)

//
// versions check:
// we don't support Metrowerks prior to version 5.3:
#if __MWERKS__ < 0x2301
#  error "Compiler not supported or configured - please reconfigure"
#endif
//
// last known and checked version:
#if (__MWERKS__ > 0x3205)
#  if defined(AUTOBOOST_ASSERT_CONFIG)
#     error "Unknown compiler version - please run the configure tests and report the results"
#  endif
#endif







