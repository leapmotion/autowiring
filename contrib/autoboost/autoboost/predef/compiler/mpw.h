/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_MPW_H
#define AUTOBOOST_PREDEF_COMPILER_MPW_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_MPW`]

[@http://en.wikipedia.org/wiki/Macintosh_Programmer%27s_Workshop MPW C++] compiler.
Version number available as major, and minor.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MRC__`] [__predef_detection__]]
    [[`MPW_C`] [__predef_detection__]]
    [[`MPW_CPLUS`] [__predef_detection__]]

    [[`__MRC__`] [V.R.0]]
    ]
 */

#define AUTOBOOST_COMP_MPW AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__MRC__) || defined(MPW_C) || defined(MPW_CPLUS)
#   if !defined(AUTOBOOST_COMP_MPW_DETECTION) && defined(__MRC__)
#       define AUTOBOOST_COMP_MPW_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VVRR(__MRC__)
#   endif
#   if !defined(AUTOBOOST_COMP_MPW_DETECTION)
#       define AUTOBOOST_COMP_MPW_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_COMP_MPW_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_MPW_EMULATED AUTOBOOST_COMP_MPW_DETECTION
#   else
#       undef AUTOBOOST_COMP_MPW
#       define AUTOBOOST_COMP_MPW AUTOBOOST_COMP_MPW_DETECTION
#   endif
#   define AUTOBOOST_COMP_MPW_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_MPW_NAME "MPW C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_MPW,AUTOBOOST_COMP_MPW_NAME)

#ifdef AUTOBOOST_COMP_MPW_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_MPW_EMULATED,AUTOBOOST_COMP_MPW_NAME)
#endif
