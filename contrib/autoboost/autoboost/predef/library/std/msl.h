/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_STD_MSL_H
#define AUTOBOOST_PREDEF_LIBRARY_STD_MSL_H

#include <autoboost/predef/library/std/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_STD_MSL`]

[@http://www.freescale.com/ Metrowerks] Standard C++ Library.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MSL_CPP__`] [__predef_detection__]]
    [[`__MSL__`] [__predef_detection__]]

    [[`__MSL_CPP__`] [V.R.P]]
    [[`__MSL__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_LIB_STD_MSL AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__MSL_CPP__) || defined(__MSL__)
#   undef AUTOBOOST_LIB_STD_MSL
#   if defined(__MSL_CPP__)
#       define AUTOBOOST_LIB_STD_MSL AUTOBOOST_PREDEF_MAKE_0X_VRPP(__MSL_CPP__)
#   else
#       define AUTOBOOST_LIB_STD_MSL AUTOBOOST_PREDEF_MAKE_0X_VRPP(__MSL__)
#   endif
#endif

#if AUTOBOOST_LIB_STD_MSL
#   define AUTOBOOST_LIB_STD_MSL_AVAILABLE
#endif

#define AUTOBOOST_LIB_STD_MSL_NAME "Metrowerks"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_STD_MSL,AUTOBOOST_LIB_STD_MSL_NAME)
