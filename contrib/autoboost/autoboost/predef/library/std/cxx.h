/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_STD_CXX_H
#define AUTOBOOST_PREDEF_LIBRARY_STD_CXX_H

#include <autoboost/predef/library/std/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_STD_CXX`]

[@http://libcxx.llvm.org/ libc++] C++ Standard Library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_LIBCPP_VERSION`] [__predef_detection__]]

    [[`_LIBCPP_VERSION`] [V.0.P]]
    ]
 */

#define AUTOBOOST_LIB_STD_CXX AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(_LIBCPP_VERSION)
#   undef AUTOBOOST_LIB_STD_CXX
#   define AUTOBOOST_LIB_STD_CXX AUTOBOOST_PREDEF_MAKE_10_VPPP(_LIBCPP_VERSION)
#endif

#if AUTOBOOST_LIB_STD_CXX
#   define AUTOBOOST_LIB_STD_CXX_AVAILABLE
#endif

#define AUTOBOOST_LIB_STD_CXX_NAME "libc++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_STD_CXX,AUTOBOOST_LIB_STD_CXX_NAME)
