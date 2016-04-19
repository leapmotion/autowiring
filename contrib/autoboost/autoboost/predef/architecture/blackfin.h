/*
Copyright Rene Rivera 2013-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_ARCHITECTURE_BLACKFIN_H
#define AUTOBOOST_PREDEF_ARCHITECTURE_BLACKFIN_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_ARCH_BLACKFIN`]

Blackfin Processors from Analog Devices.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__bfin__`] [__predef_detection__]]
    [[`__BFIN__`] [__predef_detection__]]
    [[`bfin`] [__predef_detection__]]
    [[`BFIN`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_ARCH_BLACKFIN AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__bfin__) || defined(__BFIN__) || \
    defined(bfin) || defined(BFIN)
#   undef AUTOBOOST_ARCH_BLACKFIN
#   define AUTOBOOST_ARCH_BLACKFIN AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_ARCH_BLACKFIN
#   define AUTOBOOST_ARCH_BLACKFIN_AVAILABLE
#endif

#define AUTOBOOST_ARCH_BLACKFIN_NAME "Blackfin"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_ARCH_BLACKFIN,AUTOBOOST_ARCH_BLACKFIN_NAME)
