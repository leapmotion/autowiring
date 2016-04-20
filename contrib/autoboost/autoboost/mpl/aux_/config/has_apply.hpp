
#ifndef AUTOBOOST_MPL_AUX_CONFIG_HAS_APPLY_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_HAS_APPLY_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/config/has_xxx.hpp>
#include <autoboost/mpl/aux_/config/msvc.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#if !defined(AUTOBOOST_MPL_CFG_NO_HAS_APPLY) \
    && (   defined(AUTOBOOST_MPL_CFG_NO_HAS_XXX) \
        || AUTOBOOST_WORKAROUND(__EDG_VERSION__, < 300) \
        || AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, <= 1300) \
        || AUTOBOOST_WORKAROUND(__MWERKS__, AUTOBOOST_TESTED_AT(0x3202)) \
        )

#   define AUTOBOOST_MPL_CFG_NO_HAS_APPLY

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_HAS_APPLY_HPP_INCLUDED
