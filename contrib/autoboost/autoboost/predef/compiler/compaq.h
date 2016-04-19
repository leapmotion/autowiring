/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_COMPILER_COMPAQ_H
#define AUTOBOOST_PREDEF_COMPILER_COMPAQ_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/make.h>

/*`
[heading `AUTOBOOST_COMP_DEC`]

[@http://www.openvms.compaq.com/openvms/brochures/deccplus/ Compaq C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__DECCXX`] [__predef_detection__]]
    [[`__DECC`] [__predef_detection__]]

    [[`__DECCXX_VER`] [V.R.P]]
    [[`__DECC_VER`] [V.R.P]]
    ]
 */

#define AUTOBOOST_COMP_DEC AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__DECC) || defined(__DECCXX)
#   if !defined(AUTOBOOST_COMP_DEC_DETECTION) && defined(__DECCXX_VER)
#       define AUTOBOOST_COMP_DEC_DETECTION AUTOBOOST_PREDEF_MAKE_10_VVRR0PP00(__DECCXX_VER)
#   endif
#   if !defined(AUTOBOOST_COMP_DEC_DETECTION) && defined(__DECC_VER)
#       define AUTOBOOST_COMP_DEC_DETECTION AUTOBOOST_PREDEF_MAKE_10_VVRR0PP00(__DECC_VER)
#   endif
#   if !defined(AUTOBOOST_COMP_DEC_DETECTION)
#       define AUTOBOOST_COM_DEC_DETECTION AUTOBOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef AUTOBOOST_COMP_DEC_DETECTION
#   if defined(AUTOBOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define AUTOBOOST_COMP_DEC_EMULATED AUTOBOOST_COMP_DEC_DETECTION
#   else
#       undef AUTOBOOST_COMP_DEC
#       define AUTOBOOST_COMP_DEC AUTOBOOST_COMP_DEC_DETECTION
#   endif
#   define AUTOBOOST_COMP_DEC_AVAILABLE
#   include <autoboost/predef/detail/comp_detected.h>
#endif

#define AUTOBOOST_COMP_DEC_NAME "Compaq C/C++"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_DEC,AUTOBOOST_COMP_DEC_NAME)

#ifdef AUTOBOOST_COMP_DEC_EMULATED
#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_COMP_DEC_EMULATED,AUTOBOOST_COMP_DEC_NAME)
#endif
