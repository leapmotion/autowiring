/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_METROWERKS_H
#define AUTOBOOST_PREDEF_COMPILER_METROWERKS_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_MWERKS`]

[@http://en.wikipedia.org/wiki/CodeWarrior Metrowerks CodeWarrior] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MWERKS__`] [__predef_detection__]]
    [[`__CWCC__`] [__predef_detection__]]

    [[`__CWCC__`] [V.R.P]]
    [[`__MWERKS__`] [V.R.P >= 4.2.0]]
    [[`__MWERKS__`] [9.R.0]]
    [[`__MWERKS__`] [8.R.0]]
    ]
 */

#define AUTOBOOST_COMP_MWERKS AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__MWERKS__) || defined(__CWCC__)
#   if !defined(AUTOBOOST_COMP_MWERKS_DETECTION) && defined(__CWCC__)
#       define AUTOBOOST_COMP_MWERKS_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VRPP(__CWCC__)
#   endif
#   if !defined(AUTOBOOST_COMP_MWERKS_DETECTION) && (__MWERKS__ >= 0x4200)
#       define AUTOBOOST_COMP_MWERKS_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VRPP(__MWERKS__)
#   endif
#   if !defined(AUTOBOOST_COMP_MWERKS_DETECTION) && (__MWERKS__ >= 0x3204) // note the "skip": 04->9.3
#       define AUTOBOOST_COMP_MWERKS_DETECTION AUTOBOOST_VERSION_NUMBER(9,(__MWERKS__)%100-1,0)
#   endif
#   if !defined(AUTOBOOST_COMP_MWERKS_DETECTION) && (__MWERKS__ >= 0x3200)
#       define AUTOBOOST_COMP_MWERKS_DETECTION AUTOBOOST_VERSION_NUMBER(9,(__MWERKS__)%100,0)
#   endif
#   if !defined(AUTOBOOST_COMP_MWERKS_DETECTION) && (__MWERKS__ >= 0x3000)
#       define AUTOBOOST_COMP_MWERKS_DETECTION AUTOBOOST_VERSION_NUMBER(8,(__MWERKS__)%100,0)
#   endif
#   if !defined(AUTOBOOST_COMP_MWERKS_DETECTION)
#       define AUTOBOOST_COMP_MWERKS_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_COMP_MWERKS_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_MWERKS_EMULATED AUTOBOOST_COMP_MWERKS_DETECTION
#   else
#       undef AUTOBOOST_COMP_MWERKS
#       define AUTOBOOST_COMP_MWERKS AUTOBOOST_COMP_MWERKS_DETECTION
#   endif
#   define AUTOBOOST_COMP_MWERKS_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_MWERKS_NAME "Metrowerks CodeWarrior"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_MWERKS,AUTOBOOST_COMP_MWERKS_NAME)

#ifdef AUTOBOOST_COMP_MWERKS_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_MWERKS_EMULATED,AUTOBOOST_COMP_MWERKS_NAME)
#endif
