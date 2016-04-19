/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_OS_QNXNTO_H
#define AUTOBOOST_PREDEF_OS_QNXNTO_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_OS_QNX`]

[@http://en.wikipedia.org/wiki/QNX QNX] operating system.
Version number available as major, and minor if possible. And
version 4 is specifically detected.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__QNX__`] [__predef_detection__]]
    [[`__QNXNTO__`] [__predef_detection__]]

    [[`_NTO_VERSION`] [V.R.0]]
    [[`__QNX__`] [4.0.0]]
    ]
 */

#define AUTOBOOST_OS_QNX AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(AUTOBOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__QNX__) || defined(__QNXNTO__) \
    )
#   undef AUTOBOOST_OS_QNX
#   if !defined(AUTOBOOST_OS_QNX) && defined(_NTO_VERSION)
#       define AUTOBOOST_OS_QNX AUTOBOOST_PREDEF_MAKE_10_VVRR(_NTO_VERSION)
#   endif
#   if !defined(AUTOBOOST_OS_QNX) && defined(__QNX__)
#       define AUTOBOOST_OS_QNX AUTOBOOST_VERSION_NUMBER(4,0,0)
#   endif
#   if !defined(AUTOBOOST_OS_QNX)
#       define AUTOBOOST_OS_QNX AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if AUTOBOOST_OS_QNX
#   define AUTOBOOST_OS_QNX_AVAILABLE
#   include <autoboost/predef/detail/os_detected.h>
#endif

#define AUTOBOOST_OS_QNX_NAME "QNX"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_OS_QNX,AUTOBOOST_OS_QNX_NAME)
