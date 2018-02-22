#ifndef DATE_TIME_LOCALE_CONFIG_HPP___
#define DATE_TIME_LOCALE_CONFIG_HPP___

/* Copyright (c) 2002-2006 CrystalClear Software, Inc.
 * Use, modification and distribution is subject to the
 * Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)
 * Author: Jeff Garland
 * $Date$
 */

// This file configures whether the library will support locales and hence
// iostream based i/o.  Even if a compiler has some support for locales,
// any failure to be compatible gets the compiler on the exclusion list.
//
// At the moment this is defined for MSVC 6 and any compiler that
// defines AUTOBOOST_NO_STD_LOCALE (gcc 2.95.x)

#include "autoboost/config.hpp" //sets AUTOBOOST_NO_STD_LOCALE
#include "autoboost/detail/workaround.hpp"

//This file basically becomes a noop if locales are not properly supported
#if (defined(AUTOBOOST_NO_STD_LOCALE)  \
 || (AUTOBOOST_WORKAROUND( AUTOBOOST_MSVC, < 1300)) \
 || (AUTOBOOST_WORKAROUND( __BORLANDC__, AUTOBOOST_TESTED_AT( 0x581 )) ) )
#define AUTOBOOST_DATE_TIME_NO_LOCALE
#endif


#endif

