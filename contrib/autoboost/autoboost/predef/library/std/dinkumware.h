/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_STD_DINKUMWARE_H
#define AUTOBOOST_PREDEF_LIBRARY_STD_DINKUMWARE_H

#include <autoboost/predef/library/std/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_STD_DINKUMWARE`]

[@http://en.wikipedia.org/wiki/Dinkumware Dinkumware] Standard C++ Library.
If available version number as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_YVALS`, `__IBMCPP__`] [__predef_detection__]]
    [[`_CPPLIB_VER`] [__predef_detection__]]

    [[`_CPPLIB_VER`] [V.R.0]]
    ]
 */

#define AUTOBOOST_LIB_STD_DINKUMWARE AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if (defined(_YVALS) && !defined(__IBMCPP__)) || defined(_CPPLIB_VER)
#   undef AUTOBOOST_LIB_STD_DINKUMWARE
#   if defined(_CPPLIB_VER)
#       define AUTOBOOST_LIB_STD_DINKUMWARE AUTOBOOST_PREDEF_MAKE_10_VVRR(_CPPLIB_VER)
#   else
#       define AUTOBOOST_LIB_STD_DINKUMWARE AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if AUTOBOOST_LIB_STD_DINKUMWARE
#   define AUTOBOOST_LIB_STD_DINKUMWARE_AVAILABLE
#endif

#define AUTOBOOST_LIB_STD_DINKUMWARE_NAME "Dinkumware"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_STD_DINKUMWARE,AUTOBOOST_LIB_STD_DINKUMWARE_NAME)
