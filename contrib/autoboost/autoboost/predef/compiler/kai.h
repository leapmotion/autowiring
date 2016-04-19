/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_KAI_H
#define AUTOBOOST_PREDEF_COMPILER_KAI_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_KCC`]

Kai C++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__KCC`] [__predef_detection__]]

    [[`__KCC_VERSION`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_KCC AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__KCC)
#   define AUTOBOOST_COMP_KCC_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VRPP(__KCC_VERSION)
#endif

#ifdef AUTOBOOST_COMP_KCC_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_KCC_EMULATED AUTOBOOST_COMP_KCC_DETECTION
#   else
#       undef AUTOBOOST_COMP_KCC
#       define AUTOBOOST_COMP_KCC AUTOBOOST_COMP_KCC_DETECTION
#   endif
#   define AUTOBOOST_COMP_KCC_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_KCC_NAME "Kai C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_KCC,AUTOBOOST_COMP_KCC_NAME)

#ifdef AUTOBOOST_COMP_KCC_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_KCC_EMULATED,AUTOBOOST_COMP_KCC_NAME)
#endif
