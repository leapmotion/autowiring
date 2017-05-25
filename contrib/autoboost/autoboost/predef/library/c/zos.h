/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_C_ZOS_H
#define AUTOBOOST_PREDEF_LIBRARY_C_ZOS_H

#include <autoboost/predef/library/c/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_C_ZOS`]

z/OS libc Standard C library.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__LIBREL__`] [__predef_detection__]]

    [[`__LIBREL__`] [V.R.P]]
    [[`__TARGET_LIB__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_LIB_C_ZOS AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__LIBREL__)
#   undef AUTOBOOST_LIB_C_ZOS
#   if !defined(AUTOBOOST_LIB_C_ZOS) && defined(__LIBREL__)
#       define AUTOBOOST_LIB_C_ZOS AUTOBOOST_PREDEF_MAKE_0X_VRRPPPP(__LIBREL__)
#   endif
#   if !defined(AUTOBOOST_LIB_C_ZOS) && defined(__TARGET_LIB__)
#       define AUTOBOOST_LIB_C_ZOS AUTOBOOST_PREDEF_MAKE_0X_VRRPPPP(__TARGET_LIB__)
#   endif
#   if !defined(AUTOBOOST_LIB_C_ZOS)
#       define AUTOBOOST_LIB_C_ZOS AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if AUTOBOOST_LIB_C_ZOS
#   define AUTOBOOST_LIB_C_ZOS_AVAILABLE
#endif

#define AUTOBOOST_LIB_C_ZOS_NAME "z/OS"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_C_ZOS,AUTOBOOST_LIB_C_ZOS_NAME)
