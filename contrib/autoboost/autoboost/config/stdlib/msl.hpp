//  (C) Copyright John Maddock 2001.
//  (C) Copyright Darin Adler 2001.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Metrowerks standard library:

#ifndef __MSL_CPP__
#  include <autoboost/config/no_tr1/utility.hpp>
#  ifndef __MSL_CPP__
#     error This is not the MSL standard library!
#  endif
#endif

#if __MSL_CPP__ >= 0x6000  // Pro 6
#  define AUTOBOOST_HAS_HASH
#  define AUTOBOOST_STD_EXTENSION_NAMESPACE Metrowerks
#endif
#define AUTOBOOST_HAS_SLIST

#if __MSL_CPP__ < 0x6209
#  define AUTOBOOST_NO_STD_MESSAGES
#endif

// check C lib version for <stdint.h>
#include <cstddef>

#if defined(__MSL__) && (__MSL__ >= 0x5000)
#  define AUTOBOOST_HAS_STDINT_H
#  if !defined(__PALMOS_TRAPS__)
#    define AUTOBOOST_HAS_UNISTD_H
#  endif
   // boilerplate code:
#  include <autoboost/config/posix_features.hpp>
#endif

#if defined(_MWMT) || _MSL_THREADSAFE
#  define AUTOBOOST_HAS_THREADS
#endif

#ifdef _MSL_NO_EXPLICIT_FUNC_TEMPLATE_ARG
#  define AUTOBOOST_NO_STD_USE_FACET
#  define AUTOBOOST_HAS_TWO_ARG_USE_FACET
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

// C++14 features
#  define AUTOBOOST_NO_CXX14_STD_EXCHANGE

// C++17 features
#  define AUTOBOOST_NO_CXX17_STD_APPLY
#  define AUTOBOOST_NO_CXX17_STD_INVOKE

#define AUTOBOOST_STDLIB "Metrowerks Standard Library version " AUTOBOOST_STRINGIZE(__MSL_CPP__)
