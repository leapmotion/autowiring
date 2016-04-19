/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_MINGW_H
#define AUTOBOOST_PREDEF_COMPILER_MINGW_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_PLAT_MINGW`]

[@http://en.wikipedia.org/wiki/MinGW MinGW] platform.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MINGW32__`] [__predef_detection__]]
    [[`__MINGW64__`] [__predef_detection__]]

    [[`__MINGW64_VERSION_MAJOR`, `__MINGW64_VERSION_MINOR`] [V.R.0]]
    [[`__MINGW32_VERSION_MAJOR`, `__MINGW32_VERSION_MINOR`] [V.R.0]]
    ]
 */

#define AUTOBOOST_PLAT_MINGW AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__MINGW32__) || defined(__MINGW64__)
#   include <_mingw.h>
#   if !defined(AUTOBOOST_PLAT_MINGW_DETECTION) && (defined(__MINGW64_VERSION_MAJOR) && defined(__MINGW64_VERSION_MINOR))
#       define AUTOBOOST_PLAT_MINGW_DETECTION \
            AUTOBOOST_VERSION_NUMBER(__MINGW64_VERSION_MAJOR,__MINGW64_VERSION_MINOR,0)
#   endif
#   if !defined(AUTOBOOST_PLAT_MINGW_DETECTION) && (defined(__MINGW32_VERSION_MAJOR) && defined(__MINGW32_VERSION_MINOR))
#       define AUTOBOOST_PLAT_MINGW_DETECTION \
            AUTOBOOST_VERSION_NUMBER(__MINGW32_MAJOR_VERSION,__MINGW32_MINOR_VERSION,0)
#   endif
#   if !defined(AUTOBOOST_PLAT_MINGW_DETECTION)
#       define AUTOBOOST_PLAT_MINGW_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_PLAT_MINGW_DETECTION
#   define AUTOBOOST_PLAT_MINGW_AVAILABLE
#   if defined(AUTOBOOST_PREDEF_DETAIL_PLAT_DETECTED)
#       define AUTOBOOST_PLAT_MINGW_EMULATED AUTOBOOST_PLAT_MINGW_DETECTION
#   else
#       undef AUTOBOOST_PLAT_MINGW
#       define AUTOBOOST_PLAT_MINGW AUTOBOOST_PLAT_MINGW_DETECTION
#   endif
#   include <autoboost/predef/detail/platform_detected.h>
#endif

#define AUTOBOOST_PLAT_MINGW_NAME "MinGW"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_PLAT_MINGW,AUTOBOOST_PLAT_MINGW_NAME)

#ifdef AUTOBOOST_PLAT_MINGW_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_PLAT_MINGW_EMULATED,AUTOBOOST_PLAT_MINGW_NAME)
#endif
