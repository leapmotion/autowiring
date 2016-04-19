/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_BORLAND_H
#define AUTOBOOST_PREDEF_COMPILER_BORLAND_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_BORLAND`]

[@http://en.wikipedia.org/wiki/C_plus_plus_builder Borland C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__BORLANDC__`] [__predef_detection__]]
    [[`__CODEGEARC__`] [__predef_detection__]]

    [[`__BORLANDC__`] [V.R.P]]
    [[`__CODEGEARC__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_BORLAND AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__BORLANDC__) || defined(__CODEGEARC__)
#   if !defined(AUTOBOOST_COMP_BORLAND_DETECTION) && (defined(__CODEGEARC__))
#       define AUTOBOOST_COMP_BORLAND_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VVRP(__CODEGEARC__)
#   endif
#   if !defined(AUTOBOOST_COMP_BORLAND_DETECTION)
#       define AUTOBOOST_COMP_BORLAND_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VVRP(__BORLANDC__)
#   endif
#endif

#ifdef AUTOBOOST_COMP_BORLAND_DETECTION
#   define AUTOBOOST_COMP_BORLAND_AVAILABLE
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_BORLAND_EMULATED AUTOBOOST_COMP_BORLAND_DETECTION
#   else
#       undef AUTOBOOST_COMP_BORLAND
#       define AUTOBOOST_COMP_BORLAND AUTOBOOST_COMP_BORLAND_DETECTION
#   endif
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_BORLAND_NAME "Borland C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_BORLAND,AUTOBOOST_COMP_BORLAND_NAME)

#ifdef AUTOBOOST_COMP_BORLAND_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_BORLAND_EMULATED,AUTOBOOST_COMP_BORLAND_NAME)
#endif
