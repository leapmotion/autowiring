/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_IAR_H
#define AUTOBOOST_PREDEF_COMPILER_IAR_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_IAR`]

IAR C/C++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__IAR_SYSTEMS_ICC__`] [__predef_detection__]]

    [[`__VER__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_IAR AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__IAR_SYSTEMS_ICC__)
#   define AUTOBOOST_COMP_IAR_DETECTION AUTOBOOST_PREDEF_MAKE_10_VVRR(__VER__)
#endif

#ifdef AUTOBOOST_COMP_IAR_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_IAR_EMULATED AUTOBOOST_COMP_IAR_DETECTION
#   else
#       undef AUTOBOOST_COMP_IAR
#       define AUTOBOOST_COMP_IAR AUTOBOOST_COMP_IAR_DETECTION
#   endif
#   define AUTOBOOST_COMP_IAR_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_IAR_NAME "IAR C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_IAR,AUTOBOOST_COMP_IAR_NAME)

#ifdef AUTOBOOST_COMP_IAR_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_IAR_EMULATED,AUTOBOOST_COMP_IAR_NAME)
#endif
