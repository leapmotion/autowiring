/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_DIGNUS_H
#define AUTOBOOST_PREDEF_COMPILER_DIGNUS_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_SYSC`]

[@http://www.dignus.com/dcxx/ Dignus Systems/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__SYSC__`] [__predef_detection__]]

    [[`__SYSC_VER__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_SYSC AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__SYSC__)
#   define AUTOBOOST_COMP_SYSC_DETECTION AUTOBOOST_PREDEF_MAKE_10_VRRPP(__SYSC_VER__)
#endif

#ifdef AUTOBOOST_COMP_SYSC_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_SYSC_EMULATED AUTOBOOST_COMP_SYSC_DETECTION
#   else
#       undef AUTOBOOST_COMP_SYSC
#       define AUTOBOOST_COMP_SYSC AUTOBOOST_COMP_SYSC_DETECTION
#   endif
#   define AUTOBOOST_COMP_SYSC_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_SYSC_NAME "Dignus Systems/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_SYSC,AUTOBOOST_COMP_SYSC_NAME)

#ifdef AUTOBOOST_COMP_SYSC_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_SYSC_EMULATED,AUTOBOOST_COMP_SYSC_NAME)
#endif
