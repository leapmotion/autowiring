/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_GCC_H
#define AUTOBOOST_PREDEF_COMPILER_GCC_H

/* Other compilers that emulate this one need to be detected first. */

#include <autoboost/predef/compiler/clang.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_GNUC`]

[@http://en.wikipedia.org/wiki/GNU_Compiler_Collection Gnu GCC C/C++] compiler.
Version number available as major, minor, and patch (if available).

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__GNUC__`] [__predef_detection__]]

    [[`__GNUC__`, `__GNUC_MINOR__`, `__GNUC_PATCHLEVEL__`] [V.R.P]]
    [[`__GNUC__`, `__GNUC_MINOR__`] [V.R.0]]
    ]
 */

#define AUTOBOOST_COMP_GNUC AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__GNUC__)
#   if !defined(AUTOBOOST_COMP_GNUC_DETECTION) && defined(__GNUC_PATCHLEVEL__)
#       define AUTOBOOST_COMP_GNUC_DETECTION \
            AUTOBOOST_VERSION_NUMBER(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__)
#   endif
#   if !defined(AUTOBOOST_COMP_GNUC_DETECTION)
#       define AUTOBOOST_COMP_GNUC_DETECTION \
            AUTOBOOST_VERSION_NUMBER(__GNUC__,__GNUC_MINOR__,0)
#   endif
#endif

#ifdef AUTOBOOST_COMP_GNUC_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_GNUC_EMULATED AUTOBOOST_COMP_GNUC_DETECTION
#   else
#       undef AUTOBOOST_COMP_GNUC
#       define AUTOBOOST_COMP_GNUC AUTOBOOST_COMP_GNUC_DETECTION
#   endif
#   define AUTOBOOST_COMP_GNUC_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_GNUC_NAME "Gnu GCC C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_GNUC,AUTOBOOST_COMP_GNUC_NAME)

#ifdef AUTOBOOST_COMP_GNUC_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_GNUC_EMULATED,AUTOBOOST_COMP_GNUC_NAME)
#endif
