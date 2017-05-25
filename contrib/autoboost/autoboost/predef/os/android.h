/*
Copyright Rene Rivera 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_OS_ADROID_H
#define AUTOBOOST_PREDEF_OS_ADROID_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_OS_ANDROID`]

[@http://en.wikipedia.org/wiki/Android_%28operating_system%29 Android] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__ANDROID__`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_OS_ANDROID AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(AUTOBOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__ANDROID__) \
    )
#   undef AUTOBOOST_OS_ANDROID
#   define AUTOBOOST_OS_ANDROID AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_OS_ANDROID
#   define AUTOBOOST_OS_ANDROID_AVAILABLE
#   include <autoboost/predef/detail/os_detected.h>
#endif

#define AUTOBOOST_OS_ANDROID_NAME "Android"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_OS_ANDROID,AUTOBOOST_OS_ANDROID_NAME)
