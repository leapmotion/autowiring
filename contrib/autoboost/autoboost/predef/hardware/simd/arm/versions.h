/*
Copyright Charly Chevalier 2015
Copyright Joel Falcou 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_HARDWARE_SIMD_ARM_VERSIONS_H
#define AUTOBOOST_PREDEF_HARDWARE_SIMD_ARM_VERSIONS_H

#include <autoboost/predef/version_number.h>

/*`
 Those defines represent ARM SIMD extensions versions.

 [note You *MUST* compare them with the predef `AUTOBOOST_HW_SIMD_ARM`.]
 */

// ---------------------------------

/*`
 [heading `AUTOBOOST_HW_SIMD_ARM_NEON_VERSION`]

 The [@https://en.wikipedia.org/wiki/ARM_architecture#Advanced_SIMD_.28NEON.29 NEON]
 ARM extension version number.

 Version number is: *1.0.0*.
 */
#define AUTOBOOST_HW_SIMD_ARM_NEON_VERSION AUTOBOOST_VERSION_NUMBER(1, 0, 0)

#endif
