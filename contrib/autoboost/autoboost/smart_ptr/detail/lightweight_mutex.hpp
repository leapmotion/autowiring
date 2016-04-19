#ifndef AUTOBOOST_SMART_PTR_DETAIL_LIGHTWEIGHT_MUTEX_HPP_INCLUDED
#define AUTOBOOST_SMART_PTR_DETAIL_LIGHTWEIGHT_MUTEX_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  autoboost/detail/lightweight_mutex.hpp - lightweight mutex
//
//  Copyright (c) 2002, 2003 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  typedef <unspecified> autoboost::detail::lightweight_mutex;
//
//  autoboost::detail::lightweight_mutex is a header-only implementation of
//  a subset of the Mutex concept requirements:
//
//  http://www.boost.org/doc/html/threads/concepts.html#threads.concepts.Mutex
//
//  It maps to a CRITICAL_SECTION on Windows or a pthread_mutex on POSIX.
//

#include <autoboost/config.hpp>

#if !defined(AUTOBOOST_HAS_THREADS)
#  include <autoboost/smart_ptr/detail/lwm_nop.hpp>
#elif defined(AUTOBOOST_HAS_PTHREADS)
#  include <autoboost/smart_ptr/detail/lwm_pthreads.hpp>
#elif defined(AUTOBOOST_HAS_WINTHREADS) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#  include <autoboost/smart_ptr/detail/lwm_win32_cs.hpp>
#else
// Use #define AUTOBOOST_DISABLE_THREADS to avoid the error
#  error Unrecognized threading platform
#endif

#endif // #ifndef AUTOBOOST_SMART_PTR_DETAIL_LIGHTWEIGHT_MUTEX_HPP_INCLUDED
