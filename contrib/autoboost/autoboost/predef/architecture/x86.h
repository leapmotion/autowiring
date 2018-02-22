/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#include <autoboost/predef/architecture/x86/32.h>
#include <autoboost/predef/architecture/x86/64.h>

#ifndef AUTOBOOST_PREDEF_ARCHITECTURE_X86_H
#define AUTOBOOST_PREDEF_ARCHITECTURE_X86_H

/*`
[heading `AUTOBOOST_ARCH_X86`]

[@http://en.wikipedia.org/wiki/X86 Intel x86] architecture. This is
a category to indicate that either `AUTOBOOST_ARCH_X86_32` or
`AUTOBOOST_ARCH_X86_64` is detected.
 */

#define AUTOBOOST_ARCH_X86 AUTOBOOST_VERSION_NUMBER_NOT_AVAILABLE

#if AUTOBOOST_ARCH_X86_32 || AUTOBOOST_ARCH_X86_64
#   undef AUTOBOOST_ARCH_X86
#   define AUTOBOOST_ARCH_X86 AUTOBOOST_VERSION_NUMBER_AVAILABLE
#endif

#if AUTOBOOST_ARCH_X86
#   define AUTOBOOST_ARCH_X86_AVAILABLE
#endif

#define AUTOBOOST_ARCH_X86_NAME "Intel x86"

#endif

#include <autoboost/predef/detail/test.h>
AUTOBOOST_PREDEF_DECLARE_TEST(AUTOBOOST_ARCH_X86,AUTOBOOST_ARCH_X86_NAME)
