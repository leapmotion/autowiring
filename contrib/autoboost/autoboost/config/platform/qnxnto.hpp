//  (C) Copyright Jim Douglas 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  QNX specific config options:

#define AUTOBOOST_PLATFORM "QNX"

#define AUTOBOOST_HAS_UNISTD_H
#include <autoboost/config/posix_features.hpp>

// QNX claims XOpen version 5 compatibility, but doesn't have an nl_types.h
// or log1p and expm1:
#undef  AUTOBOOST_HAS_NL_TYPES_H
#undef  AUTOBOOST_HAS_LOG1P
#undef  AUTOBOOST_HAS_EXPM1

#define AUTOBOOST_HAS_PTHREADS
#define AUTOBOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE

#define AUTOBOOST_HAS_GETTIMEOFDAY
#define AUTOBOOST_HAS_CLOCK_GETTIME
#define AUTOBOOST_HAS_NANOSLEEP





