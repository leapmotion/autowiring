/*
Copyright (c) Microsoft Corporation 2014
Copyright Rene Rivera 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_PLAT_WINDOWS_DESKTOP_H
#define AUTOBOOST_PREDEF_PLAT_WINDOWS_DESKTOP_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>
#include <autoboost/predef/os/windows.h>

/*`
[heading `AUTOBOOST_PLAT_WINDOWS_DESKTOP`]

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`!WINAPI_FAMILY`] [__predef_detection__]]
    [[`WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_PLAT_WINDOWS_DESKTOP AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if AUTOBOOST_OS_WINDOWS && \
    ( !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) )
#   undef AUTOBOOST_PLAT_WINDOWS_DESKTOP
#   define AUTOBOOST_PLAT_WINDOWS_DESKTOP AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_PLAT_WINDOWS_DESKTOP
#   define AUTOBOOST_PLAT_WINDOWS_DESKTOP_AVAILABLE
#   include <autoboost/predef/detail/platform_detected.h>
#endif

#define AUTOBOOST_PLAT_WINDOWS_DESKTOP_NAME "Windows Desktop"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_PLAT_WINDOWS_DESKTOP,AUTOBOOST_PLAT_WINDOWS_DESKTOP_NAME)
