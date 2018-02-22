/*
Copyright Rene Rivera 2012-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_OS_BSD_BSDI_H
#define AUTOBOOST_PREDEF_OS_BSD_BSDI_H

#include <autoboost/predef/os/bsd.h>

/*`
[heading `AUTOBOOST_OS_BSD_BSDI`]

[@http://en.wikipedia.org/wiki/BSD/OS BSDi BSD/OS] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__bsdi__`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_OS_BSD_BSDI AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(AUTOBOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__bsdi__) \
    )
#   ifndef AUTOBOOST_OS_BSD_AVAILABLE
#       define AUTOBOOST_OS_BSD AUTOBOOST_VERSION_NUMBER_AVAILABLE
#       define AUTOBOOST_OS_BSD_AVAILABLE
#   endif
#   undef AUTOBOOST_OS_BSD_BSDI
#   define AUTOBOOST_OS_BSD_BSDI AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_OS_BSD_BSDI
#   define AUTOBOOST_OS_BSD_BSDI_AVAILABLE
#   include <autoboost/predef/detail/os_detected.h>
#endif

#define AUTOBOOST_OS_BSD_BSDI_NAME "BSDi BSD/OS"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_OS_BSD_BSDI,AUTOBOOST_OS_BSD_BSDI_NAME)
