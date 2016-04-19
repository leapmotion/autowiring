/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_ARCHITECTURE_SYS370_H
#define AUTOBOOST_PREDEF_ARCHITECTURE_SYS370_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_ARCH_SYS370`]

[@http://en.wikipedia.org/wiki/System/370 System/370] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__370__`] [__predef_detection__]]
    [[`__THW_370__`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_ARCH_SYS370 AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__370__) || defined(__THW_370__)
#   undef AUTOBOOST_ARCH_SYS370
#   define AUTOBOOST_ARCH_SYS370 AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_ARCH_SYS370
#   define AUTOBOOST_ARCH_SYS370_AVAILABLE
#endif

#define AUTOBOOST_ARCH_SYS370_NAME "System/370"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_ARCH_SYS370,AUTOBOOST_ARCH_SYS370_NAME)
