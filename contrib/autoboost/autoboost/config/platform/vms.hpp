//  (C) Copyright Artyom Beilis 2010.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_CONFIG_PLATFORM_VMS_HPP
#define AUTOBOOST_CONFIG_PLATFORM_VMS_HPP

#define AUTOBOOST_PLATFORM "OpenVMS"

#undef  AUTOBOOST_HAS_STDINT_H
#define AUTOBOOST_HAS_UNISTD_H
#define AUTOBOOST_HAS_NL_TYPES_H
#define AUTOBOOST_HAS_GETTIMEOFDAY
#define AUTOBOOST_HAS_DIRENT_H
#define AUTOBOOST_HAS_PTHREADS
#define AUTOBOOST_HAS_NANOSLEEP
#define AUTOBOOST_HAS_CLOCK_GETTIME
#define AUTOBOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
#define AUTOBOOST_HAS_LOG1P
#define AUTOBOOST_HAS_EXPM1
#define AUTOBOOST_HAS_THREADS
#undef  AUTOBOOST_HAS_SCHED_YIELD

#endif
