/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_IBM_H
#define AUTOBOOST_PREDEF_COMPILER_IBM_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_IBM`]

[@http://en.wikipedia.org/wiki/VisualAge IBM XL C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__IBMCPP__`] [__predef_detection__]]
    [[`__xlC__`] [__predef_detection__]]
    [[`__xlc__`] [__predef_detection__]]

    [[`__COMPILER_VER__`] [V.R.P]]
    [[`__xlC__`] [V.R.P]]
    [[`__xlc__`] [V.R.P]]
    [[`__IBMCPP__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_IBM AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__IBMCPP__) || defined(__xlC__) || defined(__xlc__)
#   if !defined(AUTOBOOST_COMP_IBM_DETECTION) && defined(__COMPILER_VER__)
#       define AUTOBOOST_COMP_IBM_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VRRPPPP(__COMPILER_VER__)
#   endif
#   if !defined(AUTOBOOST_COMP_IBM_DETECTION) && defined(__xlC__)
#       define AUTOBOOST_COMP_IBM_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VVRR(__xlC__)
#   endif
#   if !defined(AUTOBOOST_COMP_IBM_DETECTION) && defined(__xlc__)
#       define AUTOBOOST_COMP_IBM_DETECTION AUTOBOOST_PREDEF_MAKE_0X_VVRR(__xlc__)
#   endif
#   if !defined(AUTOBOOST_COMP_IBM_DETECTION)
#       define AUTOBOOST_COMP_IBM_DETECTION AUTOBOOST_PREDEF_MAKE_10_VRP(__IBMCPP__)
#   endif
#endif

#ifdef AUTOBOOST_COMP_IBM_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_IBM_EMULATED AUTOBOOST_COMP_IBM_DETECTION
#   else
#       undef AUTOBOOST_COMP_IBM
#       define AUTOBOOST_COMP_IBM AUTOBOOST_COMP_IBM_DETECTION
#   endif
#   define AUTOBOOST_COMP_IBM_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_IBM_NAME "IBM XL C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_IBM,AUTOBOOST_COMP_IBM_NAME)

#ifdef AUTOBOOST_COMP_IBM_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_IBM_EMULATED,AUTOBOOST_COMP_IBM_NAME)
#endif
