/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_STD_MODENA_H
#define AUTOBOOST_PREDEF_LIBRARY_STD_MODENA_H

#include <autoboost/predef/library/std/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_STD_MSIPL`]

[@http://modena.us/ Modena Software Lib++] Standard C++ Library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`MSIPL_COMPILE_H`] [__predef_detection__]]
    [[`__MSIPL_COMPILE_H`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_LIB_STD_MSIPL AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(MSIPL_COMPILE_H) || defined(__MSIPL_COMPILE_H)
#   undef AUTOBOOST_LIB_STD_MSIPL
#   define AUTOBOOST_LIB_STD_MSIPL AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_LIB_STD_MSIPL
#   define AUTOBOOST_LIB_STD_MSIPL_AVAILABLE
#endif

#define AUTOBOOST_LIB_STD_MSIPL_NAME "Modena Software Lib++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_STD_MSIPL,AUTOBOOST_LIB_STD_MSIPL_NAME)
