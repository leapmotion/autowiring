/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_STD_LIBCOMO_H
#define AUTOBOOST_PREDEF_LIBRARY_STD_LIBCOMO_H

#include <autoboost/predef/library/std/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_STD_COMO`]

[@http://www.comeaucomputing.com/libcomo/ Comeau Computing] Standard C++ Library.
Version number available as major.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__LIBCOMO__`] [__predef_detection__]]

    [[`__LIBCOMO_VERSION__`] [V.0.0]]
    ]
 */

#define AUTOBOOST_LIB_STD_COMO AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__LIBCOMO__)
#   undef AUTOBOOST_LIB_STD_COMO
#   define AUTOBOOST_LIB_STD_COMO AUTOBOOST_VERSION_NUMBER(__LIBCOMO_VERSION__,0,0)
#endif

#if AUTOBOOST_LIB_STD_COMO
#   define AUTOBOOST_LIB_STD_COMO_AVAILABLE
#endif

#define AUTOBOOST_LIB_STD_COMO_NAME "Comeau Computing"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_STD_COMO,AUTOBOOST_LIB_STD_COMO_NAME)
