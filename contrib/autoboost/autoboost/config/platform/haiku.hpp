//  (C) Copyright Jessica Hamilton 2014.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Haiku specific config options:

#define AUTOBOOST_PLATFORM "Haiku"

#define AUTOBOOST_HAS_UNISTD_H
#define AUTOBOOST_HAS_STDINT_H

#ifndef AUTOBOOST_DISABLE_THREADS
#  define AUTOBOOST_HAS_THREADS
#endif

#define AUTOBOOST_NO_CXX11_HDR_TYPE_TRAITS
#define AUTOBOOST_NO_CXX11_ATOMIC_SMART_PTR
#define AUTOBOOST_NO_CXX11_STATIC_ASSERT
#define AUTOBOOST_NO_CXX11_VARIADIC_MACROS

//
// thread API's not auto detected:
//
#define AUTOBOOST_HAS_SCHED_YIELD
#define AUTOBOOST_HAS_GETTIMEOFDAY

// boilerplate code:
#include <autoboost/config/posix_features.hpp>
