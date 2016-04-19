/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_ARCHITECTURE_PYRAMID_H
#define AUTOBOOST_PREDEF_ARCHITECTURE_PYRAMID_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_ARCH_PYRAMID`]

Pyramid 9810 architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`pyr`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_ARCH_PYRAMID AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(pyr)
#   undef AUTOBOOST_ARCH_PYRAMID
#   define AUTOBOOST_ARCH_PYRAMID AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_ARCH_PYRAMID
#   define AUTOBOOST_ARCH_PYRAMID_AVAILABLE
#endif

#define AUTOBOOST_ARCH_PYRAMID_NAME "Pyramid 9810"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_ARCH_PYRAMID,AUTOBOOST_ARCH_PYRAMID_NAME)
