/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_DIAB_H
#define AUTOBOOST_PREDEF_COMPILER_DIAB_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_DIAB`]

[@http://www.windriver.com/products/development_suite/wind_river_compiler/ Diab C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__DCC__`] [__predef_detection__]]

    [[`__VERSION_NUMBER__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_DIAB AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__DCC__)
#   define AUTOBOOST_COMP_DIAB_DETECTION AUTOBOOST_PREDEF_MAKE_10_VRPP(__VERSION_NUMBER__)
#endif

#ifdef AUTOBOOST_COMP_DIAB_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_DIAB_EMULATED AUTOBOOST_COMP_DIAB_DETECTION
#   else
#       undef AUTOBOOST_COMP_DIAB
#       define AUTOBOOST_COMP_DIAB AUTOBOOST_COMP_DIAB_DETECTION
#   endif
#   define AUTOBOOST_COMP_DIAB_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_DIAB_NAME "Diab C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_DIAB,AUTOBOOST_COMP_DIAB_NAME)

#ifdef AUTOBOOST_COMP_DIAB_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_DIAB_EMULATED,AUTOBOOST_COMP_DIAB_NAME)
#endif
