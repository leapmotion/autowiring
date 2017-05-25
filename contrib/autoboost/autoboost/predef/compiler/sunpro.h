/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_SUNPRO_H
#define AUTOBOOST_PREDEF_COMPILER_SUNPRO_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_SUNPRO`]

[@http://en.wikipedia.org/wiki/Oracle_Solaris_Studio Oracle Solaris Studio] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__SUNPRO_CC`] [__predef_detection__]]
    [[`__SUNPRO_C`] [__predef_detection__]]

    [[`__SUNPRO_CC`] [V.R.P]]
    [[`__SUNPRO_C`] [V.R.P]]
    [[`__SUNPRO_CC`] [VV.RR.P]]
    [[`__SUNPRO_C`] [VV.RR.P]]
    ]
 */

#define AUTOBOOST_COMP_SUNPRO AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__SUNPRO_CC) || defined(__SUNPRO_C)
#   if !defined(AUTOBOOST_COMP_SUNPRO_DETECTION) && defined(__SUNPRO_CC)
#       if (__SUNPRO_CC < 0x5100)
#           define AUTOBOOST_COMP_SUNPRO_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VRP(__SUNPRO_CC)
#       else
#           define AUTOBOOST_COMP_SUNPRO_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VVRRP(__SUNPRO_CC)
#       endif
#   endif
#   if !defined(AUTOBOOST_COMP_SUNPRO_DETECTION) && defined(__SUNPRO_C)
#       if (__SUNPRO_C < 0x5100)
#           define AUTOBOOST_COMP_SUNPRO_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VRP(__SUNPRO_C)
#       else
#           define AUTOBOOST_COMP_SUNPRO_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VVRRP(__SUNPRO_C)
#       endif
#   endif
#   if !defined(AUTOBOOST_COMP_SUNPRO_DETECTION)
#       define AUTOBOOST_COMP_SUNPRO_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_COMP_SUNPRO_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_SUNPRO_EMULATED AUTOBOOST_COMP_SUNPRO_DETECTION
#   else
#       undef AUTOBOOST_COMP_SUNPRO
#       define AUTOBOOST_COMP_SUNPRO AUTOBOOST_COMP_SUNPRO_DETECTION
#   endif
#   define AUTOBOOST_COMP_SUNPRO_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_SUNPRO_NAME "Oracle Solaris Studio"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_SUNPRO,AUTOBOOST_COMP_SUNPRO_NAME)

#ifdef AUTOBOOST_COMP_SUNPRO_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_SUNPRO_EMULATED,AUTOBOOST_COMP_SUNPRO_NAME)
#endif
