/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_STD_ROGUEWAVE_H
#define AUTOBOOST_PREDEF_LIBRARY_STD_ROGUEWAVE_H

#include <autoboost/predef/library/std/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_STD_RW`]

[@http://stdcxx.apache.org/ Roguewave] Standard C++ library.
If available version number as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__STD_RWCOMPILER_H__`] [__predef_detection__]]
    [[`_RWSTD_VER`] [__predef_detection__]]

    [[`_RWSTD_VER`] [V.R.P]]
    ]
 */

#define AUTOBOOST_LIB_STD_RW AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__STD_RWCOMPILER_H__) || defined(_RWSTD_VER)
#   undef AUTOBOOST_LIB_STD_RW
#   if defined(_RWSTD_VER)
#       if _RWSTD_VER < 0x010000
#           define AUTOBOOST_LIB_STD_RW AUTOBOOST_PREDEF_MAKE_0X_VVRRP(_RWSTD_VER)
#       else
#           define AUTOBOOST_LIB_STD_RW AUTOBOOST_PREDEF_MAKE_0X_VVRRPP(_RWSTD_VER)
#       endif
#   else
#       define AUTOBOOST_LIB_STD_RW AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if AUTOBOOST_LIB_STD_RW
#   define AUTOBOOST_LIB_STD_RW_AVAILABLE
#endif

#define AUTOBOOST_LIB_STD_RW_NAME "Roguewave"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_STD_RW,AUTOBOOST_LIB_STD_RW_NAME)
