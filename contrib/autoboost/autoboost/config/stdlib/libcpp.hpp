//  (C) Copyright Christopher Jefferson 2011.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  config for libc++
//  Might need more in here later.

#if !defined(_LIBCPP_VERSION)
#  include <ciso646>
#  if !defined(_LIBCPP_VERSION)
#      error "This is not libc++!"
#  endif
#endif

#define AUTOBOOST_STDLIB "libc++ version " AUTOBOOST_STRINGIZE(_LIBCPP_VERSION)

#define AUTOBOOST_HAS_THREADS

#ifdef _LIBCPP_HAS_NO_VARIADICS
#    define AUTOBOOST_NO_CXX11_HDR_TUPLE
#endif

// AUTOBOOST_NO_CXX11_ALLOCATOR should imply no support for the C++11
// allocator model. The C++11 allocator model requires a conforming
// std::allocator_traits which is only possible with C++11 template
// aliases since members rebind_alloc and rebind_traits require it.
#if defined(_LIBCPP_HAS_NO_TEMPLATE_ALIASES)
#    define AUTOBOOST_NO_CXX11_ALLOCATOR
#endif

#if __cplusplus < 201103
#  define AUTOBOOST_NO_CXX11_HDR_ARRAY
#  define AUTOBOOST_NO_CXX11_HDR_CODECVT
#  define AUTOBOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#  define AUTOBOOST_NO_CXX11_HDR_FORWARD_LIST
#  define AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#  define AUTOBOOST_NO_CXX11_HDR_MUTEX
#  define AUTOBOOST_NO_CXX11_HDR_RANDOM
#  define AUTOBOOST_NO_CXX11_HDR_RATIO
#  define AUTOBOOST_NO_CXX11_HDR_REGEX
#  define AUTOBOOST_NO_CXX11_HDR_SYSTEM_ERROR
#  define AUTOBOOST_NO_CXX11_HDR_THREAD
#  define AUTOBOOST_NO_CXX11_HDR_TUPLE
#  define AUTOBOOST_NO_CXX11_HDR_TYPEINDEX
#  define AUTOBOOST_NO_CXX11_HDR_UNORDERED_MAP
#  define AUTOBOOST_NO_CXX11_HDR_UNORDERED_SET
#  define AUTOBOOST_NO_CXX11_NUMERIC_LIMITS
#  define AUTOBOOST_NO_CXX11_ALLOCATOR
#  define AUTOBOOST_NO_CXX11_SMART_PTR
#  define AUTOBOOST_NO_CXX11_HDR_FUNCTIONAL
#  define AUTOBOOST_NO_CXX11_STD_ALIGN
#  define AUTOBOOST_NO_CXX11_ADDRESSOF
#endif

//
// These appear to be unusable/incomplete so far:
//
#  define AUTOBOOST_NO_CXX11_HDR_CHRONO
#  define AUTOBOOST_NO_CXX11_HDR_FUTURE
#  define AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS
#  define AUTOBOOST_NO_CXX11_ATOMIC_SMART_PTR
#  define AUTOBOOST_NO_CXX11_HDR_ATOMIC

// libc++ uses a non-standard messages_base
#define AUTOBOOST_NO_STD_MESSAGES

#if defined(__has_include)
#if !__has_include(<shared_mutex>)
#  define AUTOBOOST_NO_CXX14_HDR_SHARED_MUTEX
#elif __cplusplus <= 201103
#  define AUTOBOOST_NO_CXX14_HDR_SHARED_MUTEX
#endif
#elif __cplusplus < 201402
#  define AUTOBOOST_NO_CXX14_HDR_SHARED_MUTEX
#endif

//  --- end ---
