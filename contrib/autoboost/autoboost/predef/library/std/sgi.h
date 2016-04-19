/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_STD_SGI_H
#define AUTOBOOST_PREDEF_LIBRARY_STD_SGI_H

#include <autoboost/predef/library/std/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_STD_SGI`]

[@http://www.sgi.com/tech/stl/ SGI] Standard C++ library.
If available version number as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__STL_CONFIG_H`] [__predef_detection__]]

    [[`__SGI_STL`] [V.R.P]]
    ]
 */

#define AUTOBOOST_LIB_STD_SGI AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__STL_CONFIG_H)
#   undef AUTOBOOST_LIB_STD_SGI
#   if defined(__SGI_STL)
#       define AUTOBOOST_LIB_STD_SGI AUTOBOOST_PREDEF_MAKE_0X_VRP(__SGI_STL)
#   else
#       define AUTOBOOST_LIB_STD_SGI AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if AUTOBOOST_LIB_STD_SGI
#   define AUTOBOOST_LIB_STD_SGI_AVAILABLE
#endif

#define AUTOBOOST_LIB_STD_SGI_NAME "SGI"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_STD_SGI,AUTOBOOST_LIB_STD_SGI_NAME)
