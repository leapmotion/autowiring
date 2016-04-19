/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_CLANG_H
#define AUTOBOOST_PREDEF_COMPILER_CLANG_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_CLANG`]

[@http://en.wikipedia.org/wiki/Clang Clang] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__clang__`] [__predef_detection__]]

    [[`__clang_major__`, `__clang_minor__`, `__clang_patchlevel__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_CLANG AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__clang__)
#   define AUTOBOOST_COMP_CLANG_DETECTION AUTOBOOST_VERSION_NUMBER(__clang_major__,__clang_minor__,__clang_patchlevel__)
#endif

#ifdef AUTOBOOST_COMP_CLANG_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_CLANG_EMULATED AUTOBOOST_COMP_CLANG_DETECTION
#   else
#       undef AUTOBOOST_COMP_CLANG
#       define AUTOBOOST_COMP_CLANG AUTOBOOST_COMP_CLANG_DETECTION
#   endif
#   define AUTOBOOST_COMP_CLANG_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_CLANG_NAME "Clang"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_CLANG,AUTOBOOST_COMP_CLANG_NAME)

#ifdef AUTOBOOST_COMP_CLANG_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_CLANG_EMULATED,AUTOBOOST_COMP_CLANG_NAME)
#endif
