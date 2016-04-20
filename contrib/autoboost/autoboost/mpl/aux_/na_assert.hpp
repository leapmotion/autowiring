
#ifndef AUTOBOOST_MPL_AUX_NA_ASSERT_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_NA_ASSERT_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/na.hpp>
#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if !AUTOBOOST_WORKAROUND(_MSC_FULL_VER, <= 140050601)    \
    && !AUTOBOOST_WORKAROUND(__EDG_VERSION__, <= 243)
#   include <autoboost/mpl/assert.hpp>
#   define AUTOBOOST_MPL_AUX_ASSERT_NOT_NA(x) \
    AUTOBOOST_MPL_ASSERT_NOT((autoboost::mpl::is_na<type>)) \
/**/
#else
#   include <autoboost/static_assert.hpp>
#   define AUTOBOOST_MPL_AUX_ASSERT_NOT_NA(x) \
    AUTOBOOST_STATIC_ASSERT(!autoboost::mpl::is_na<x>::value) \
/**/
#endif

#endif // AUTOBOOST_MPL_AUX_NA_ASSERT_HPP_INCLUDED
