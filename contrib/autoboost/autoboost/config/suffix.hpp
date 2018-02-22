//  Boost config.hpp configuration header file  ------------------------------//
//  autoboostinspect:ndprecated_macros -- tell the inspect tool to ignore this file

//  Copyright (c) 2001-2003 John Maddock
//  Copyright (c) 2001 Darin Adler
//  Copyright (c) 2001 Peter Dimov
//  Copyright (c) 2002 Bill Kempf
//  Copyright (c) 2002 Jens Maurer
//  Copyright (c) 2002-2003 David Abrahams
//  Copyright (c) 2003 Gennaro Prota
//  Copyright (c) 2003 Eric Friedman
//  Copyright (c) 2010 Eric Jourdanneau, Joel Falcou
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for most recent version.

//  Boost config.hpp policy and rationale documentation has been moved to
//  http://www.boost.org/libs/config/
//
//  This file is intended to be stable, and relatively unchanging.
//  It should contain boilerplate code only - no compiler specific
//  code unless it is unavoidable - no changes unless unavoidable.

#ifndef AUTOBOOST_CONFIG_SUFFIX_HPP
#define AUTOBOOST_CONFIG_SUFFIX_HPP

#if defined(__GNUC__) && (__GNUC__ >= 4)
//
// Some GCC-4.x versions issue warnings even when __extension__ is used,
// so use this as a workaround:
//
#pragma GCC system_header
#endif

//
// ensure that visibility macros are always defined, thus symplifying use
//
#ifndef AUTOBOOST_SYMBOL_EXPORT
# define AUTOBOOST_SYMBOL_EXPORT
#endif
#ifndef AUTOBOOST_SYMBOL_IMPORT
# define AUTOBOOST_SYMBOL_IMPORT
#endif
#ifndef AUTOBOOST_SYMBOL_VISIBLE
# define AUTOBOOST_SYMBOL_VISIBLE
#endif

//
// look for long long by looking for the appropriate macros in <limits.h>.
// Note that we use limits.h rather than climits for maximal portability,
// remember that since these just declare a bunch of macros, there should be
// no namespace issues from this.
//
#if !defined(AUTOBOOST_HAS_LONG_LONG) && !defined(AUTOBOOST_NO_LONG_LONG)                                              \
   && !defined(AUTOBOOST_MSVC) && !defined(__BORLANDC__)
# include <limits.h>
# if (defined(ULLONG_MAX) || defined(ULONG_LONG_MAX) || defined(ULONGLONG_MAX))
#   define AUTOBOOST_HAS_LONG_LONG
# else
#   define AUTOBOOST_NO_LONG_LONG
# endif
#endif

// GCC 3.x will clean up all of those nasty macro definitions that
// AUTOBOOST_NO_CTYPE_FUNCTIONS is intended to help work around, so undefine
// it under GCC 3.x.
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(AUTOBOOST_NO_CTYPE_FUNCTIONS)
#  undef AUTOBOOST_NO_CTYPE_FUNCTIONS
#endif

//
// Assume any extensions are in namespace std:: unless stated otherwise:
//
#  ifndef AUTOBOOST_STD_EXTENSION_NAMESPACE
#    define AUTOBOOST_STD_EXTENSION_NAMESPACE std
#  endif

//
// If cv-qualified specializations are not allowed, then neither are cv-void ones:
//
#  if defined(AUTOBOOST_NO_CV_SPECIALIZATIONS) \
      && !defined(AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS)
#     define AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
#  endif

//
// If there is no numeric_limits template, then it can't have any compile time
// constants either!
//
#  if defined(AUTOBOOST_NO_LIMITS) \
      && !defined(AUTOBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS)
#     define AUTOBOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
#     define AUTOBOOST_NO_MS_INT64_NUMERIC_LIMITS
#     define AUTOBOOST_NO_LONG_LONG_NUMERIC_LIMITS
#  endif

//
// if there is no long long then there is no specialisation
// for numeric_limits<long long> either:
//
#if !defined(AUTOBOOST_HAS_LONG_LONG) && !defined(AUTOBOOST_NO_LONG_LONG_NUMERIC_LIMITS)
#  define AUTOBOOST_NO_LONG_LONG_NUMERIC_LIMITS
#endif

//
// if there is no __int64 then there is no specialisation
// for numeric_limits<__int64> either:
//
#if !defined(AUTOBOOST_HAS_MS_INT64) && !defined(AUTOBOOST_NO_MS_INT64_NUMERIC_LIMITS)
#  define AUTOBOOST_NO_MS_INT64_NUMERIC_LIMITS
#endif

//
// if member templates are supported then so is the
// VC6 subset of member templates:
//
#  if !defined(AUTOBOOST_NO_MEMBER_TEMPLATES) \
       && !defined(AUTOBOOST_MSVC6_MEMBER_TEMPLATES)
#     define AUTOBOOST_MSVC6_MEMBER_TEMPLATES
#  endif

//
// Without partial specialization, can't test for partial specialisation bugs:
//
#  if defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(AUTOBOOST_BCB_PARTIAL_SPECIALIZATION_BUG)
#     define AUTOBOOST_BCB_PARTIAL_SPECIALIZATION_BUG
#  endif

//
// Without partial specialization, we can't have array-type partial specialisations:
//
#  if defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(AUTOBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
#     define AUTOBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS
#  endif

//
// Without partial specialization, std::iterator_traits can't work:
//
#  if defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(AUTOBOOST_NO_STD_ITERATOR_TRAITS)
#     define AUTOBOOST_NO_STD_ITERATOR_TRAITS
#  endif

//
// Without partial specialization, partial
// specialization with default args won't work either:
//
#  if defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(AUTOBOOST_NO_PARTIAL_SPECIALIZATION_IMPLICIT_DEFAULT_ARGS)
#     define AUTOBOOST_NO_PARTIAL_SPECIALIZATION_IMPLICIT_DEFAULT_ARGS
#  endif

//
// Without member template support, we can't have template constructors
// in the standard library either:
//
#  if defined(AUTOBOOST_NO_MEMBER_TEMPLATES) \
      && !defined(AUTOBOOST_MSVC6_MEMBER_TEMPLATES) \
      && !defined(AUTOBOOST_NO_TEMPLATED_ITERATOR_CONSTRUCTORS)
#     define AUTOBOOST_NO_TEMPLATED_ITERATOR_CONSTRUCTORS
#  endif

//
// Without member template support, we can't have a conforming
// std::allocator template either:
//
#  if defined(AUTOBOOST_NO_MEMBER_TEMPLATES) \
      && !defined(AUTOBOOST_MSVC6_MEMBER_TEMPLATES) \
      && !defined(AUTOBOOST_NO_STD_ALLOCATOR)
#     define AUTOBOOST_NO_STD_ALLOCATOR
#  endif

//
// without ADL support then using declarations will break ADL as well:
//
#if defined(AUTOBOOST_NO_ARGUMENT_DEPENDENT_LOOKUP) && !defined(AUTOBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
#  define AUTOBOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#endif

//
// Without typeid support we have no dynamic RTTI either:
//
#if defined(AUTOBOOST_NO_TYPEID) && !defined(AUTOBOOST_NO_RTTI)
#  define AUTOBOOST_NO_RTTI
#endif

//
// If we have a standard allocator, then we have a partial one as well:
//
#if !defined(AUTOBOOST_NO_STD_ALLOCATOR)
#  define AUTOBOOST_HAS_PARTIAL_STD_ALLOCATOR
#endif

//
// We can't have a working std::use_facet if there is no std::locale:
//
#  if defined(AUTOBOOST_NO_STD_LOCALE) && !defined(AUTOBOOST_NO_STD_USE_FACET)
#     define AUTOBOOST_NO_STD_USE_FACET
#  endif

//
// We can't have a std::messages facet if there is no std::locale:
//
#  if defined(AUTOBOOST_NO_STD_LOCALE) && !defined(AUTOBOOST_NO_STD_MESSAGES)
#     define AUTOBOOST_NO_STD_MESSAGES
#  endif

//
// We can't have a working std::wstreambuf if there is no std::locale:
//
#  if defined(AUTOBOOST_NO_STD_LOCALE) && !defined(AUTOBOOST_NO_STD_WSTREAMBUF)
#     define AUTOBOOST_NO_STD_WSTREAMBUF
#  endif

//
// We can't have a <cwctype> if there is no <cwchar>:
//
#  if defined(AUTOBOOST_NO_CWCHAR) && !defined(AUTOBOOST_NO_CWCTYPE)
#     define AUTOBOOST_NO_CWCTYPE
#  endif

//
// We can't have a swprintf if there is no <cwchar>:
//
#  if defined(AUTOBOOST_NO_CWCHAR) && !defined(AUTOBOOST_NO_SWPRINTF)
#     define AUTOBOOST_NO_SWPRINTF
#  endif

//
// If Win32 support is turned off, then we must turn off
// threading support also, unless there is some other
// thread API enabled:
//
#if defined(AUTOBOOST_DISABLE_WIN32) && defined(_WIN32) \
   && !defined(AUTOBOOST_DISABLE_THREADS) && !defined(AUTOBOOST_HAS_PTHREADS)
#  define AUTOBOOST_DISABLE_THREADS
#endif

//
// Turn on threading support if the compiler thinks that it's in
// multithreaded mode.  We put this here because there are only a
// limited number of macros that identify this (if there's any missing
// from here then add to the appropriate compiler section):
//
#if (defined(__MT__) || defined(_MT) || defined(_REENTRANT) \
    || defined(_PTHREADS) || defined(__APPLE__) || defined(__DragonFly__)) \
    && !defined(AUTOBOOST_HAS_THREADS)
#  define AUTOBOOST_HAS_THREADS
#endif

//
// Turn threading support off if AUTOBOOST_DISABLE_THREADS is defined:
//
#if defined(AUTOBOOST_DISABLE_THREADS) && defined(AUTOBOOST_HAS_THREADS)
#  undef AUTOBOOST_HAS_THREADS
#endif

//
// Turn threading support off if we don't recognise the threading API:
//
#if defined(AUTOBOOST_HAS_THREADS) && !defined(AUTOBOOST_HAS_PTHREADS)\
      && !defined(AUTOBOOST_HAS_WINTHREADS) && !defined(AUTOBOOST_HAS_BETHREADS)\
      && !defined(AUTOBOOST_HAS_MPTASKS)
#  undef AUTOBOOST_HAS_THREADS
#endif

//
// Turn threading detail macros off if we don't (want to) use threading
//
#ifndef AUTOBOOST_HAS_THREADS
#  undef AUTOBOOST_HAS_PTHREADS
#  undef AUTOBOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
#  undef AUTOBOOST_HAS_PTHREAD_YIELD
#  undef AUTOBOOST_HAS_PTHREAD_DELAY_NP
#  undef AUTOBOOST_HAS_WINTHREADS
#  undef AUTOBOOST_HAS_BETHREADS
#  undef AUTOBOOST_HAS_MPTASKS
#endif

//
// If the compiler claims to be C99 conformant, then it had better
// have a <stdint.h>:
//
#  if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#     define AUTOBOOST_HAS_STDINT_H
#     ifndef AUTOBOOST_HAS_LOG1P
#        define AUTOBOOST_HAS_LOG1P
#     endif
#     ifndef AUTOBOOST_HAS_EXPM1
#        define AUTOBOOST_HAS_EXPM1
#     endif
#  endif

//
// Define AUTOBOOST_NO_SLIST and AUTOBOOST_NO_HASH if required.
// Note that this is for backwards compatibility only.
//
#  if !defined(AUTOBOOST_HAS_SLIST) && !defined(AUTOBOOST_NO_SLIST)
#     define AUTOBOOST_NO_SLIST
#  endif

#  if !defined(AUTOBOOST_HAS_HASH) && !defined(AUTOBOOST_NO_HASH)
#     define AUTOBOOST_NO_HASH
#  endif

//
// Set AUTOBOOST_SLIST_HEADER if not set already:
//
#if defined(AUTOBOOST_HAS_SLIST) && !defined(AUTOBOOST_SLIST_HEADER)
#  define AUTOBOOST_SLIST_HEADER <slist>
#endif

//
// Set AUTOBOOST_HASH_SET_HEADER if not set already:
//
#if defined(AUTOBOOST_HAS_HASH) && !defined(AUTOBOOST_HASH_SET_HEADER)
#  define AUTOBOOST_HASH_SET_HEADER <hash_set>
#endif

//
// Set AUTOBOOST_HASH_MAP_HEADER if not set already:
//
#if defined(AUTOBOOST_HAS_HASH) && !defined(AUTOBOOST_HASH_MAP_HEADER)
#  define AUTOBOOST_HASH_MAP_HEADER <hash_map>
#endif

//  AUTOBOOST_HAS_ABI_HEADERS
//  This macro gets set if we have headers that fix the ABI,
//  and prevent ODR violations when linking to external libraries:
#if defined(AUTOBOOST_ABI_PREFIX) && defined(AUTOBOOST_ABI_SUFFIX) && !defined(AUTOBOOST_HAS_ABI_HEADERS)
#  define AUTOBOOST_HAS_ABI_HEADERS
#endif

#if defined(AUTOBOOST_HAS_ABI_HEADERS) && defined(AUTOBOOST_DISABLE_ABI_HEADERS)
#  undef AUTOBOOST_HAS_ABI_HEADERS
#endif

//  AUTOBOOST_NO_STDC_NAMESPACE workaround  --------------------------------------//
//  Because std::size_t usage is so common, even in autoboost headers which do not
//  otherwise use the C library, the <cstddef> workaround is included here so
//  that ugly workaround code need not appear in many other autoboost headers.
//  NOTE WELL: This is a workaround for non-conforming compilers; <cstddef>
//  must still be #included in the usual places so that <cstddef> inclusion
//  works as expected with standard conforming compilers.  The resulting
//  double inclusion of <cstddef> is harmless.

# if defined(AUTOBOOST_NO_STDC_NAMESPACE) && defined(__cplusplus)
#   include <cstddef>
    namespace std { using ::ptrdiff_t; using ::size_t; }
# endif

//  Workaround for the unfortunate min/max macros defined by some platform headers

#define AUTOBOOST_PREVENT_MACRO_SUBSTITUTION

#ifndef AUTOBOOST_USING_STD_MIN
#  define AUTOBOOST_USING_STD_MIN() using std::min
#endif

#ifndef AUTOBOOST_USING_STD_MAX
#  define AUTOBOOST_USING_STD_MAX() using std::max
#endif

//  AUTOBOOST_NO_STD_MIN_MAX workaround  -----------------------------------------//

#  if defined(AUTOBOOST_NO_STD_MIN_MAX) && defined(__cplusplus)

namespace std {
  template <class _Tp>
  inline const _Tp& min AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (const _Tp& __a, const _Tp& __b) {
    return __b < __a ? __b : __a;
  }
  template <class _Tp>
  inline const _Tp& max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION (const _Tp& __a, const _Tp& __b) {
    return  __a < __b ? __b : __a;
  }
}

#  endif

// AUTOBOOST_STATIC_CONSTANT workaround --------------------------------------- //
// On compilers which don't allow in-class initialization of static integral
// constant members, we must use enums as a workaround if we want the constants
// to be available at compile-time. This macro gives us a convenient way to
// declare such constants.

#  ifdef AUTOBOOST_NO_INCLASS_MEMBER_INITIALIZATION
#       define AUTOBOOST_STATIC_CONSTANT(type, assignment) enum { assignment }
#  else
#     define AUTOBOOST_STATIC_CONSTANT(type, assignment) static const type assignment
#  endif

// AUTOBOOST_USE_FACET / HAS_FACET workaround ----------------------------------//
// When the standard library does not have a conforming std::use_facet there
// are various workarounds available, but they differ from library to library.
// The same problem occurs with has_facet.
// These macros provide a consistent way to access a locale's facets.
// Usage:
//    replace
//       std::use_facet<Type>(loc);
//    with
//       AUTOBOOST_USE_FACET(Type, loc);
//    Note do not add a std:: prefix to the front of AUTOBOOST_USE_FACET!
//  Use for AUTOBOOST_HAS_FACET is analogous.

#if defined(AUTOBOOST_NO_STD_USE_FACET)
#  ifdef AUTOBOOST_HAS_TWO_ARG_USE_FACET
#     define AUTOBOOST_USE_FACET(Type, loc) std::use_facet(loc, static_cast<Type*>(0))
#     define AUTOBOOST_HAS_FACET(Type, loc) std::has_facet(loc, static_cast<Type*>(0))
#  elif defined(AUTOBOOST_HAS_MACRO_USE_FACET)
#     define AUTOBOOST_USE_FACET(Type, loc) std::_USE(loc, Type)
#     define AUTOBOOST_HAS_FACET(Type, loc) std::_HAS(loc, Type)
#  elif defined(AUTOBOOST_HAS_STLP_USE_FACET)
#     define AUTOBOOST_USE_FACET(Type, loc) (*std::_Use_facet<Type >(loc))
#     define AUTOBOOST_HAS_FACET(Type, loc) std::has_facet< Type >(loc)
#  endif
#else
#  define AUTOBOOST_USE_FACET(Type, loc) std::use_facet< Type >(loc)
#  define AUTOBOOST_HAS_FACET(Type, loc) std::has_facet< Type >(loc)
#endif

// AUTOBOOST_NESTED_TEMPLATE workaround ------------------------------------------//
// Member templates are supported by some compilers even though they can't use
// the A::template member<U> syntax, as a workaround replace:
//
// typedef typename A::template rebind<U> binder;
//
// with:
//
// typedef typename A::AUTOBOOST_NESTED_TEMPLATE rebind<U> binder;

#ifndef AUTOBOOST_NO_MEMBER_TEMPLATE_KEYWORD
#  define AUTOBOOST_NESTED_TEMPLATE template
#else
#  define AUTOBOOST_NESTED_TEMPLATE
#endif

// AUTOBOOST_UNREACHABLE_RETURN(x) workaround -------------------------------------//
// Normally evaluates to nothing, unless AUTOBOOST_NO_UNREACHABLE_RETURN_DETECTION
// is defined, in which case it evaluates to return x; Use when you have a return
// statement that can never be reached.

#ifndef AUTOBOOST_UNREACHABLE_RETURN
#  ifdef AUTOBOOST_NO_UNREACHABLE_RETURN_DETECTION
#     define AUTOBOOST_UNREACHABLE_RETURN(x) return x;
#  else
#     define AUTOBOOST_UNREACHABLE_RETURN(x)
#  endif
#endif

// AUTOBOOST_DEDUCED_TYPENAME workaround ------------------------------------------//
//
// Some compilers don't support the use of `typename' for dependent
// types in deduced contexts, e.g.
//
//     template <class T> void f(T, typename T::type);
//                                  ^^^^^^^^
// Replace these declarations with:
//
//     template <class T> void f(T, AUTOBOOST_DEDUCED_TYPENAME T::type);

#ifndef AUTOBOOST_NO_DEDUCED_TYPENAME
#  define AUTOBOOST_DEDUCED_TYPENAME typename
#else
#  define AUTOBOOST_DEDUCED_TYPENAME
#endif

#ifndef AUTOBOOST_NO_TYPENAME_WITH_CTOR
#  define AUTOBOOST_CTOR_TYPENAME typename
#else
#  define AUTOBOOST_CTOR_TYPENAME
#endif

// long long workaround ------------------------------------------//
// On gcc (and maybe other compilers?) long long is alway supported
// but it's use may generate either warnings (with -ansi), or errors
// (with -pedantic -ansi) unless it's use is prefixed by __extension__
//
#if defined(AUTOBOOST_HAS_LONG_LONG) && defined(__cplusplus)
namespace autoboost{
#  ifdef __GNUC__
   __extension__ typedef long long long_long_type;
   __extension__ typedef unsigned long long ulong_long_type;
#  else
   typedef long long long_long_type;
   typedef unsigned long long ulong_long_type;
#  endif
}
#endif
// same again for __int128:
#if defined(AUTOBOOST_HAS_INT128) && defined(__cplusplus)
namespace autoboost{
#  ifdef __GNUC__
   __extension__ typedef __int128 int128_type;
   __extension__ typedef unsigned __int128 uint128_type;
#  else
   typedef __int128 int128_type;
   typedef unsigned __int128 uint128_type;
#  endif
}
#endif
// same again for __float128:
#if defined(AUTOBOOST_HAS_FLOAT128) && defined(__cplusplus)
namespace autoboost {
#  ifdef __GNUC__
   __extension__ typedef __float128 float128_type;
#  else
   typedef __float128 float128_type;
#  endif
}
#endif

// AUTOBOOST_[APPEND_]EXPLICIT_TEMPLATE_[NON_]TYPE macros --------------------------//

// These macros are obsolete. Port away and remove.

#  define AUTOBOOST_EXPLICIT_TEMPLATE_TYPE(t)
#  define AUTOBOOST_EXPLICIT_TEMPLATE_TYPE_SPEC(t)
#  define AUTOBOOST_EXPLICIT_TEMPLATE_NON_TYPE(t, v)
#  define AUTOBOOST_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)

#  define AUTOBOOST_APPEND_EXPLICIT_TEMPLATE_TYPE(t)
#  define AUTOBOOST_APPEND_EXPLICIT_TEMPLATE_TYPE_SPEC(t)
#  define AUTOBOOST_APPEND_EXPLICIT_TEMPLATE_NON_TYPE(t, v)
#  define AUTOBOOST_APPEND_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)

// When AUTOBOOST_NO_STD_TYPEINFO is defined, we can just import
// the global definition into std namespace:
#if defined(AUTOBOOST_NO_STD_TYPEINFO) && defined(__cplusplus)
#include <typeinfo>
namespace std{ using ::type_info; }
#endif

// ---------------------------------------------------------------------------//

//
// Helper macro AUTOBOOST_STRINGIZE:
// Converts the parameter X to a string after macro replacement
// on X has been performed.
//
#define AUTOBOOST_STRINGIZE(X) AUTOBOOST_DO_STRINGIZE(X)
#define AUTOBOOST_DO_STRINGIZE(X) #X

//
// Helper macro AUTOBOOST_JOIN:
// The following piece of macro magic joins the two
// arguments together, even when one of the arguments is
// itself a macro (see 16.3.1 in C++ standard).  The key
// is that macro expansion of macro arguments does not
// occur in AUTOBOOST_DO_JOIN2 but does in AUTOBOOST_DO_JOIN.
//
#define AUTOBOOST_JOIN( X, Y ) AUTOBOOST_DO_JOIN( X, Y )
#define AUTOBOOST_DO_JOIN( X, Y ) AUTOBOOST_DO_JOIN2(X,Y)
#define AUTOBOOST_DO_JOIN2( X, Y ) X##Y

//
// Set some default values for compiler/library/platform names.
// These are for debugging config setup only:
//
#  ifndef AUTOBOOST_COMPILER
#     define AUTOBOOST_COMPILER "Unknown ISO C++ Compiler"
#  endif
#  ifndef AUTOBOOST_STDLIB
#     define AUTOBOOST_STDLIB "Unknown ISO standard library"
#  endif
#  ifndef AUTOBOOST_PLATFORM
#     if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) \
         || defined(_POSIX_SOURCE)
#        define AUTOBOOST_PLATFORM "Generic Unix"
#     else
#        define AUTOBOOST_PLATFORM "Unknown"
#     endif
#  endif

//
// Set some default values GPU support
//
#  ifndef AUTOBOOST_GPU_ENABLED
#  define AUTOBOOST_GPU_ENABLED
#  endif

// AUTOBOOST_RESTRICT ---------------------------------------------//
// Macro to use in place of 'restrict' keyword variants
#if !defined(AUTOBOOST_RESTRICT)
#  if defined(_MSC_VER)
#    define AUTOBOOST_RESTRICT __restrict
#    if !defined(AUTOBOOST_NO_RESTRICT_REFERENCES) && (_MSC_FULL_VER < 190023026)
#      define AUTOBOOST_NO_RESTRICT_REFERENCES
#    endif
#  elif defined(__GNUC__) && __GNUC__ > 3
     // Clang also defines __GNUC__ (as 4)
#    define AUTOBOOST_RESTRICT __restrict__
#  else
#    define AUTOBOOST_RESTRICT
#    if !defined(AUTOBOOST_NO_RESTRICT_REFERENCES)
#      define AUTOBOOST_NO_RESTRICT_REFERENCES
#    endif
#  endif
#endif

// AUTOBOOST_FORCEINLINE ---------------------------------------------//
// Macro to use in place of 'inline' to force a function to be inline
#if !defined(AUTOBOOST_FORCEINLINE)
#  if defined(_MSC_VER)
#    define AUTOBOOST_FORCEINLINE __forceinline
#  elif defined(__GNUC__) && __GNUC__ > 3
     // Clang also defines __GNUC__ (as 4)
#    define AUTOBOOST_FORCEINLINE inline __attribute__ ((__always_inline__))
#  else
#    define AUTOBOOST_FORCEINLINE inline
#  endif
#endif

// AUTOBOOST_NOINLINE ---------------------------------------------//
// Macro to use in place of 'inline' to prevent a function to be inlined
#if !defined(AUTOBOOST_NOINLINE)
#  if defined(_MSC_VER)
#    define AUTOBOOST_NOINLINE __declspec(noinline)
#  elif defined(__GNUC__) && __GNUC__ > 3
     // Clang also defines __GNUC__ (as 4)
#    if defined(__CUDACC__)
       // nvcc doesn't always parse __noinline__,
       // see: https://svn.boost.org/trac/autoboost/ticket/9392
#      define AUTOBOOST_NOINLINE __attribute__ ((noinline))
#    else
#      define AUTOBOOST_NOINLINE __attribute__ ((__noinline__))
#    endif
#  else
#    define AUTOBOOST_NOINLINE
#  endif
#endif

// AUTOBOOST_NORETURN ---------------------------------------------//
// Macro to use before a function declaration/definition to designate
// the function as not returning normally (i.e. with a return statement
// or by leaving the function scope, if the function return type is void).
#if !defined(AUTOBOOST_NORETURN)
#  if defined(_MSC_VER)
#    define AUTOBOOST_NORETURN __declspec(noreturn)
#  elif defined(__GNUC__)
#    define AUTOBOOST_NORETURN __attribute__ ((__noreturn__))
#  elif defined(__has_attribute) && defined(__SUNPRO_CC)
#    if __has_attribute(noreturn)
#      define AUTOBOOST_NORETURN [[noreturn]]
#    endif
#  elif defined(__has_cpp_attribute)
#    if __has_cpp_attribute(noreturn)
#      define AUTOBOOST_NORETURN [[noreturn]]
#    endif
#  endif
#endif

#if !defined(AUTOBOOST_NORETURN)
#  define AUTOBOOST_NO_NORETURN
#  define AUTOBOOST_NORETURN
#endif

// Branch prediction hints
// These macros are intended to wrap conditional expressions that yield true or false
//
//  if (AUTOBOOST_LIKELY(var == 10))
//  {
//     // the most probable code here
//  }
//
#if !defined(AUTOBOOST_LIKELY)
#  define AUTOBOOST_LIKELY(x) x
#endif
#if !defined(AUTOBOOST_UNLIKELY)
#  define AUTOBOOST_UNLIKELY(x) x
#endif

// Type and data alignment specification
//
#if !defined(AUTOBOOST_NO_CXX11_ALIGNAS)
#  define AUTOBOOST_ALIGNMENT(x) alignas(x)
#elif defined(_MSC_VER)
#  define AUTOBOOST_ALIGNMENT(x) __declspec(align(x))
#elif defined(__GNUC__)
#  define AUTOBOOST_ALIGNMENT(x) __attribute__ ((__aligned__(x)))
#else
#  define AUTOBOOST_NO_ALIGNMENT
#  define AUTOBOOST_ALIGNMENT(x)
#endif

// Lack of non-public defaulted functions is implied by the lack of any defaulted functions
#if !defined(AUTOBOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS) && defined(AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS)
#  define AUTOBOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS
#endif

// Defaulted and deleted function declaration helpers
// These macros are intended to be inside a class definition.
// AUTOBOOST_DEFAULTED_FUNCTION accepts the function declaration and its
// body, which will be used if the compiler doesn't support defaulted functions.
// AUTOBOOST_DELETED_FUNCTION only accepts the function declaration. It
// will expand to a private function declaration, if the compiler doesn't support
// deleted functions. Because of this it is recommended to use AUTOBOOST_DELETED_FUNCTION
// in the end of the class definition.
//
//  class my_class
//  {
//  public:
//      // Default-constructible
//      AUTOBOOST_DEFAULTED_FUNCTION(my_class(), {})
//      // Copying prohibited
//      AUTOBOOST_DELETED_FUNCTION(my_class(my_class const&))
//      AUTOBOOST_DELETED_FUNCTION(my_class& operator= (my_class const&))
//  };
//
#if !(defined(AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS) || defined(AUTOBOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS))
#   define AUTOBOOST_DEFAULTED_FUNCTION(fun, body) fun = default;
#else
#   define AUTOBOOST_DEFAULTED_FUNCTION(fun, body) fun body
#endif

#if !defined(AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS)
#   define AUTOBOOST_DELETED_FUNCTION(fun) fun = delete;
#else
#   define AUTOBOOST_DELETED_FUNCTION(fun) private: fun;
#endif

//
// Set AUTOBOOST_NO_DECLTYPE_N3276 when AUTOBOOST_NO_DECLTYPE is defined
//
#if defined(AUTOBOOST_NO_CXX11_DECLTYPE) && !defined(AUTOBOOST_NO_CXX11_DECLTYPE_N3276)
#define AUTOBOOST_NO_CXX11_DECLTYPE_N3276 AUTOBOOST_NO_CXX11_DECLTYPE
#endif

//  -------------------- Deprecated macros for 1.50 ---------------------------
//  These will go away in a future release

//  Use AUTOBOOST_NO_CXX11_HDR_UNORDERED_SET or AUTOBOOST_NO_CXX11_HDR_UNORDERED_MAP
//           instead of AUTOBOOST_NO_STD_UNORDERED
#if defined(AUTOBOOST_NO_CXX11_HDR_UNORDERED_MAP) || defined (AUTOBOOST_NO_CXX11_HDR_UNORDERED_SET)
# ifndef AUTOBOOST_NO_CXX11_STD_UNORDERED
#  define AUTOBOOST_NO_CXX11_STD_UNORDERED
# endif
#endif

//  Use AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST instead of AUTOBOOST_NO_INITIALIZER_LISTS
#if defined(AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST) && !defined(AUTOBOOST_NO_INITIALIZER_LISTS)
#  define AUTOBOOST_NO_INITIALIZER_LISTS
#endif

//  Use AUTOBOOST_NO_CXX11_HDR_ARRAY instead of AUTOBOOST_NO_0X_HDR_ARRAY
#if defined(AUTOBOOST_NO_CXX11_HDR_ARRAY) && !defined(AUTOBOOST_NO_0X_HDR_ARRAY)
#  define AUTOBOOST_NO_0X_HDR_ARRAY
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_CHRONO instead of AUTOBOOST_NO_0X_HDR_CHRONO
#if defined(AUTOBOOST_NO_CXX11_HDR_CHRONO) && !defined(AUTOBOOST_NO_0X_HDR_CHRONO)
#  define AUTOBOOST_NO_0X_HDR_CHRONO
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_CODECVT instead of AUTOBOOST_NO_0X_HDR_CODECVT
#if defined(AUTOBOOST_NO_CXX11_HDR_CODECVT) && !defined(AUTOBOOST_NO_0X_HDR_CODECVT)
#  define AUTOBOOST_NO_0X_HDR_CODECVT
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_CONDITION_VARIABLE instead of AUTOBOOST_NO_0X_HDR_CONDITION_VARIABLE
#if defined(AUTOBOOST_NO_CXX11_HDR_CONDITION_VARIABLE) && !defined(AUTOBOOST_NO_0X_HDR_CONDITION_VARIABLE)
#  define AUTOBOOST_NO_0X_HDR_CONDITION_VARIABLE
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_FORWARD_LIST instead of AUTOBOOST_NO_0X_HDR_FORWARD_LIST
#if defined(AUTOBOOST_NO_CXX11_HDR_FORWARD_LIST) && !defined(AUTOBOOST_NO_0X_HDR_FORWARD_LIST)
#  define AUTOBOOST_NO_0X_HDR_FORWARD_LIST
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_FUTURE instead of AUTOBOOST_NO_0X_HDR_FUTURE
#if defined(AUTOBOOST_NO_CXX11_HDR_FUTURE) && !defined(AUTOBOOST_NO_0X_HDR_FUTURE)
#  define AUTOBOOST_NO_0X_HDR_FUTURE
#endif

//  Use AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
//  instead of AUTOBOOST_NO_0X_HDR_INITIALIZER_LIST or AUTOBOOST_NO_INITIALIZER_LISTS
#ifdef AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
# ifndef AUTOBOOST_NO_0X_HDR_INITIALIZER_LIST
#  define AUTOBOOST_NO_0X_HDR_INITIALIZER_LIST
# endif
# ifndef AUTOBOOST_NO_INITIALIZER_LISTS
#  define AUTOBOOST_NO_INITIALIZER_LISTS
# endif
#endif

//  Use AUTOBOOST_NO_CXX11_HDR_MUTEX instead of AUTOBOOST_NO_0X_HDR_MUTEX
#if defined(AUTOBOOST_NO_CXX11_HDR_MUTEX) && !defined(AUTOBOOST_NO_0X_HDR_MUTEX)
#  define AUTOBOOST_NO_0X_HDR_MUTEX
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_RANDOM instead of AUTOBOOST_NO_0X_HDR_RANDOM
#if defined(AUTOBOOST_NO_CXX11_HDR_RANDOM) && !defined(AUTOBOOST_NO_0X_HDR_RANDOM)
#  define AUTOBOOST_NO_0X_HDR_RANDOM
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_RATIO instead of AUTOBOOST_NO_0X_HDR_RATIO
#if defined(AUTOBOOST_NO_CXX11_HDR_RATIO) && !defined(AUTOBOOST_NO_0X_HDR_RATIO)
#  define AUTOBOOST_NO_0X_HDR_RATIO
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_REGEX instead of AUTOBOOST_NO_0X_HDR_REGEX
#if defined(AUTOBOOST_NO_CXX11_HDR_REGEX) && !defined(AUTOBOOST_NO_0X_HDR_REGEX)
#  define AUTOBOOST_NO_0X_HDR_REGEX
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_SYSTEM_ERROR instead of AUTOBOOST_NO_0X_HDR_SYSTEM_ERROR
#if defined(AUTOBOOST_NO_CXX11_HDR_SYSTEM_ERROR) && !defined(AUTOBOOST_NO_0X_HDR_SYSTEM_ERROR)
#  define AUTOBOOST_NO_0X_HDR_SYSTEM_ERROR
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_THREAD instead of AUTOBOOST_NO_0X_HDR_THREAD
#if defined(AUTOBOOST_NO_CXX11_HDR_THREAD) && !defined(AUTOBOOST_NO_0X_HDR_THREAD)
#  define AUTOBOOST_NO_0X_HDR_THREAD
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_TUPLE instead of AUTOBOOST_NO_0X_HDR_TUPLE
#if defined(AUTOBOOST_NO_CXX11_HDR_TUPLE) && !defined(AUTOBOOST_NO_0X_HDR_TUPLE)
#  define AUTOBOOST_NO_0X_HDR_TUPLE
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS instead of AUTOBOOST_NO_0X_HDR_TYPE_TRAITS
#if defined(AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS) && !defined(AUTOBOOST_NO_0X_HDR_TYPE_TRAITS)
#  define AUTOBOOST_NO_0X_HDR_TYPE_TRAITS
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_TYPEINDEX instead of AUTOBOOST_NO_0X_HDR_TYPEINDEX
#if defined(AUTOBOOST_NO_CXX11_HDR_TYPEINDEX) && !defined(AUTOBOOST_NO_0X_HDR_TYPEINDEX)
#  define AUTOBOOST_NO_0X_HDR_TYPEINDEX
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_UNORDERED_MAP instead of AUTOBOOST_NO_0X_HDR_UNORDERED_MAP
#if defined(AUTOBOOST_NO_CXX11_HDR_UNORDERED_MAP) && !defined(AUTOBOOST_NO_0X_HDR_UNORDERED_MAP)
#  define AUTOBOOST_NO_0X_HDR_UNORDERED_MAP
#endif
//  Use AUTOBOOST_NO_CXX11_HDR_UNORDERED_SET instead of AUTOBOOST_NO_0X_HDR_UNORDERED_SET
#if defined(AUTOBOOST_NO_CXX11_HDR_UNORDERED_SET) && !defined(AUTOBOOST_NO_0X_HDR_UNORDERED_SET)
#  define AUTOBOOST_NO_0X_HDR_UNORDERED_SET
#endif

//  ------------------ End of deprecated macros for 1.50 ---------------------------

//  -------------------- Deprecated macros for 1.51 ---------------------------
//  These will go away in a future release

//  Use     AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS instead of   AUTOBOOST_NO_AUTO_DECLARATIONS
#if defined(AUTOBOOST_NO_CXX11_AUTO_DECLARATIONS) && !defined(AUTOBOOST_NO_AUTO_DECLARATIONS)
#  define AUTOBOOST_NO_AUTO_DECLARATIONS
#endif
//  Use     AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS instead of   AUTOBOOST_NO_AUTO_MULTIDECLARATIONS
#if defined(AUTOBOOST_NO_CXX11_AUTO_MULTIDECLARATIONS) && !defined(AUTOBOOST_NO_AUTO_MULTIDECLARATIONS)
#  define AUTOBOOST_NO_AUTO_MULTIDECLARATIONS
#endif
//  Use     AUTOBOOST_NO_CXX11_CHAR16_T instead of   AUTOBOOST_NO_CHAR16_T
#if defined(AUTOBOOST_NO_CXX11_CHAR16_T) && !defined(AUTOBOOST_NO_CHAR16_T)
#  define AUTOBOOST_NO_CHAR16_T
#endif
//  Use     AUTOBOOST_NO_CXX11_CHAR32_T instead of   AUTOBOOST_NO_CHAR32_T
#if defined(AUTOBOOST_NO_CXX11_CHAR32_T) && !defined(AUTOBOOST_NO_CHAR32_T)
#  define AUTOBOOST_NO_CHAR32_T
#endif
//  Use     AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES instead of   AUTOBOOST_NO_TEMPLATE_ALIASES
#if defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES) && !defined(AUTOBOOST_NO_TEMPLATE_ALIASES)
#  define AUTOBOOST_NO_TEMPLATE_ALIASES
#endif
//  Use     AUTOBOOST_NO_CXX11_CONSTEXPR instead of   AUTOBOOST_NO_CONSTEXPR
#if defined(AUTOBOOST_NO_CXX11_CONSTEXPR) && !defined(AUTOBOOST_NO_CONSTEXPR)
#  define AUTOBOOST_NO_CONSTEXPR
#endif
//  Use     AUTOBOOST_NO_CXX11_DECLTYPE_N3276 instead of   AUTOBOOST_NO_DECLTYPE_N3276
#if defined(AUTOBOOST_NO_CXX11_DECLTYPE_N3276) && !defined(AUTOBOOST_NO_DECLTYPE_N3276)
#  define AUTOBOOST_NO_DECLTYPE_N3276
#endif
//  Use     AUTOBOOST_NO_CXX11_DECLTYPE instead of   AUTOBOOST_NO_DECLTYPE
#if defined(AUTOBOOST_NO_CXX11_DECLTYPE) && !defined(AUTOBOOST_NO_DECLTYPE)
#  define AUTOBOOST_NO_DECLTYPE
#endif
//  Use     AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS instead of   AUTOBOOST_NO_DEFAULTED_FUNCTIONS
#if defined(AUTOBOOST_NO_CXX11_DEFAULTED_FUNCTIONS) && !defined(AUTOBOOST_NO_DEFAULTED_FUNCTIONS)
#  define AUTOBOOST_NO_DEFAULTED_FUNCTIONS
#endif
//  Use     AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS instead of   AUTOBOOST_NO_DELETED_FUNCTIONS
#if defined(AUTOBOOST_NO_CXX11_DELETED_FUNCTIONS) && !defined(AUTOBOOST_NO_DELETED_FUNCTIONS)
#  define AUTOBOOST_NO_DELETED_FUNCTIONS
#endif
//  Use     AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS instead of   AUTOBOOST_NO_EXPLICIT_CONVERSION_OPERATORS
#if defined(AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS) && !defined(AUTOBOOST_NO_EXPLICIT_CONVERSION_OPERATORS)
#  define AUTOBOOST_NO_EXPLICIT_CONVERSION_OPERATORS
#endif
//  Use     AUTOBOOST_NO_CXX11_EXTERN_TEMPLATE instead of   AUTOBOOST_NO_EXTERN_TEMPLATE
#if defined(AUTOBOOST_NO_CXX11_EXTERN_TEMPLATE) && !defined(AUTOBOOST_NO_EXTERN_TEMPLATE)
#  define AUTOBOOST_NO_EXTERN_TEMPLATE
#endif
//  Use     AUTOBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS instead of   AUTOBOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
#if defined(AUTOBOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) && !defined(AUTOBOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
#  define AUTOBOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
#endif
//  Use     AUTOBOOST_NO_CXX11_LAMBDAS instead of   AUTOBOOST_NO_LAMBDAS
#if defined(AUTOBOOST_NO_CXX11_LAMBDAS) && !defined(AUTOBOOST_NO_LAMBDAS)
#  define AUTOBOOST_NO_LAMBDAS
#endif
//  Use     AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS instead of   AUTOBOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
#if defined(AUTOBOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS) && !defined(AUTOBOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS)
#  define AUTOBOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
#endif
//  Use     AUTOBOOST_NO_CXX11_NOEXCEPT instead of   AUTOBOOST_NO_NOEXCEPT
#if defined(AUTOBOOST_NO_CXX11_NOEXCEPT) && !defined(AUTOBOOST_NO_NOEXCEPT)
#  define AUTOBOOST_NO_NOEXCEPT
#endif
//  Use     AUTOBOOST_NO_CXX11_NULLPTR instead of   AUTOBOOST_NO_NULLPTR
#if defined(AUTOBOOST_NO_CXX11_NULLPTR) && !defined(AUTOBOOST_NO_NULLPTR)
#  define AUTOBOOST_NO_NULLPTR
#endif
//  Use     AUTOBOOST_NO_CXX11_RAW_LITERALS instead of   AUTOBOOST_NO_RAW_LITERALS
#if defined(AUTOBOOST_NO_CXX11_RAW_LITERALS) && !defined(AUTOBOOST_NO_RAW_LITERALS)
#  define AUTOBOOST_NO_RAW_LITERALS
#endif
//  Use     AUTOBOOST_NO_CXX11_RVALUE_REFERENCES instead of   AUTOBOOST_NO_RVALUE_REFERENCES
#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_NO_RVALUE_REFERENCES)
#  define AUTOBOOST_NO_RVALUE_REFERENCES
#endif
//  Use     AUTOBOOST_NO_CXX11_SCOPED_ENUMS instead of   AUTOBOOST_NO_SCOPED_ENUMS
#if defined(AUTOBOOST_NO_CXX11_SCOPED_ENUMS) && !defined(AUTOBOOST_NO_SCOPED_ENUMS)
#  define AUTOBOOST_NO_SCOPED_ENUMS
#endif
//  Use     AUTOBOOST_NO_CXX11_STATIC_ASSERT instead of   AUTOBOOST_NO_STATIC_ASSERT
#if defined(AUTOBOOST_NO_CXX11_STATIC_ASSERT) && !defined(AUTOBOOST_NO_STATIC_ASSERT)
#  define AUTOBOOST_NO_STATIC_ASSERT
#endif
//  Use     AUTOBOOST_NO_CXX11_STD_UNORDERED instead of   AUTOBOOST_NO_STD_UNORDERED
#if defined(AUTOBOOST_NO_CXX11_STD_UNORDERED) && !defined(AUTOBOOST_NO_STD_UNORDERED)
#  define AUTOBOOST_NO_STD_UNORDERED
#endif
//  Use     AUTOBOOST_NO_CXX11_UNICODE_LITERALS instead of   AUTOBOOST_NO_UNICODE_LITERALS
#if defined(AUTOBOOST_NO_CXX11_UNICODE_LITERALS) && !defined(AUTOBOOST_NO_UNICODE_LITERALS)
#  define AUTOBOOST_NO_UNICODE_LITERALS
#endif
//  Use     AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX instead of   AUTOBOOST_NO_UNIFIED_INITIALIZATION_SYNTAX
#if defined(AUTOBOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX) && !defined(AUTOBOOST_NO_UNIFIED_INITIALIZATION_SYNTAX)
#  define AUTOBOOST_NO_UNIFIED_INITIALIZATION_SYNTAX
#endif
//  Use     AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES instead of   AUTOBOOST_NO_VARIADIC_TEMPLATES
#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(AUTOBOOST_NO_VARIADIC_TEMPLATES)
#  define AUTOBOOST_NO_VARIADIC_TEMPLATES
#endif
//  Use     AUTOBOOST_NO_CXX11_VARIADIC_MACROS instead of   AUTOBOOST_NO_VARIADIC_MACROS
#if defined(AUTOBOOST_NO_CXX11_VARIADIC_MACROS) && !defined(AUTOBOOST_NO_VARIADIC_MACROS)
#  define AUTOBOOST_NO_VARIADIC_MACROS
#endif
//  Use     AUTOBOOST_NO_CXX11_NUMERIC_LIMITS instead of   AUTOBOOST_NO_NUMERIC_LIMITS_LOWEST
#if defined(AUTOBOOST_NO_CXX11_NUMERIC_LIMITS) && !defined(AUTOBOOST_NO_NUMERIC_LIMITS_LOWEST)
#  define AUTOBOOST_NO_NUMERIC_LIMITS_LOWEST
#endif
//  ------------------ End of deprecated macros for 1.51 ---------------------------



//
// Helper macros AUTOBOOST_NOEXCEPT, AUTOBOOST_NOEXCEPT_IF, AUTOBOOST_NOEXCEPT_EXPR
// These aid the transition to C++11 while still supporting C++03 compilers
//
#ifdef AUTOBOOST_NO_CXX11_NOEXCEPT
#  define AUTOBOOST_NOEXCEPT
#  define AUTOBOOST_NOEXCEPT_OR_NOTHROW throw()
#  define AUTOBOOST_NOEXCEPT_IF(Predicate)
#  define AUTOBOOST_NOEXCEPT_EXPR(Expression) false
#else
#  define AUTOBOOST_NOEXCEPT noexcept
#  define AUTOBOOST_NOEXCEPT_OR_NOTHROW noexcept
#  define AUTOBOOST_NOEXCEPT_IF(Predicate) noexcept((Predicate))
#  define AUTOBOOST_NOEXCEPT_EXPR(Expression) noexcept((Expression))
#endif
//
// Helper macro AUTOBOOST_FALLTHROUGH
// Fallback definition of AUTOBOOST_FALLTHROUGH macro used to mark intended
// fall-through between case labels in a switch statement. We use a definition
// that requires a semicolon after it to avoid at least one type of misuse even
// on unsupported compilers.
//
#ifndef AUTOBOOST_FALLTHROUGH
#  define AUTOBOOST_FALLTHROUGH ((void)0)
#endif

//
// constexpr workarounds
//
#if defined(AUTOBOOST_NO_CXX11_CONSTEXPR)
#define AUTOBOOST_CONSTEXPR
#define AUTOBOOST_CONSTEXPR_OR_CONST const
#else
#define AUTOBOOST_CONSTEXPR constexpr
#define AUTOBOOST_CONSTEXPR_OR_CONST constexpr
#endif
#if defined(AUTOBOOST_NO_CXX14_CONSTEXPR)
#define AUTOBOOST_CXX14_CONSTEXPR
#else
#define AUTOBOOST_CXX14_CONSTEXPR constexpr
#endif

//
// Unused variable/typedef workarounds:
//
#ifndef AUTOBOOST_ATTRIBUTE_UNUSED
#  define AUTOBOOST_ATTRIBUTE_UNUSED
#endif

#define AUTOBOOST_STATIC_CONSTEXPR  static AUTOBOOST_CONSTEXPR_OR_CONST

//
// Set AUTOBOOST_HAS_STATIC_ASSERT when AUTOBOOST_NO_CXX11_STATIC_ASSERT is not defined
//
#if !defined(AUTOBOOST_NO_CXX11_STATIC_ASSERT) && !defined(AUTOBOOST_HAS_STATIC_ASSERT)
#  define AUTOBOOST_HAS_STATIC_ASSERT
#endif

//
// Set AUTOBOOST_HAS_RVALUE_REFS when AUTOBOOST_NO_CXX11_RVALUE_REFERENCES is not defined
//
#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(AUTOBOOST_HAS_RVALUE_REFS)
#define AUTOBOOST_HAS_RVALUE_REFS
#endif

//
// Set AUTOBOOST_HAS_VARIADIC_TMPL when AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES is not defined
//
#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(AUTOBOOST_HAS_VARIADIC_TMPL)
#define AUTOBOOST_HAS_VARIADIC_TMPL
#endif
//
// Set AUTOBOOST_NO_CXX11_FIXED_LENGTH_VARIADIC_TEMPLATE_EXPANSION_PACKS when
// AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES is set:
//
#if defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(AUTOBOOST_NO_CXX11_FIXED_LENGTH_VARIADIC_TEMPLATE_EXPANSION_PACKS)
#  define AUTOBOOST_NO_CXX11_FIXED_LENGTH_VARIADIC_TEMPLATE_EXPANSION_PACKS
#endif

//
// Finish off with checks for macros that are depricated / no longer supported,
// if any of these are set then it's very likely that much of Boost will no
// longer work.  So stop with a #error for now, but give the user a chance
// to continue at their own risk if they really want to:
//
#if defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(AUTOBOOST_CONFIG_ALLOW_DEPRECATED)
#  error "You are using a compiler which lacks features which are now a minimum requirement in order to use Boost, define AUTOBOOST_CONFIG_ALLOW_DEPRECATED if you want to continue at your own risk!!!"
#endif

#endif
