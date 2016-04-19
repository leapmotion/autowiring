/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_MICROTEC_H
#define AUTOBOOST_PREDEF_COMPILER_MICROTEC_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_MRI`]

[@http://www.mentor.com/microtec/ Microtec C/C++] compiler.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_MRI`] [__predef_detection__]]
    ]
 */

#define AUTOBOOST_COMP_MRI AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(_MRI)
#   define AUTOBOOST_COMP_MRI_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#ifdef AUTOBOOST_COMP_MRI_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_MRI_EMULATED AUTOBOOST_COMP_MRI_DETECTION
#   else
#       undef AUTOBOOST_COMP_MRI
#       define AUTOBOOST_COMP_MRI AUTOBOOST_COMP_MRI_DETECTION
#   endif
#   define AUTOBOOST_COMP_MRI_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_MRI_NAME "Microtec C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_MRI,AUTOBOOST_COMP_MRI_NAME)

#ifdef AUTOBOOST_COMP_MRI_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_MRI_EMULATED,AUTOBOOST_COMP_MRI_NAME)
#endif
