/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_INTEL_H
#define AUTOBOOST_PREDEF_COMPILER_INTEL_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_INTEL`]

[@http://en.wikipedia.org/wiki/Intel_C%2B%2B Intel C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__INTEL_COMPILER`] [__predef_detection__]]
    [[`__ICL`] [__predef_detection__]]
    [[`__ICC`] [__predef_detection__]]
    [[`__ECC`] [__predef_detection__]]

    [[`__INTEL_COMPILER`] [V.R]]
    [[`__INTEL_COMPILER` and `__INTEL_COMPILER_UPDATE`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_INTEL AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || \
    defined(__ECC)
#   if !defined(AUTOBOOST_COMP_INTEL_DETECTION) && defined(__INTEL_COMPILER) && defined(__INTEL_COMPILER_UPDATE)
#       define AUTOBOOST_COMP_INTEL_DETECTION AUTOBOOST_VERSION_NUMBER( \
            AUTOBOOST_VERSION_NUMBER_MAJOR(AUTOBOOST_PREDEF_MAKE_10_VVRR(__INTEL_COMPILER)), \
            AUTOBOOST_VERSION_NUMBER_MINOR(AUTOBOOST_PREDEF_MAKE_10_VVRR(__INTEL_COMPILER)), \
            __INTEL_COMPILER_UPDATE)
#   endif
#   if !defined(AUTOBOOST_COMP_INTEL_DETECTION) && defined(__INTEL_COMPILER)
#       define AUTOBOOST_COMP_INTEL_DETECTION AUTOBOOST_PREDEF_MAKE_10_VVRR(__INTEL_COMPILER)
#   endif
#   if !defined(AUTOBOOST_COMP_INTEL_DETECTION)
#       define AUTOBOOST_COMP_INTEL_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_COMP_INTEL_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_INTEL_EMULATED AUTOBOOST_COMP_INTEL_DETECTION
#   else
#       undef AUTOBOOST_COMP_INTEL
#       define AUTOBOOST_COMP_INTEL AUTOBOOST_COMP_INTEL_DETECTION
#   endif
#   define AUTOBOOST_COMP_INTEL_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_INTEL_NAME "Intel C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_INTEL,AUTOBOOST_COMP_INTEL_NAME)

#ifdef AUTOBOOST_COMP_INTEL_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_INTEL_EMULATED,AUTOBOOST_COMP_INTEL_NAME)
#endif
