/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_SGI_MIPSPRO_H
#define AUTOBOOST_PREDEF_COMPILER_SGI_MIPSPRO_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_SGI`]

[@http://en.wikipedia.org/wiki/MIPSpro SGI MIPSpro] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__sgi`] [__predef_detection__]]
    [[`sgi`] [__predef_detection__]]

    [[`_SGI_COMPILER_VERSION`] [V.R.P]]
    [[`_COMPILER_VERSION`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_SGI AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__sgi) || defined(sgi)
#   if !defined(AUTOBOOST_COMP_SGI_DETECTION) && defined(_SGI_COMPILER_VERSION)
#       define AUTOBOOST_COMP_SGI_DETECTION AUTOBOOST_PREDEF_MAKE_10_VRP(_SGI_COMPILER_VERSION)
#   endif
#   if !defined(AUTOBOOST_COMP_SGI_DETECTION) && defined(_COMPILER_VERSION)
#       define AUTOBOOST_COMP_SGI_DETECTION AUTOBOOST_PREDEF_MAKE_10_VRP(_COMPILER_VERSION)
#   endif
#   if !defined(AUTOBOOST_COMP_SGI_DETECTION)
#       define AUTOBOOST_COMP_SGI_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_COMP_SGI_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_SGI_EMULATED AUTOBOOST_COMP_SGI_DETECTION
#   else
#       undef AUTOBOOST_COMP_SGI
#       define AUTOBOOST_COMP_SGI AUTOBOOST_COMP_SGI_DETECTION
#   endif
#   define AUTOBOOST_COMP_SGI_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_SGI_NAME "SGI MIPSpro"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_SGI,AUTOBOOST_COMP_SGI_NAME)

#ifdef AUTOBOOST_COMP_SGI_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_SGI_EMULATED,AUTOBOOST_COMP_SGI_NAME)
#endif
