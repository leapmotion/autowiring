/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_EKOPATH_H
#define AUTOBOOST_PREDEF_COMPILER_EKOPATH_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_PATH`]

[@http://en.wikipedia.org/wiki/PathScale EKOpath] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__PATHCC__`] [__predef_detection__]]

    [[`__PATHCC__`, `__PATHCC_MINOR__`, `__PATHCC_PATCHLEVEL__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_PATH AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__PATHCC__)
#   define AUTOBOOST_COMP_PATH_DETECTION \
        AUTOBOOST_VERSION_NUMBER(__PATHCC__,__PATHCC_MINOR__,__PATHCC_PATCHLEVEL__)
#endif

#ifdef AUTOBOOST_COMP_PATH_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_PATH_EMULATED AUTOBOOST_COMP_PATH_DETECTION
#   else
#       undef AUTOBOOST_COMP_PATH
#       define AUTOBOOST_COMP_PATH AUTOBOOST_COMP_PATH_DETECTION
#   endif
#   define AUTOBOOST_COMP_PATH_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_PATH_NAME "EKOpath"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_PATH,AUTOBOOST_COMP_PATH_NAME)

#ifdef AUTOBOOST_COMP_PATH_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_PATH_EMULATED,AUTOBOOST_COMP_PATH_NAME)
#endif
