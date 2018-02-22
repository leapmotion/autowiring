/*
Copyright Rene Rivera 2012-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_OS_BSD_DRAGONFLY_H
#define AUTOBOOST_PREDEF_OS_BSD_DRAGONFLY_H

#include <autoboost/predef/os/bsd.h>

/*`
[heading `AUTOBOOST_OS_BSD_DRAGONFLY`]

[@http://en.wikipedia.org/wiki/DragonFly_BSD DragonFly BSD] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__DragonFly__`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_OS_BSD_DRAGONFLY AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(AUTOBOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__DragonFly__) \
    )
#   ifndef AUTOBOOST_OS_BSD_AVAILABLE
#       define AUTOBOOST_OS_BSD AUTOBOOST_VERSION_NUMBER_AVAILABLE
#       define AUTOBOOST_OS_BSD_AVAILABLE
#   endif
#   undef AUTOBOOST_OS_BSD_DRAGONFLY
#   if defined(__DragonFly__)
#       define AUTOBOOST_OS_DRAGONFLY_BSD AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if AUTOBOOST_OS_BSD_DRAGONFLY
#   define AUTOBOOST_OS_BSD_DRAGONFLY_AVAILABLE
#   include <autoboost/predef/detail/os_detected.h>
#endif

#define AUTOBOOST_OS_BSD_DRAGONFLY_NAME "DragonFly BSD"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_OS_BSD_DRAGONFLY,AUTOBOOST_OS_BSD_DRAGONFLY_NAME)
