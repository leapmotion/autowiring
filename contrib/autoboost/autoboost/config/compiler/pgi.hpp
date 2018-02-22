//  (C) Copyright Noel Belcourt 2007.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  PGI C++ compiler setup:

#define AUTOBOOST_COMPILER_VERSION __PGIC__##__PGIC_MINOR__
#define AUTOBOOST_COMPILER "PGI compiler version " AUTOBOOST_STRINGIZE(AUTOBOOST_COMPILER_VERSION)

//
// Threading support:
// Turn this on unconditionally here, it will get turned off again later
// if no threading API is detected.
//

#if __PGIC__ >= 11

// options requested by configure --enable-test
#define AUTOBOOST_HAS_PTHREADS
#define AUTOBOOST_HAS_THREADS
#define AUTOBOOST_HAS_PTHREAD_YIELD
#define AUTOBOOST_HAS_NRVO
#define AUTOBOOST_HAS_LONG_LONG

// options --enable-test wants undefined
#undef AUTOBOOST_NO_STDC_NAMESPACE
#undef AUTOBOOST_NO_EXCEPTION_STD_NAMESPACE
#undef AUTOBOOST_DEDUCED_TYPENAME

#define AUTOBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#define AUTOBOOST_NO_TWO_PHASE_NAME_LOOKUP
#define AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#define AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS

#elif __PGIC__ >= 10

// options requested by configure --enable-test
#define AUTOBOOST_HAS_THREADS
#define AUTOBOOST_HAS_NRVO
#define AUTOBOOST_HAS_LONG_LONG
#if defined(linux) || defined(__linux) || defined(__linux__)
#  define AUTOBOOST_HAS_STDINT_H
#endif

// options --enable-test wants undefined
#undef AUTOBOOST_NO_STDC_NAMESPACE
#undef AUTOBOOST_NO_EXCEPTION_STD_NAMESPACE
#undef AUTOBOOST_DEDUCED_TYPENAME

#elif __PGIC__ >= 7

#define AUTOBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#define AUTOBOOST_NO_TWO_PHASE_NAME_LOOKUP
#define AUTOBOOST_NO_SWPRINTF
#define AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#define AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS

#else

#  error "Pgi compiler not configured - please reconfigure"

#endif
//
// C++0x features
//
//   See autoboost\config\suffix.hpp for AUTOBOOST_NO_LONG_LONG
//
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
#define AUTOBOOST_NO_CXX11_LAMBDAS
#define AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#define AUTOBOOST_NO_CXX11_NOEXCEPT
#define AUTOBOOST_NO_CXX11_NULLPTR
#define AUTOBOOST_NO_CXX11_NUMERIC_LIMITS
#define AUTOBOOST_NO_CXX11_RANGE_BASED_FOR
#define AUTOBOOST_NO_CXX11_RAW_LITERALS
#define AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#define AUTOBOOST_NO_CXX11_SCOPED_ENUMS
#define AUTOBOOST_NO_SFINAE_EXPR
#define AUTOBOOST_NO_CXX11_SFINAE_EXPR
#define AUTOBOOST_NO_CXX11_STATIC_ASSERT
#define AUTOBOOST_NO_SWPRINTF
#define AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES
#define AUTOBOOST_NO_CXX11_UNICODE_LITERALS
#define AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
#define AUTOBOOST_NO_CXX11_VARIADIC_MACROS
#define AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX

#define AUTOBOOST_NO_CXX11_HDR_UNORDERED_SET
#define AUTOBOOST_NO_CXX11_HDR_UNORDERED_MAP
#define AUTOBOOST_NO_CXX11_HDR_TYPEINDEX
#define AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS
#define AUTOBOOST_NO_CXX11_HDR_TUPLE
#define AUTOBOOST_NO_CXX11_HDR_THREAD
#define AUTOBOOST_NO_CXX11_HDR_SYSTEM_ERROR
#define AUTOBOOST_NO_CXX11_HDR_REGEX
#define AUTOBOOST_NO_CXX11_HDR_RATIO
#define AUTOBOOST_NO_CXX11_HDR_RANDOM
#define AUTOBOOST_NO_CXX11_HDR_MUTEX
#define AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#define AUTOBOOST_NO_CXX11_HDR_FUTURE
#define AUTOBOOST_NO_CXX11_HDR_FORWARD_LIST
#define AUTOBOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#define AUTOBOOST_NO_CXX11_HDR_CODECVT
#define AUTOBOOST_NO_CXX11_HDR_CHRONO
#define AUTOBOOST_NO_CXX11_HDR_ARRAY
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
// version check:
// probably nothing to do here?

