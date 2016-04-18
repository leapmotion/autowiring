//  (C) Copyright John Maddock 2001 - 2003.
//  (C) Copyright Jens Maurer 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  sun specific config options:

#define AUTOBOOST_PLATFORM "Sun Solaris"

#define AUTOBOOST_HAS_GETTIMEOFDAY

// boilerplate code:
#define AUTOBOOST_HAS_UNISTD_H
#include <autoboost/config/posix_features.hpp>

//
// pthreads don't actually work with gcc unless _PTHREADS is defined:
//
#if defined(__GNUC__) && defined(_POSIX_THREADS) && !defined(_PTHREADS)
# undef AUTOBOOST_HAS_PTHREADS
#endif

#define AUTOBOOST_HAS_STDINT_H
#define AUTOBOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
#define AUTOBOOST_HAS_LOG1P
#define AUTOBOOST_HAS_EXPM1


