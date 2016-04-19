/*
Copyright Charly Chevalier 2015
Copyright Joel Falcou 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_HARDWARE_SIMD_ARM_H
#define AUTOBOOST_PREDEF_HARDWARE_SIMD_ARM_H

#include <autoboost/predef/version_number.h>
#include <autoboost/predef/hardware/simd/arm/versions.h>

/*`
 [heading `AUTOBOOST_HW_SIMD_ARM`]

 The SIMD extension for ARM (*if detected*).
 Version number depends on the most recent detected extension.

 [table
     [[__predef_symbol__] [__predef_version__]]

     [[`__ARM_NEON__`] [__predef_detection__]]
     [[`__aarch64__`] [__predef_detection__]]
     [[`_M_ARM`] [__predef_detection__]]
     ]

 [table
     [[__predef_symbol__] [__predef_version__]]

     [[`__ARM_NEON__`] [AUTOBOOST_HW_SIMD_ARM_NEON_VERSION]]
     [[`__aarch64__`] [AUTOBOOST_HW_SIMD_ARM_NEON_VERSION]]
     [[`_M_ARM`] [AUTOBOOST_HW_SIMD_ARM_NEON_VERSION]]
     ]

 */

#define AUTOBOOST_HW_SIMD_ARM AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#undef AUTOBOOST_HW_SIMD_ARM
#if !defined(AUTOBOOST_HW_SIMD_ARM) && (defined(__ARM_NEON__) || defined(__aarch64__) || defined (_M_ARM))
#   define AUTOBOOST_HW_SIMD_ARM AUTOBOOST_HW_SIMD_ARM_NEON_VERSION
#endif

#if !defined(AUTOBOOST_HW_SIMD_ARM)
#   define AUTOBOOST_HW_SIMD_ARM AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE
#else
#   define AUTOBOOST_HW_SIMD_ARM_AVAILABLE
#endif

#define AUTOBOOST_HW_SIMD_ARM_NAME "ARM SIMD"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_HW_SIMD_ARM, AUTOBOOST_HW_SIMD_ARM_NAME)
