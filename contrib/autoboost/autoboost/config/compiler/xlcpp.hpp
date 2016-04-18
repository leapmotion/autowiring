// (C) Copyright Douglas Gregor 2010
//
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  compiler setup for IBM XL C/C++ for Linux (Little Endian) based on clang.

#define AUTOBOOST_HAS_PRAGMA_ONCE

// Detecting `-fms-extension` compiler flag assuming that _MSC_VER defined when that flag is used.
#if defined (_MSC_VER) && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 4))
#   define AUTOBOOST_HAS_PRAGMA_DETECT_MISMATCH
#endif

// When compiling with clang before __has_extension was defined,
// even if one writes 'defined(__has_extension) && __has_extension(xxx)',
// clang reports a compiler error. So the only workaround found is:

#ifndef __has_extension
#define __has_extension __has_feature
#endif

#if !__has_feature(cxx_exceptions) && !defined(AUTOBOOST_NO_EXCEPTIONS)
#  define AUTOBOOST_NO_EXCEPTIONS
#endif

#if !__has_feature(cxx_rtti) && !defined(AUTOBOOST_NO_RTTI)
#  define AUTOBOOST_NO_RTTI
#endif

#if !__has_feature(cxx_rtti) && !defined(AUTOBOOST_NO_TYPEID)
#  define AUTOBOOST_NO_TYPEID
#endif

#if defined(__int64) && !defined(__GNUC__)
#  define AUTOBOOST_HAS_MS_INT64
#endif

#define AUTOBOOST_HAS_NRVO

// Branch prediction hints
#if defined(__has_builtin)
#if __has_builtin(__builtin_expect)
#define AUTOBOOST_LIKELY(x) __builtin_expect(x, 1)
#define AUTOBOOST_UNLIKELY(x) __builtin_expect(x, 0)
#endif
#endif

// Clang supports "long long" in all compilation modes.
#define AUTOBOOST_HAS_LONG_LONG

//
// Dynamic shared object (DSO) and dynamic-link library (DLL) support
//
#if !defined(_WIN32) && !defined(__WIN32__) && !defined(WIN32)
#  define AUTOBOOST_SYMBOL_EXPORT __attribute__((__visibility__("default")))
#  define AUTOBOOST_SYMBOL_IMPORT
#  define AUTOBOOST_SYMBOL_VISIBLE __attribute__((__visibility__("default")))
#endif

//
// The AUTOBOOST_FALLTHROUGH macro can be used to annotate implicit fall-through
// between switch labels.
//
#if __cplusplus >= 201103L && defined(__has_warning)
#  if __has_feature(cxx_attributes) && __has_warning("-Wimplicit-fallthrough")
#    define AUTOBOOST_FALLTHROUGH [[clang::fallthrough]]
#  endif
#endif

#if !__has_feature(cxx_auto_type)
#  define AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS
#  define AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#endif

//
// Currently clang on Windows using VC++ RTL does not support C++11's char16_t or char32_t
//
#if defined(_MSC_VER) || !(defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L)
#  define AUTOBOOST_NO_CXX11_CHAR16_T
#  define AUTOBOOST_NO_CXX11_CHAR32_T
#endif

#if !__has_feature(cxx_constexpr)
#  define AUTOBOOST_NO_CXX11_CONSTEXPR
#endif

#if !__has_feature(cxx_decltype)
#  define AUTOBOOST_NO_CXX11_DECLTYPE
#endif

#if !__has_feature(cxx_decltype_incomplete_return_types)
#  define AUTOBOOST_NO_CXX11_DECLTYPE_N3276
#endif

#if !__has_feature(cxx_defaulted_functions)
#  define AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#endif

#if !__has_feature(cxx_deleted_functions)
#  define AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS
#endif

#if !__has_feature(cxx_explicit_conversions)
#  define AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#endif

#if !__has_feature(cxx_default_function_template_args)
#  define AUTOBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#endif

#if !__has_feature(cxx_generalized_initializers)
#  define AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#endif

#if !__has_feature(cxx_lambdas)
#  define AUTOBOOST_NO_CXX11_LAMBDAS
#endif

#if !__has_feature(cxx_local_type_template_args)
#  define AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#endif

#if !__has_feature(cxx_noexcept)
#  define AUTOBOOST_NO_CXX11_NOEXCEPT
#endif

#if !__has_feature(cxx_nullptr)
#  define AUTOBOOST_NO_CXX11_NULLPTR
#endif

#if !__has_feature(cxx_range_for)
#  define AUTOBOOST_NO_CXX11_RANGE_BASED_FOR
#endif

#if !__has_feature(cxx_raw_string_literals)
#  define AUTOBOOST_NO_CXX11_RAW_LITERALS
#endif

#if !__has_feature(cxx_reference_qualified_functions)
#  define AUTOBOOST_NO_CXX11_REF_QUALIFIERS
#endif

#if !__has_feature(cxx_generalized_initializers)
#  define AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#endif

#if !__has_feature(cxx_rvalue_references)
#  define AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#endif

#if !__has_feature(cxx_strong_enums)
#  define AUTOBOOST_NO_CXX11_SCOPED_ENUMS
#endif

#if !__has_feature(cxx_static_assert)
#  define AUTOBOOST_NO_CXX11_STATIC_ASSERT
#endif

#if !__has_feature(cxx_alias_templates)
#  define AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES
#endif

#if !__has_feature(cxx_unicode_literals)
#  define AUTOBOOST_NO_CXX11_UNICODE_LITERALS
#endif

#if !__has_feature(cxx_variadic_templates)
#  define AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
#endif

#if !__has_feature(cxx_user_literals)
#  define AUTOBOOST_NO_CXX11_USER_DEFINED_LITERALS
#endif

#if !__has_feature(cxx_alignas)
#  define AUTOBOOST_NO_CXX11_ALIGNAS
#endif

#if !__has_feature(cxx_trailing_return)
#  define AUTOBOOST_NO_CXX11_TRAILING_RESULT_TYPES
#endif

#if !__has_feature(cxx_inline_namespaces)
#  define AUTOBOOST_NO_CXX11_INLINE_NAMESPACES
#endif

#if !__has_feature(cxx_override_control)
#  define AUTOBOOST_NO_CXX11_FINAL
#endif

#if !(__has_feature(__cxx_binary_literals__) || __has_extension(__cxx_binary_literals__))
#  define AUTOBOOST_NO_CXX14_BINARY_LITERALS
#endif

#if !__has_feature(__cxx_decltype_auto__)
#  define AUTOBOOST_NO_CXX14_DECLTYPE_AUTO
#endif

#if !__has_feature(__cxx_aggregate_nsdmi__)
#  define AUTOBOOST_NO_CXX14_AGGREGATE_NSDMI
#endif

#if !__has_feature(__cxx_init_captures__)
#  define AUTOBOOST_NO_CXX14_INITIALIZED_LAMBDA_CAPTURES
#endif

#if !__has_feature(__cxx_generic_lambdas__)
#  define AUTOBOOST_NO_CXX14_GENERIC_LAMBDAS
#endif

// clang < 3.5 has a defect with dependent type, like following.
//
//  template <class T>
//  constexpr typename enable_if<pred<T> >::type foo(T &)
//  { } // error: no return statement in constexpr function
//
// This issue also affects C++11 mode, but C++11 constexpr requires return stmt.
// Therefore we don't care such case.
//
// Note that we can't check Clang version directly as the numbering system changes depending who's
// creating the Clang release (see https://github.com/autoboostorg/config/pull/39#issuecomment-59927873)
// so instead verify that we have a feature that was introduced at the same time as working C++14
// constexpr (generic lambda's in this case):
//
#if !__has_feature(__cxx_generic_lambdas__) || !__has_feature(__cxx_relaxed_constexpr__)
#  define AUTOBOOST_NO_CXX14_CONSTEXPR
#endif

#if !__has_feature(__cxx_return_type_deduction__)
#  define AUTOBOOST_NO_CXX14_RETURN_TYPE_DEDUCTION
#endif

#if !__has_feature(__cxx_variable_templates__)
#  define AUTOBOOST_NO_CXX14_VARIABLE_TEMPLATES
#endif

#if __cplusplus < 201400
// All versions with __cplusplus above this value seem to support this:
#  define AUTOBOOST_NO_CXX14_DIGIT_SEPARATORS
#endif


// Unused attribute:
#if defined(__GNUC__) && (__GNUC__ >= 4)
#  define AUTOBOOST_ATTRIBUTE_UNUSED __attribute__((unused))
#endif

#ifndef AUTOBOOST_COMPILER
#  define AUTOBOOST_COMPILER "Clang version " __clang_version__
#endif

// Macro used to identify the Clang compiler.
#define AUTOBOOST_CLANG 1

