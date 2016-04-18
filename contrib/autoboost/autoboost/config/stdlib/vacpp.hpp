//  (C) Copyright John Maddock 2001 - 2002.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

#if __IBMCPP__ <= 501
#  define AUTOBOOST_NO_STD_ALLOCATOR
#endif

#define AUTOBOOST_HAS_MACRO_USE_FACET
#define AUTOBOOST_NO_STD_MESSAGES

// Apple doesn't seem to reliably defined a *unix* macro
#if !defined(CYGWIN) && (  defined(__unix__)  \
                        || defined(__unix)    \
                        || defined(unix)      \
                        || defined(__APPLE__) \
                        || defined(__APPLE)   \
                        || defined(APPLE))
#  include <unistd.h>
#endif

//  C++0x headers not yet implemented
//
#  define AUTOBOOST_NO_CXX11_HDR_ARRAY
#  define AUTOBOOST_NO_CXX11_HDR_CHRONO
#  define AUTOBOOST_NO_CXX11_HDR_CODECVT
#  define AUTOBOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#  define AUTOBOOST_NO_CXX11_HDR_FORWARD_LIST
#  define AUTOBOOST_NO_CXX11_HDR_FUTURE
#  define AUTOBOOST_NO_CXX11_HDR_INITIALIZER_LIST
#  define AUTOBOOST_NO_CXX11_HDR_MUTEX
#  define AUTOBOOST_NO_CXX11_HDR_RANDOM
#  define AUTOBOOST_NO_CXX11_HDR_RATIO
#  define AUTOBOOST_NO_CXX11_HDR_REGEX
#  define AUTOBOOST_NO_CXX11_HDR_SYSTEM_ERROR
#  define AUTOBOOST_NO_CXX11_HDR_THREAD
#  define AUTOBOOST_NO_CXX11_HDR_TUPLE
#  define AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS
#  define AUTOBOOST_NO_CXX11_HDR_TYPEINDEX
#  define AUTOBOOST_NO_CXX11_HDR_UNORDERED_MAP
#  define AUTOBOOST_NO_CXX11_HDR_UNORDERED_SET
#  define AUTOBOOST_NO_CXX11_NUMERIC_LIMITS
#  define AUTOBOOST_NO_CXX11_ALLOCATOR
#  define AUTOBOOST_NO_CXX11_ATOMIC_SMART_PTR
#  define AUTOBOOST_NO_CXX11_SMART_PTR
#  define AUTOBOOST_NO_CXX11_HDR_FUNCTIONAL
#  define AUTOBOOST_NO_CXX11_HDR_ATOMIC
#  define AUTOBOOST_NO_CXX11_STD_ALIGN
#  define AUTOBOOST_NO_CXX11_ADDRESSOF

#if defined(__has_include)
#if !__has_include(<shared_mutex>)
#  define AUTOBOOST_NO_CXX14_HDR_SHARED_MUTEX
#elif __cplusplus < 201402
#  define AUTOBOOST_NO_CXX14_HDR_SHARED_MUTEX
#endif
#else
#  define AUTOBOOST_NO_CXX14_HDR_SHARED_MUTEX
#endif

#define AUTOBOOST_STDLIB "Visual Age default standard library"
