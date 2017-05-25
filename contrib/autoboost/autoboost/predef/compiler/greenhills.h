/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_GREENHILLS_H
#define AUTOBOOST_PREDEF_COMPILER_GREENHILLS_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_GHS`]

[@http://en.wikipedia.org/wiki/Green_Hills_Software Green Hills C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__ghs`] [__predef_detection__]]
    [[`__ghs__`] [__predef_detection__]]

    [[`__GHS_VERSION_NUMBER__`] [V.R.P]]
    [[`__ghs`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_GHS AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__ghs) || defined(__ghs__)
#   if !defined(AUTOBOOST_COMP_GHS_DETECTION) && defined(__GHS_VERSION_NUMBER__)
#       define AUTOBOOST_COMP_GHS_DETECTION AUTOBOOST_PREDEF_MAKE_10_VRP(__GHS_VERSION_NUMBER__)
#   endif
#   if !defined(AUTOBOOST_COMP_GHS_DETECTION) && defined(__ghs)
#       define AUTOBOOST_COMP_GHS_DETECTION AUTOBOOST_PREDEF_MAKE_10_VRP(__ghs)
#   endif
#   if !defined(AUTOBOOST_COMP_GHS_DETECTION)
#       define AUTOBOOST_COMP_GHS_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_COMP_GHS_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_GHS_EMULATED AUTOBOOST_COMP_GHS_DETECTION
#   else
#       undef AUTOBOOST_COMP_GHS
#       define AUTOBOOST_COMP_GHS AUTOBOOST_COMP_GHS_DETECTION
#   endif
#   define AUTOBOOST_COMP_GHS_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_GHS_NAME "Green Hills C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_GHS,AUTOBOOST_COMP_GHS_NAME)

#ifdef AUTOBOOST_COMP_GHS_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_GHS_EMULATED,AUTOBOOST_COMP_GHS_NAME)
#endif
