/*
Copyright Rene Rivera 2012-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_OS_BSD_FREE_H
#define AUTOBOOST_PREDEF_OS_BSD_FREE_H

#include <autoboost/predef/os/bsd.h>

/*`
[heading `AUTOBOOST_OS_BSD_FREE`]

[@http://en.wikipedia.org/wiki/Freebsd FreeBSD] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__FreeBSD__`] [__predef_detection__]]

    [[`__FreeBSD_version`] [V.R.P]]
    ]
 */

#define AUTOBOOST_OS_BSD_FREE AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(AUTOBOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__FreeBSD__) \
    )
#   ifndef AUTOBOOST_OS_BSD_AVAILABLE
#       define AUTOBOOST_OS_BSD AUTOBOOST_VERSION_NUMBER_AVAILABLE
#       define AUTOBOOST_OS_BSD_AVAILABLE
#   endif
#   undef AUTOBOOST_OS_BSD_FREE
#   if defined(__FreeBSD_version)
#       if __FreeBSD_version < 500000
#           define AUTOBOOST_OS_BSD_FREE \
                AUTOBOOST_PREDEF_MAKE_10_VRP000(__FreeBSD_version)
#       else
#           define AUTOBOOST_OS_BSD_FREE \
                AUTOBOOST_PREDEF_MAKE_10_VRR000(__FreeBSD_version)
#       endif
#   else
#       define AUTOBOOST_OS_BSD_FREE AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if AUTOBOOST_OS_BSD_FREE
#   define AUTOBOOST_OS_BSD_FREE_AVAILABLE
#   include <autoboost/predef/detail/os_detected.h>
#endif

#define AUTOBOOST_OS_BSD_FREE_NAME "Free BSD"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_OS_BSD_FREE,AUTOBOOST_OS_BSD_FREE_NAME)
