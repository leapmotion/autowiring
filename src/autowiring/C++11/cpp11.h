// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once

// The reason this header exists is due to the asymmetric availability of C++11 on our
// various compiler targets.

#define STL11_ALLOWED 1

#define IS_CLANG defined(__clang_major__)
#define CLANG_CHECK(maj, min) (__clang_major__ == maj && __clang_minor__ >= min || __clang_major__ > maj)
#define GCC_CHECK(maj, min) (__GNUC__ == maj && __GNUC_MINOR__  >= min || __GNUC__ > maj)

#if IS_CLANG && !CLANG_CHECK(3, 6)
  #error Autowiring is not supported on clang 3.5 or older
#endif

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
#define STL_UNORDERED_SET <unordered_set>
#define STL_UNORDERED_MAP <unordered_map>

/*********************
* initializer_list header
*********************/
#define HAS_INITIALIZER_LIST 1
#define INITIALIZER_LIST_HEADER <initializer_list>

/*********************
 * Check override keyword availability
 *********************/
#if __cplusplus < 201103 && !defined(_MSC_VER)
  #define override
#endif

/*********************
 * static_assert availability
 *********************/
#if IS_CLANG || GCC_CHECK(4, 3)
  #define HAS_STATIC_ASSERT 1
#elif _MSC_VER >= 1500
  #define HAS_STATIC_ASSERT 1
#else
  #define HAS_STATIC_ASSERT 0
#endif

#if !HAS_STATIC_ASSERT
  // Static assert completely disabled if it's not available
  #define static_assert(...)
#endif

#if defined(_MSC_VER) || !IS_CLANG
  #define AUTOWIRE_cxx_override_control 1
#endif

/*********************
 * system error availability
 *********************/
#define SYSTEM_ERROR_HEADER <system_error>

/*********************
 * future availability
 *********************/
#if (!__ANDROID__ || (GCC_CHECK(4, 9) && __aarch64__))
  #define FUTURE_HEADER <future>
#else
  // As of NDK r10, we still don't have an implementation of "future" for Android
  #define FUTURE_HEADER <autowiring/C++11/boost_future.h>
#endif

/**
 * type_index support
 */
#define TYPE_INDEX_HEADER <typeindex>

/*********************
 * Decide what version of type_traits we are going to use
 *********************/
#if _MSC_VER >= 1500
  #define TYPE_TRAITS_AVAILABLE 1
#elif __GXX_EXPERIMENTAL_CXX0X__
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

#if SHARED_PTR_IN_STL
  #define MEMORY_HEADER <autowiring/C++11/memory.h>
#else
  #define MEMORY_HEADER <autowiring/C++11/memory_nostl11.h>
#endif

#define HAS_NULLPTR_T 1

/*********************
 * Specific support for is_constructible
 *********************/
#define AUTOWIRE_cxx_is_constructible 1

/*********************
 * noexcept support
 *********************/
#ifdef _MSC_VER
  #if _MSC_VER >= 1900
    #define AUTO_NOEXCEPT noexcept
  #else
    #define AUTO_NOEXCEPT throw()
  #endif
#else
  #if __has_feature(cxx_noexcept)
    #define AUTO_NOEXCEPT noexcept
  #else
    #define AUTO_NOEXCEPT
  #endif
#endif

/*********************
 * Lambdas?
 *********************/
#if _MSC_VER >= 1500
  #define LAMBDAS_AVAILABLE 1
#elif IS_CLANG
  #define LAMBDAS_AVAILABLE 1
#elif __GXX_EXPERIMENTAL_CXX0X__
  #define LAMBDAS_AVAILABLE 1
#endif

#if LAMBDAS_AVAILABLE
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
#elif __GXX_EXPERIMENTAL_CXX0X__
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
#define RVALUE_HEADER <memory>
#define _WEBSOCKETPP_CPP11_MEMORY_

/**
 * Atomic
 */
#define ATOMIC_HEADER <atomic>

/**
 * Tuple
 */
#define STL_TUPLE_HEADER <tuple>

 /**
 * Mutex
 */
#define MUTEX_HEADER <autowiring/C++11/mutex.h>

/**
 * Thread
 */
#define THREAD_HEADER <thread>
#define _WEBSOCKETPP_CPP11_THREAD_

 /**
 * Chrono
 */
#define CHRONO_HEADER <autowiring/C++11/chrono_with_profiling_clock.h>

 /**
 * Utility
 */
#define UTILITY_HEADER <utility>

/**
 * System error
 */
#define _WEBSOCKETPP_CPP11_SYSTEM_ERROR_

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
