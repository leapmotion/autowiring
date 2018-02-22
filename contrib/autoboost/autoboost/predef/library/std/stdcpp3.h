/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LIBRARY_STD_STDCPP3_H
#define AUTOBOOST_PREDEF_LIBRARY_STD_STDCPP3_H

#include <autoboost/predef/library/std/_prefix.h>

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LIB_STD_GNU`]

[@http://gcc.gnu.org/libstdc++/ GNU libstdc++] Standard C++ library.
Version number available as year (from 1970), month, and day.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__GLIBCXX__`] [__predef_detection__]]
    [[`__GLIBCPP__`] [__predef_detection__]]

    [[`__GLIBCXX__`] [V.R.P]]
    [[`__GLIBCPP__`] [V.R.P]]
    ]
 */

#define AUTOBOOST_LIB_STD_GNU AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__GLIBCPP__) || defined(__GLIBCXX__)
#   undef AUTOBOOST_LIB_STD_GNU
#   if defined(__GLIBCXX__)
#       define AUTOBOOST_LIB_STD_GNU AUTOBOOST_PREDEF_MAKE_YYYYMMDD(__GLIBCXX__)
#   else
#       define AUTOBOOST_LIB_STD_GNU AUTOBOOST_PREDEF_MAKE_YYYYMMDD(__GLIBCPP__)
#   endif
#endif

#if AUTOBOOST_LIB_STD_GNU
#   define AUTOBOOST_LIB_STD_GNU_AVAILABLE
#endif

#define AUTOBOOST_LIB_STD_GNU_NAME "GNU"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LIB_STD_GNU,AUTOBOOST_LIB_STD_GNU_NAME)
