/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_OS_WINDOWS_H
#define AUTOBOOST_PREDEF_OS_WINDOWS_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_OS_WINDOWS`]

[@http://en.wikipedia.org/wiki/Category:Microsoft_Windows Microsoft Windows] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_WIN32`] [__predef_detection__]]
    [[`_WIN64`] [__predef_detection__]]
    [[`__WIN32__`] [__predef_detection__]]
    [[`__TOS_WIN__`] [__predef_detection__]]
    [[`__WINDOWS__`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_OS_WINDOWS AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(AUTOBOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(_WIN32) || defined(_WIN64) || \
    defined(__WIN32__) || defined(__TOS_WIN__) || \
    defined(__WINDOWS__) \
    )
#   undef AUTOBOOST_OS_WINDOWS
#   define AUTOBOOST_OS_WINDOWS AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_OS_WINDOWS
#   define AUTOBOOST_OS_WINDOWS_AVAILABLE
#   include <autoboost/predef/detail/os_detected.h>
#endif

#define AUTOBOOST_OS_WINDOWS_NAME "Microsoft Windows"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_OS_WINDOWS,AUTOBOOST_OS_WINDOWS_NAME)
