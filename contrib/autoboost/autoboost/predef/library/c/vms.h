/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_C_VMS_H
#define AUTOBOOST_PREDEF_LIBRARY_C_VMS_H

#include <autoboost/predef/library/c/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_C_VMS`]

VMS libc Standard C library.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__CRTL_VER`] [__predef_detection__]]

    [[`__CRTL_VER`] [V.R.P]]
    ]
 */

#define AUTOBOOST_LIB_C_VMS AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__CRTL_VER)
#   undef AUTOBOOST_LIB_C_VMS
#   define AUTOBOOST_LIB_C_VMS AUTOBOOST_PREDEF_MAKE_10_VVRR0PP00(__CRTL_VER)
#endif

#if AUTOBOOST_LIB_C_VMS
#   define AUTOBOOST_LIB_C_VMS_AVAILABLE
#endif

#define AUTOBOOST_LIB_C_VMS_NAME "VMS"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_C_VMS,AUTOBOOST_LIB_C_VMS_NAME)
