/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_ARCHITECTURE_SYS390_H
#define AUTOBOOST_PREDEF_ARCHITECTURE_SYS390_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_ARCH_SYS390`]

[@http://en.wikipedia.org/wiki/System/390 System/390] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__s390__`] [__predef_detection__]]
    [[`__s390x__`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_ARCH_SYS390 AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__s390__) || defined(__s390x__)
#   undef AUTOBOOST_ARCH_SYS390
#   define AUTOBOOST_ARCH_SYS390 AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_ARCH_SYS390
#   define AUTOBOOST_ARCH_SYS390_AVAILABLE
#endif

#define AUTOBOOST_ARCH_SYS390_NAME "System/390"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_ARCH_SYS390,AUTOBOOST_ARCH_SYS390_NAME)
