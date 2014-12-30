/*
Copyright Rene Rivera 2013
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AUTOBOOST_PREDEF_DETAIL_ENDIAN_COMPAT_H
#define AUTOBOOST_PREDEF_DETAIL_ENDIAN_COMPAT_H

#include <autoboost/predef/other/endian.h>

#if AUTOBOOST_ENDIAN_BIG_BYTE
#   define AUTOBOOST_BIG_ENDIAN
#   define AUTOBOOST_BYTE_ORDER 4321
#endif
#if AUTOBOOST_ENDIAN_LITTLE_BYTE
#   define AUTOBOOST_LITTLE_ENDIAN
#   define AUTOBOOST_BYTE_ORDER 1234
#endif
#if AUTOBOOST_ENDIAN_LITTLE_WORD
#   define AUTOBOOST_PDP_ENDIAN
#   define AUTOBOOST_BYTE_ORDER 2134
#endif

#endif
