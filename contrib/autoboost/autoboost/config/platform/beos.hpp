//  (C) Copyright John Maddock 2001.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  BeOS specific config options:

#define AUTOBOOST_PLATFORM "BeOS"

#define AUTOBOOST_NO_CWCHAR
#define AUTOBOOST_NO_CWCTYPE
#define AUTOBOOST_HAS_UNISTD_H

#define AUTOBOOST_HAS_BETHREADS

#ifndef AUTOBOOST_DISABLE_THREADS
#  define AUTOBOOST_HAS_THREADS
#endif

// boilerplate code:
#include <autoboost/config/posix_features.hpp>



