
#ifndef AUTOBOOST_MPL_AUX_CONFIG_HAS_XXX_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_CONFIG_HAS_XXX_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
// Copyright David Abrahams 2002-2003
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/config/overload_resolution.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

// agurt, 11/jan/03: signals a stub-only 'has_xxx' implementation

#if !defined(AUTOBOOST_MPL_CFG_NO_HAS_XXX) \
    && (   defined(AUTOBOOST_MPL_CFG_BROKEN_OVERLOAD_RESOLUTION) \
        || AUTOBOOST_WORKAROUND(__GNUC__, <= 2) \
        || AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840)) \
        )

#   define AUTOBOOST_MPL_CFG_NO_HAS_XXX
#   define AUTOBOOST_MPL_CFG_NO_HAS_XXX_TEMPLATE

#endif

#endif // AUTOBOOST_MPL_AUX_CONFIG_HAS_XXX_HPP_INCLUDED
