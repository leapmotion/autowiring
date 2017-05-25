/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_OS_UNIX_H
#define AUTOBOOST_PREDEF_OS_UNIX_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_OS_UNIX`]

[@http://en.wikipedia.org/wiki/Unix Unix Environment] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`unix`] [__predef_detection__]]
    [[`__unix`] [__predef_detection__]]
    [[`_XOPEN_SOURCE`] [__predef_detection__]]
    [[`_POSIX_SOURCE`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_OS_UNIX AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(unix) || defined(__unix) || \
    defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#   undef AUTOBOOST_OS_UNIX
#   define AUTOBOOST_OS_UNIX AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_OS_UNIX
#   define AUTOBOOST_OS_UNIX_AVAILABLE
#endif

#define AUTOBOOST_OS_UNIX_NAME "Unix Environment"

/*`
[heading `AUTOBOOST_OS_SVR4`]

[@http://en.wikipedia.org/wiki/UNIX_System_V SVR4 Environment] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__sysv__`] [__predef_detection__]]
    [[`__SVR4`] [__predef_detection__]]
    [[`__svr4__`] [__predef_detection__]]
    [[`_SYSTYPE_SVR4`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_OS_SVR4 AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__sysv__) || defined(__SVR4) || \
    defined(__svr4__) || defined(_SYSTYPE_SVR4)
#   undef AUTOBOOST_OS_SVR4
#   define AUTOBOOST_OS_SVR4 AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_OS_SVR4
#   define AUTOBOOST_OS_SVR4_AVAILABLE
#endif

#define AUTOBOOST_OS_SVR4_NAME "SVR4 Environment"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_OS_UNIX,AUTOBOOST_OS_UNIX_NAME)
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_OS_SVR4,AUTOBOOST_OS_SVR4_NAME)
