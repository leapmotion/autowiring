
#ifndef AUTOBOOST_MPL_AUX_STATIC_CAST_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_STATIC_CAST_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/config/workaround.hpp>

#if AUTOBOOST_WORKAROUND(__BORLANDC__, AUTOBOOST_TESTED_AT(0x561)) \
 || AUTOBOOST_WORKAROUND(__GNUC__, < 3) \
 || AUTOBOOST_WORKAROUND(__MWERKS__, <= 0x3001)
#   define AUTOBOOST_MPL_AUX_STATIC_CAST(T, expr) (T)(expr)
#else
#   define AUTOBOOST_MPL_AUX_STATIC_CAST(T, expr) static_cast<T>(expr)
#endif

#endif // AUTOBOOST_MPL_AUX_STATIC_CAST_HPP_INCLUDED
