/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_GCC_XML_H
#define AUTOBOOST_PREDEF_COMPILER_GCC_XML_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_GCCXML`]

[@http://www.gccxml.org/ GCC XML] compiler.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__GCCXML__`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_COMP_GCCXML AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__GCCXML__)
#   define AUTOBOOST_COMP_GCCXML_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#ifdef AUTOBOOST_COMP_GCCXML_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_GCCXML_EMULATED AUTOBOOST_COMP_GCCXML_DETECTION
#   else
#       undef AUTOBOOST_COMP_GCCXML
#       define AUTOBOOST_COMP_GCCXML AUTOBOOST_COMP_GCCXML_DETECTION
#   endif
#   define AUTOBOOST_COMP_GCCXML_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_GCCXML_NAME "GCC XML"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_GCCXML,AUTOBOOST_COMP_GCCXML_NAME)

#ifdef AUTOBOOST_COMP_GCCXML_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_GCCXML_EMULATED,AUTOBOOST_COMP_GCCXML_NAME)
#endif
