/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_PALM_H
#define AUTOBOOST_PREDEF_COMPILER_PALM_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_PALM`]

Palm C/C++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_PACC_VER`] [__predef_detection__]]

    [[`_PACC_VER`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_PALM AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(_PACC_VER)
#   define AUTOBOOST_COMP_PALM_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VRRPP000(_PACC_VER)
#endif

#ifdef AUTOBOOST_COMP_PALM_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_PALM_EMULATED AUTOBOOST_COMP_PALM_DETECTION
#   else
#       undef AUTOBOOST_COMP_PALM
#       define AUTOBOOST_COMP_PALM AUTOBOOST_COMP_PALM_DETECTION
#   endif
#   define AUTOBOOST_COMP_PALM_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_PALM_NAME "Palm C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_PALM,AUTOBOOST_COMP_PALM_NAME)

#ifdef AUTOBOOST_COMP_PALM_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_PALM_EMULATED,AUTOBOOST_COMP_PALM_NAME)
#endif
