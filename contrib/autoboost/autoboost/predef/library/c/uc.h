/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_C_UC_H
#define AUTOBOOST_PREDEF_LIBRARY_C_UC_H

#include <autoboost/predef/library/c/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_C_UC`]

[@http://en.wikipedia.org/wiki/Uclibc uClibc] Standard C library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__UCLIBC__`] [__predef_detection__]]

    [[`__UCLIBC_MAJOR__`, `__UCLIBC_MINOR__`, `__UCLIBC_SUBLEVEL__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_LIB_C_UC AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__UCLIBC__)
#   undef AUTOBOOST_LIB_C_UC
#   define AUTOBOOST_LIB_C_UC AUTOBOOST_VERSION_NUMBER(\
        __UCLIBC_MAJOR__,__UCLIBC_MINOR__,__UCLIBC_SUBLEVEL__)
#endif

#if AUTOBOOST_LIB_C_UC
#   define AUTOBOOST_LIB_C_UC_AVAILABLE
#endif

#define AUTOBOOST_LIB_C_UC_NAME "uClibc"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_C_UC,AUTOBOOST_LIB_C_UC_NAME)
