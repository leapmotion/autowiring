/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_VISUALC_H
#define AUTOBOOST_PREDEF_COMPILER_VISUALC_H

/* Other compilers that emulate this one need to be detected first. */

#include <autoboost/predef/compiler/clang.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_MSVC`]

[@http://en.wikipedia.org/wiki/Visual_studio Microsoft Visual C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_MSC_VER`] [__predef_detection__]]

    [[`_MSC_FULL_VER`] [V.R.P]]
    [[`_MSC_VER`] [V.R.0]]
    ]
 */

#define AUTOBOOST_COMP_MSVC AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(_MSC_VER)
#   if !defined (_MSC_FULL_VER)
#       define AUTOBOOST_COMP_MSVC_BUILD 0
#   else
        /* how many digits does the build number have? */
#       if _MSC_FULL_VER / 10000 == _MSC_VER
            /* four digits */
#           define AUTOBOOST_COMP_MSVC_BUILD (_MSC_FULL_VER % 10000)
#       elif _MSC_FULL_VER / 100000 == _MSC_VER
            /* five digits */
#           define AUTOBOOST_COMP_MSVC_BUILD (_MSC_FULL_VER % 100000)
#       else
#           error "Cannot determine build number from _MSC_FULL_VER"
#       endif
#   endif
    /*
    VS2014 was skipped in the release sequence for MS. Which
    means that the compiler and VS product versions are no longer
    in sync. Hence we need to use different formulas for
    mapping from MSC version to VS product version.
    */
#   if (_MSC_VER >= 1900)
#       define AUTOBOOST_COMP_MSVC_DETECTION AUTOBOOST_VERSION_NUMBER(\
            _MSC_VER/100-5,\
            _MSC_VER%100,\
            AUTOBOOST_COMP_MSVC_BUILD)
#   else
#       define AUTOBOOST_COMP_MSVC_DETECTION AUTOBOOST_VERSION_NUMBER(\
            _MSC_VER/100-6,\
            _MSC_VER%100,\
            AUTOBOOST_COMP_MSVC_BUILD)
#   endif
#endif

#ifdef AUTOBOOST_COMP_MSVC_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_MSVC_EMULATED AUTOBOOST_COMP_MSVC_DETECTION
#   else
#       undef AUTOBOOST_COMP_MSVC
#       define AUTOBOOST_COMP_MSVC AUTOBOOST_COMP_MSVC_DETECTION
#   endif
#   define AUTOBOOST_COMP_MSVC_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_MSVC_NAME "Microsoft Visual C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_MSVC,AUTOBOOST_COMP_MSVC_NAME)

#ifdef AUTOBOOST_COMP_MSVC_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_MSVC_EMULATED,AUTOBOOST_COMP_MSVC_NAME)
#endif
