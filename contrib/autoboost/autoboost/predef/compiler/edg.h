/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_EDG_H
#define AUTOBOOST_PREDEF_COMPILER_EDG_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_EDG`]

[@http://en.wikipedia.org/wiki/Edison_Design_Group EDG C++ Frontend] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__EDG__`] [__predef_detection__]]

    [[`__EDG_VERSION__`] [V.R.0]]
    ]
 */

#define AUTOBOOST_COMP_EDG AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__EDG__)
#   define AUTOBOOST_COMP_EDG_DETECTION AUTOBOOST_PREDEF_MAKE_10_VRR(__EDG_VERSION__)
#endif

#ifdef AUTOBOOST_COMP_EDG_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_EDG_EMULATED AUTOBOOST_COMP_EDG_DETECTION
#   else
#       undef AUTOBOOST_COMP_EDG
#       define AUTOBOOST_COMP_EDG AUTOBOOST_COMP_EDG_DETECTION
#   endif
#   define AUTOBOOST_COMP_EDG_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_EDG_NAME "EDG C++ Frontend"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_EDG,AUTOBOOST_COMP_EDG_NAME)

#ifdef AUTOBOOST_COMP_EDG_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_EDG_EMULATED,AUTOBOOST_COMP_EDG_NAME)
#endif
