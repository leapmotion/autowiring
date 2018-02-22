//  (C) Copyright John Maddock 2001 - 2003.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  cygwin specific config options:

#define AUTOBOOST_PLATFORM "Cygwin"
#define AUTOBOOST_HAS_DIRENT_H
#define AUTOBOOST_HAS_LOG1P
#define AUTOBOOST_HAS_EXPM1

//
// Threading API:
// See if we have POSIX threads, if we do use them, otherwise
// revert to native Win threads.
#define AUTOBOOST_HAS_UNISTD_H
#include <unistd.h>
#if defined(_POSIX_THREADS) && (_POSIX_THREADS+0 >= 0) && !defined(AUTOBOOST_HAS_WINTHREADS)
#  define AUTOBOOST_HAS_PTHREADS
#  define AUTOBOOST_HAS_SCHED_YIELD
#  define AUTOBOOST_HAS_GETTIMEOFDAY
#  define AUTOBOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
//#  define AUTOBOOST_HAS_SIGACTION
#else
#  if !defined(AUTOBOOST_HAS_WINTHREADS)
#     define AUTOBOOST_HAS_WINTHREADS
#  endif
#  define AUTOBOOST_HAS_FTIME
#endif

//
// find out if we have a stdint.h, there should be a better way to do this:
//
#include <sys/types.h>
#ifdef _STDINT_H
#define AUTOBOOST_HAS_STDINT_H
#endif

/// Cygwin has no fenv.h
#define AUTOBOOST_NO_FENV_H

// boilerplate code:
#include <autoboost/config/posix_features.hpp>

//
// Cygwin lies about XSI conformance, there is no nl_types.h:
//
#ifdef AUTOBOOST_HAS_NL_TYPES_H
#  undef AUTOBOOST_HAS_NL_TYPES_H
#endif




