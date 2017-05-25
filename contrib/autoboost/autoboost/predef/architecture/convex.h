/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_ARCHITECTURE_CONVEX_H
#define AUTOBOOST_PREDEF_ARCHITECTURE_CONVEX_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_ARCH_CONVEX`]

[@http://en.wikipedia.org/wiki/Convex_Computer Convex Computer] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__convex__`] [__predef_detection__]]

    [[`__convex_c1__`] [1.0.0]]
    [[`__convex_c2__`] [2.0.0]]
    [[`__convex_c32__`] [3.2.0]]
    [[`__convex_c34__`] [3.4.0]]
    [[`__convex_c38__`] [3.8.0]]
    ]
 */

#define AUTOBOOST_ARCH_CONVEX AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__convex__)
#   undef AUTOBOOST_ARCH_CONVEX
#   if !defined(AUTOBOOST_ARCH_CONVEX) && defined(__convex_c1__)
#       define AUTOBOOST_ARCH_CONVEX AUTOBOOST_VERSION_NUMBER(1,0,0)
#   endif
#   if !defined(AUTOBOOST_ARCH_CONVEX) && defined(__convex_c2__)
#       define AUTOBOOST_ARCH_CONVEX AUTOBOOST_VERSION_NUMBER(2,0,0)
#   endif
#   if !defined(AUTOBOOST_ARCH_CONVEX) && defined(__convex_c32__)
#       define AUTOBOOST_ARCH_CONVEX AUTOBOOST_VERSION_NUMBER(3,2,0)
#   endif
#   if !defined(AUTOBOOST_ARCH_CONVEX) && defined(__convex_c34__)
#       define AUTOBOOST_ARCH_CONVEX AUTOBOOST_VERSION_NUMBER(3,4,0)
#   endif
#   if !defined(AUTOBOOST_ARCH_CONVEX) && defined(__convex_c38__)
#       define AUTOBOOST_ARCH_CONVEX AUTOBOOST_VERSION_NUMBER(3,8,0)
#   endif
#   if !defined(AUTOBOOST_ARCH_CONVEX)
#       define AUTOBOOST_ARCH_CONVEX AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if AUTOBOOST_ARCH_CONVEX
#   define AUTOBOOST_ARCH_CONVEX_AVAILABLE
#endif

#define AUTOBOOST_ARCH_CONVEX_NAME "Convex Computer"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_ARCH_CONVEX,AUTOBOOST_ARCH_CONVEX_NAME)
