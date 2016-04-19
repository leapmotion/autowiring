/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_DIGITALMARS_H
#define AUTOBOOST_PREDEF_COMPILER_DIGITALMARS_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_DMC`]

[@http://en.wikipedia.org/wiki/Digital_Mars Digital Mars] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__DMC__`] [__predef_detection__]]

    [[`__DMC__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_DMC AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__DMC__)
#   define AUTOBOOST_COMP_DMC_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VRP(__DMC__)
#endif

#ifdef AUTOBOOST_COMP_DMC_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_DMC_EMULATED AUTOBOOST_COMP_DMC_DETECTION
#   else
#       undef AUTOBOOST_COMP_DMC
#       define AUTOBOOST_COMP_DMC AUTOBOOST_COMP_DMC_DETECTION
#   endif
#   define AUTOBOOST_COMP_DMC_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_DMC_NAME "Digital Mars"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_DMC,AUTOBOOST_COMP_DMC_NAME)

#ifdef AUTOBOOST_COMP_DMC_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_DMC_EMULATED,AUTOBOOST_COMP_DMC_NAME)
#endif
