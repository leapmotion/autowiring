/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_STD_VACPP_H
#define AUTOBOOST_PREDEF_LIBRARY_STD_VACPP_H

#include <autoboost/predef/library/std/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_STD_IBM`]

[@http://www.ibm.com/software/awdtools/xlcpp/ IBM VACPP Standard C++] library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__IBMCPP__`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_LIB_STD_IBM AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__IBMCPP__)
#   undef AUTOBOOST_LIB_STD_IBM
#   define AUTOBOOST_LIB_STD_IBM AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_LIB_STD_IBM
#   define AUTOBOOST_LIB_STD_IBM_AVAILABLE
#endif

#define AUTOBOOST_LIB_STD_IBM_NAME "IBM VACPP"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_STD_IBM,AUTOBOOST_LIB_STD_IBM_NAME)
