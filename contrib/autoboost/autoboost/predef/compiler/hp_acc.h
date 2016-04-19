/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_HP_ACC_H
#define AUTOBOOST_PREDEF_COMPILER_HP_ACC_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_HPACC`]

HP aC++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__HP_aCC`] [__predef_detection__]]

    [[`__HP_aCC`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_HPACC AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__HP_aCC)
#   if !defined(AUTOBOOST_COMP_HPACC_DETECTION) && (__HP_aCC > 1)
#       define AUTOBOOST_COMP_HPACC_DETECTION AUTOBOOST_PREDEF_MAKE_10_VVRRPP(__HP_aCC)
#   endif
#   if !defined(AUTOBOOST_COMP_HPACC_DETECTION)
#       define AUTOBOOST_COMP_HPACC_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_COMP_HPACC_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_HPACC_EMULATED AUTOBOOST_COMP_HPACC_DETECTION
#   else
#       undef AUTOBOOST_COMP_HPACC
#       define AUTOBOOST_COMP_HPACC AUTOBOOST_COMP_HPACC_DETECTION
#   endif
#   define AUTOBOOST_COMP_HPACC_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_HPACC_NAME "HP aC++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_HPACC,AUTOBOOST_COMP_HPACC_NAME)

#ifdef AUTOBOOST_COMP_HPACC_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_HPACC_EMULATED,AUTOBOOST_COMP_HPACC_NAME)
#endif
