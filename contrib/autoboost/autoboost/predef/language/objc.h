/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_LANGUAGE_OBJC_H
#define AUTOBOOST_PREDEF_LANGUAGE_OBJC_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_LANG_OBJC`]

[@http://en.wikipedia.org/wiki/Objective-C Objective-C] language.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__OBJC__`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_LANG_OBJC AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__OBJC__)
#   undef AUTOBOOST_LANG_OBJC
#   define AUTOBOOST_LANG_OBJC AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_LANG_OBJC
#   define AUTOBOOST_LANG_OBJC_AVAILABLE
#endif

#define AUTOBOOST_LANG_OBJC_NAME "Objective-C"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_LANG_OBJC,AUTOBOOST_LANG_OBJC_NAME)
