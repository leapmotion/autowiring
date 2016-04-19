/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_WATCOM_H
#define AUTOBOOST_PREDEF_COMPILER_WATCOM_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_WATCOM`]

[@http://en.wikipedia.org/wiki/Watcom Watcom C++] compiler.
Version number available as major, and minor.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__WATCOMC__`] [__predef_detection__]]

    [[`__WATCOMC__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_WATCOM AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__WATCOMC__)
#   define AUTOBOOST_COMP_WATCOM_DETECTION AUTOBOOST_PREDEF_MAKE_10_VVRR(__WATCOMC__)
#endif

#ifdef AUTOBOOST_COMP_WATCOM_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_WATCOM_EMULATED AUTOBOOST_COMP_WATCOM_DETECTION
#   else
#       undef AUTOBOOST_COMP_WATCOM
#       define AUTOBOOST_COMP_WATCOM AUTOBOOST_COMP_WATCOM_DETECTION
#   endif
#   define AUTOBOOST_COMP_WATCOM_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_WATCOM_NAME "Watcom C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_WATCOM,AUTOBOOST_COMP_WATCOM_NAME)

#ifdef AUTOBOOST_COMP_WATCOM_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_WATCOM_EMULATED,AUTOBOOST_COMP_WATCOM_NAME)
#endif
