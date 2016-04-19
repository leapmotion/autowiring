/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_OS_AIX_H
#define AUTOBOOST_PREDEF_OS_AIX_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_OS_AIX`]

[@http://en.wikipedia.org/wiki/AIX_operating_system IBM AIX] operating system.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_AIX`] [__predef_detection__]]
    [[`__TOS_AIX__`] [__predef_detection__]]

    [[`_AIX43`] [4.3.0]]
    [[`_AIX41`] [4.1.0]]
    [[`_AIX32`] [3.2.0]]
    [[`_AIX3`] [3.0.0]]
    ]
 */

#define AUTOBOOST_OS_AIX AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(AUTOBOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(_AIX) || defined(__TOS_AIX__) \
    )
#   undef AUTOBOOST_OS_AIX
#   if !defined(AUTOBOOST_OS_AIX) && defined(_AIX43)
#       define AUTOBOOST_OS_AIX AUTOBOOST_VERSION_NUMBER(4,3,0)
#   endif
#   if !defined(AUTOBOOST_OS_AIX) && defined(_AIX41)
#       define AUTOBOOST_OS_AIX AUTOBOOST_VERSION_NUMBER(4,1,0)
#   endif
#   if !defined(AUTOBOOST_OS_AIX) && defined(_AIX32)
#       define AUTOBOOST_OS_AIX AUTOBOOST_VERSION_NUMBER(3,2,0)
#   endif
#   if !defined(AUTOBOOST_OS_AIX) && defined(_AIX3)
#       define AUTOBOOST_OS_AIX AUTOBOOST_VERSION_NUMBER(3,0,0)
#   endif
#   if !defined(AUTOBOOST_OS_AIX)
#       define AUTOBOOST_OS_AIX AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if AUTOBOOST_OS_AIX
#   define AUTOBOOST_OS_AIX_AVAILABLE
#   include <autoboost/predef/detail/os_detected.h>
#endif

#define AUTOBOOST_OS_AIX_NAME "IBM AIX"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_OS_AIX,AUTOBOOST_OS_AIX_NAME)
