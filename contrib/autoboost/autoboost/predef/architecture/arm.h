/*
Copyright Rene Rivera 2008-2015
Copyright Franz Detro 2014
Copyright (c) Microsoft Corporation 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_ARCHITECTURE_ARM_H
#define AUTOBOOST_PREDEF_ARCHITECTURE_ARM_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_ARCH_ARM`]

[@http://en.wikipedia.org/wiki/ARM_architecture ARM] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__arm__`] [__predef_detection__]]
    [[`__arm64`] [__predef_detection__]]
    [[`__thumb__`] [__predef_detection__]]
    [[`__TARGET_ARCH_ARM`] [__predef_detection__]]
    [[`__TARGET_ARCH_THUMB`] [__predef_detection__]]
    [[`_M_ARM`] [__predef_detection__]]

    [[`__arm64`] [8.0.0]]
    [[`__TARGET_ARCH_ARM`] [V.0.0]]
    [[`__TARGET_ARCH_THUMB`] [V.0.0]]
    [[`_M_ARM`] [V.0.0]]
    ]
 */

#define AUTOBOOST_ARCH_ARM AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__arm__) || defined(__arm64) || defined(__thumb__) || \
    defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || \
    defined(_M_ARM)
#   undef AUTOBOOST_ARCH_ARM
#   if !defined(AUTOBOOST_ARCH_ARM) && defined(__arm64)
#       define AUTOBOOST_ARCH_ARM AUTOBOOST_VERSION_NUMBER(8,0,0)
#   endif
#   if !defined(AUTOBOOST_ARCH_ARM) && defined(__TARGET_ARCH_ARM)
#       define AUTOBOOST_ARCH_ARM AUTOBOOST_VERSION_NUMBER(__TARGET_ARCH_ARM,0,0)
#   endif
#   if !defined(AUTOBOOST_ARCH_ARM) && defined(__TARGET_ARCH_THUMB)
#       define AUTOBOOST_ARCH_ARM AUTOBOOST_VERSION_NUMBER(__TARGET_ARCH_THUMB,0,0)
#   endif
#   if !defined(AUTOBOOST_ARCH_ARM) && defined(_M_ARM)
#       define AUTOBOOST_ARCH_ARM AUTOBOOST_VERSION_NUMBER(_M_ARM,0,0)
#   endif
#   if !defined(AUTOBOOST_ARCH_ARM)
#       define AUTOBOOST_ARCH_ARM AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if AUTOBOOST_ARCH_ARM
#   define AUTOBOOST_ARCH_ARM_AVAILABLE
#endif

#define AUTOBOOST_ARCH_ARM_NAME "ARM"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_ARCH_ARM,AUTOBOOST_ARCH_ARM_NAME)
