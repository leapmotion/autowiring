/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_COMEAU_H
#define AUTOBOOST_PREDEF_COMPILER_COMEAU_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

#define AUTOBOOST_COMP_COMO AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

/*`
[heading `AUTOBOOST_COMP_COMO`]

[@http://en.wikipedia.org/wiki/Comeau_C/C%2B%2B Comeau C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__COMO__`] [__predef_detection__]]

    [[`__COMO_VERSION__`] [V.R.P]]
    ]
 */

#if defined(__COMO__)
#   if !defined(AUTOBOOST_COMP_COMO_DETECTION) && defined(__COMO_VERSION__)
#       define AUTOBOOST_COMP_COMO_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VRP(__COMO_VERSION__)
#   endif
#   if !defined(AUTOBOOST_COMP_COMO_DETECTION)
#       define AUTOBOOST_COMP_COMO_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_COMP_COMO_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_COMO_EMULATED AUTOBOOST_COMP_COMO_DETECTION
#   else
#       undef AUTOBOOST_COMP_COMO
#       define AUTOBOOST_COMP_COMO AUTOBOOST_COMP_COMO_DETECTION
#   endif
#   define AUTOBOOST_COMP_COMO_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_COMO_NAME "Comeau C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_COMO,AUTOBOOST_COMP_COMO_NAME)

#ifdef AUTOBOOST_COMP_COMO_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_COMO_EMULATED,AUTOBOOST_COMP_COMO_NAME)
#endif
