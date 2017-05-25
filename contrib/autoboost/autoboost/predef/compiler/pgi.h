/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_PGI_H
#define AUTOBOOST_PREDEF_COMPILER_PGI_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_PGI`]

[@http://en.wikipedia.org/wiki/The_Portland_Group Portland Group C/C++] compiler.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__PGI`] [__predef_detection__]]

    [[`__PGIC__`, `__PGIC_MINOR__`, `__PGIC_PATCHLEVEL__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_PGI AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__PGI)
#   if !defined(AUTOBOOST_COMP_PGI_DETECTION) && (defined(__PGIC__) && defined(__PGIC_MINOR__) && defined(__PGIC_PATCHLEVEL__))
#       define AUTOBOOST_COMP_PGI_DETECTION AUTOBOOST_VERSION_NUMBER(__PGIC__,__PGIC_MINOR__,__PGIC_PATCHLEVEL__)
#   endif
#   if !defined(AUTOBOOST_COMP_PGI_DETECTION)
#       define AUTOBOOST_COMP_PGI_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_COMP_PGI_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_PGI_EMULATED AUTOBOOST_COMP_PGI_DETECTION
#   else
#       undef AUTOBOOST_COMP_PGI
#       define AUTOBOOST_COMP_PGI AUTOBOOST_COMP_PGI_DETECTION
#   endif
#   define AUTOBOOST_COMP_PGI_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_PGI_NAME "Portland Group C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_PGI,AUTOBOOST_COMP_PGI_NAME)

#ifdef AUTOBOOST_COMP_PGI_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_PGI_EMULATED,AUTOBOOST_COMP_PGI_NAME)
#endif
