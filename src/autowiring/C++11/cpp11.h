// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

// The reason this header exists is due to the asymmetric availability of C++11 on our
// various compiler targets.  In particular, none of the typical headers for C++11 support
// exist when building with libstdc++ on Apple, so we have to disable it across the board.
#ifdef __APPLE__
  #include <ciso646>  // detect std::lib
  #ifdef _LIBCPP_VERSION // clang + libc++
    #define STL11_ALLOWED 1
  #else // clang + libstdc++
    #define STL11_ALLOWED 0
  #endif
#else // gcc or MSVC
  #define STL11_ALLOWED 1
#endif

// Deprecated macros, provided here until we can be sure that they aren't used downstream
#define EXCEPTION_PTR_HEADER <stdexcept>

#define IS_CLANG defined(__clang_major__)
#define CLANG_CHECK(maj, min) (__clang_major__ == maj && __clang_minor__ >= min || __clang_major__ > maj)
#define GCC_CHECK(maj, min) (__GNUC__ == maj && __GNUC_MINOR__  >= min || __GNUC__ > maj)

// If Boost.Thread is used, we want it to provide the new name for its <future> class
#define BOOST_THREAD_PROVIDES_FUTURE

#ifdef AUTOWIRING_IS_BEING_BUILT
  // We know that we're using deprecated stuff in our unit tests, but the tests still
  // need to do what they do.  Undefine all of the deprecated macros so we don't get
  // spammed with warnings telling us what we already know.
  #define AUTOWIRING_NO_DEPRECATE
#endif

#ifndef __has_feature
  #define __has_feature(x) (AUTOWIRE_##x)
#endif

/*********************
 * __func__ function name support
 *********************/
#ifdef _MSC_VER
  #define __func__ __FUNCTION__
#endif

/*********************
 * alignas support added in all versions of GCC, but not until MSVC 2015
 *********************/
#if defined(_MSC_VER) && _MSC_VER <= 1800
  #define AUTO_ALIGNAS(n) __declspec(align(n))
#else
  #define AUTO_ALIGNAS alignas
#endif

/*********************
 * alignof has similar support, but has a strange name in older versions
 *********************/
#if defined(_MSC_VER) && _MSC_VER <= 1800
  #define AUTO_ALIGNOF __alignof
#else
  #define AUTO_ALIGNOF alignof
#endif

/*********************
 * Location of the unordered_set header
 *********************/
#if defined(__APPLE__) && !defined(_LIBCPP_VERSION)
  #define STL_UNORDERED_SET <autowiring/C++11/tr1_unordered_set.h>
  #define STL_UNORDERED_MAP <autowiring/C++11/tr1_unordered_map.h>
#else
  #define STL_UNORDERED_SET <unordered_set>
  #define STL_UNORDERED_MAP <unordered_map>
#endif

/*********************
* initializer_list header
*********************/
#if IS_CLANG
  #define HAS_INITIALIZER_LIST __has_feature(cxx_generalized_initializers)
  #if HAS_INITIALIZER_LIST
    #define INITIALIZER_LIST_HEADER <initializer_list>
  #else
  #define INITIALIZER_LIST_HEADER <autowiring/C++11/empty_file.h>
  #endif
#else
  #define HAS_INITIALIZER_LIST 1
  #define INITIALIZER_LIST_HEADER <initializer_list>
#endif

/*********************
 * Check override keyword availability
 *********************/
#if __cplusplus < 201103 && !defined(_MSC_VER)
  #define override
#endif

/*********************
 * static_assert availability
 *********************/
#if CLANG_CHECK(2, 9) || GCC_CHECK(4, 3)
  #define HAS_STATIC_ASSERT 1
#elif _MSC_VER >= 1500
  #define HAS_STATIC_ASSERT 1
#else
  #define HAS_STATIC_ASSERT 0
#endif

#if !HAS_STATIC_ASSERT || !STL11_ALLOWED
  // Static assert completely disabled if it's not available
  #define static_assert(...)
#endif

#if defined(_MSC_VER) || !IS_CLANG
  #define AUTOWIRE_cxx_override_control 1
#endif

/*********************
 * system error availability
 *********************/
#if STL11_ALLOWED
  #define SYSTEM_ERROR_HEADER <system_error>
#else
  #define SYSTEM_ERROR_HEADER <autowiring/C++11/boost_system_error.h>
#endif

/*********************
 * future availability
 *********************/
#if (_MSC_VER >= 1700 || (STL11_ALLOWED)) && (!__ANDROID__ || (GCC_CHECK(4, 9) && __aarch64__))
  #define FUTURE_HEADER <future>
#else
  // As of NDK r10, we still don't have an implementation of "future" for Android
  #define FUTURE_HEADER <autowiring/C++11/boost_future.h>
#endif

/**
 * type_index support
 */
// Supported natively on any platform except Mac/libstdc++
#if STL11_ALLOWED
  #define TYPE_INDEX_HEADER <typeindex>
#else
  #define TYPE_INDEX_HEADER <autowiring/C++11/type_index.h>
#endif

/*********************
 * Decide what version of type_traits we are going to use
 *********************/
#if _MSC_VER >= 1500
  #define TYPE_TRAITS_AVAILABLE 1
#elif __GXX_EXPERIMENTAL_CXX0X__ && STL11_ALLOWED
  #define TYPE_TRAITS_AVAILABLE 1
#else
  #define TYPE_TRAITS_AVAILABLE 0
#endif

#if TYPE_TRAITS_AVAILABLE
  #define TYPE_TRAITS_HEADER <type_traits>
#else
  #define TYPE_TRAITS_HEADER <autowiring/C++11/boost_type_traits.h>
#endif

/*********************
 * Decide what version of shared_ptr we are going to use
 *********************/

// Shared pointer available in VS10
#if _MSC_VER >= 1500
  #define SHARED_PTR_IN_STL 1
#elif __cplusplus > 199711L || __GXX_EXPERIMENTAL_CXX0X__
  #define SHARED_PTR_IN_STL 1
#else
  #define SHARED_PTR_IN_STL 0
#endif

#if SHARED_PTR_IN_STL && STL11_ALLOWED
  #define MEMORY_HEADER <autowiring/C++11/memory.h>
#else
  #define MEMORY_HEADER <autowiring/C++11/memory_nostl11.h>
#endif

// Nullptr_t has odd availability
#ifdef _MSC_VER
  #define HAS_NULLPTR_T 1
#elif IS_CLANG
  #define HAS_NULLPTR_T (STL11_ALLOWED && __has_feature(cxx_nullptr))
#elif __cplusplus > 199711L
  #define HAS_NULLPTR_T 1
#else
  // No idea--better safe than sorry!
  #define HAS_NULLPTR_T 1
#endif

#if ! HAS_NULLPTR_T
  // Have to provide our own dummy type, then, there's no header for this one
  namespace std { typedef decltype(nullptr) nullptr_t; }
#endif


/*********************
 * Specific support for is_constructible
 *********************/
#ifdef _MSC_VER
  #define AUTOWIRE_cxx_is_constructible 1
#elif IS_CLANG
  #define AUTOWIRE_cxx_is_constructible STL11_ALLOWED
#else
  #define AUTOWIRE_cxx_is_constructible 1
#endif

/*********************
 * noexcept support
 *********************/
#ifdef _MSC_VER
  #define AUTOWIRE_cxx_noexcept 0
  #define NOEXCEPT(x)
#else
  #define AUTOWIRE_cxx_noexcept 1
  #define NOEXCEPT(x) x noexcept
#endif

/*********************
 * Lambdas?
 *********************/
#if _MSC_VER >= 1500
  #define LAMBDAS_AVAILABLE 1
#elif CLANG_CHECK(3, 2)
  // Only available if we're told we're using at least C++11
  #if __cplusplus >= 201103L
    #define LAMBDAS_AVAILABLE 1
  #else
    #define LAMBDAS_AVAILABLE 0
  #endif
#elif __GXX_EXPERIMENTAL_CXX0X__
  #define LAMBDAS_AVAILABLE 1
#endif

#if LAMBDAS_AVAILABLE && STL11_ALLOWED
  #define FUNCTIONAL_HEADER <functional>
  #define _WEBSOCKETPP_CPP11_FUNCTIONAL_
#else
  #define FUNCTIONAL_HEADER <autowiring/C++11/boost_functional.h>
#endif

#ifndef LAMBDAS_AVAILABLE
  #define LAMBDAS_AVAILABLE 0
#endif

/*********************
* Arrays?
*********************/
#if _MSC_VER >= 1500
#define ARRAYS_AVAILABLE 1
#elif __GXX_EXPERIMENTAL_CXX0X__ && STL11_ALLOWED
#define ARRAYS_AVAILABLE 1
#else
#define ARRAYS_AVAILABLE 0
#endif

#if ARRAYS_AVAILABLE
#define ARRAY_HEADER <array>
#else
#define ARRAY_HEADER <autowiring/C++11/boost_array.h>
#endif

/**
 * Standard language features
 **/
#define _WEBSOCKETPP_RVALUE_REFERENCES_
#define _WEBSOCKETPP_DELETED_FUNCTIONS_

/**
 * R-value reference check
 */
#if STL11_ALLOWED
  #define RVALUE_HEADER <memory>
  #define _WEBSOCKETPP_CPP11_MEMORY_
#else
  // Remove literal references in order to fix another missing header problem on *nix
  // Defined by default on C++11 Clang
  #if !defined(__APPLE__) || __cplusplus >= 201103L
    #define BOOST_NO_UNICODE_LITERALS 1
  #endif
  #define RVALUE_HEADER <autowiring/C++11/boost_rvalue.h>
#endif

/**
 * Atomic
 */
#if STL11_ALLOWED
  #define ATOMIC_HEADER <atomic>
#else
  #define ATOMIC_HEADER <autowiring/C++11/boost_atomic.h>
#endif

/**
 * Tuple
 */
#if STL11_ALLOWED
  #define STL_TUPLE_HEADER <tuple>
#else
  #define STL_TUPLE_HEADER <autowiring/C++11/boost_tuple.h>
#endif

 /**
 * Mutex
 */
#if STL11_ALLOWED
  #define MUTEX_HEADER <autowiring/C++11/mutex.h>
#else
  #define MUTEX_HEADER <autowiring/C++11/boost_mutex.h>
#endif

/**
 * Thread
 */
#if STL11_ALLOWED
  #define THREAD_HEADER <thread>
  #define _WEBSOCKETPP_CPP11_THREAD_
#else
  #define THREAD_HEADER <autowiring/C++11/boost_thread.h>
#endif

 /**
 * Chrono
 */
#define CHRONO_HEADER <autowiring/C++11/chrono_with_profiling_clock.h>

 /**
 * Utility
 */
#if STL11_ALLOWED
  #define UTILITY_HEADER <utility>
#else
  #define UTILITY_HEADER <autowiring/C++11/boost_utility.h>
#endif

/**
 * System error
 */
#if STL11_ALLOWED
  #define _WEBSOCKETPP_CPP11_SYSTEM_ERROR_
#endif

/**
 * Filesystem
 */
#define FILESYSTEM_HEADER <autowiring/C++11/filesystem.h>

/*********************
 * Deprecation convenience macro
 *********************/
#if !defined(_DEBUG) && !defined(AUTOWIRING_NO_DEPRECATE)
  #ifdef _MSC_VER
    #define DEPRECATED(signature, msg) __declspec(deprecated(msg)) signature
    #define DEPRECATED_CLASS(classname, msg) __declspec(deprecated(msg)) classname
    #define DEPRECATED_MEMBER(member, msg) member
  #else
    #define DEPRECATED(signature, msg) signature __attribute__((deprecated))
    #define DEPRECATED_CLASS(classname, msg) classname
    #define DEPRECATED_MEMBER(member, msg) DEPRECATED(member, msg)
  #endif
#else
  #define DEPRECATED(signature, msg) signature
  #define DEPRECATED_CLASS(classname, msg) classname
  #define DEPRECATED_MEMBER(member, msg) member
#endif


/*********************
 * Enum forward declaration convenience macro - VS2010 has a bad implementation
 *********************/
#if defined(_MSC_VER) && _MSC_VER <= 1600
#define DECLARE_ENUM(x,type) enum x
#else
#define DECLARE_ENUM(x,type) enum x : type
#endif

/*****
 * Aligned memory allocation
 *****/
#ifdef _MSC_VER
  #define ALIGNED_ALLOC_HEADER <malloc.h>
  #define cpp11_aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
  #define cpp11_aligned_free _aligned_free
#elif defined(__APPLE__) && defined(__MACH__)
  // On Mac, memory is already 16-byte aligned
  #define ALIGNED_ALLOC_HEADER <stdlib.h>
  #define cpp11_aligned_alloc(alignment, size) malloc(size)
  #define cpp11_aligned_free free
#else
  #define ALIGNED_ALLOC_HEADER <malloc.h>
  #define cpp11_aligned_alloc memalign
  #define cpp11_aligned_free free
#endif
