
#ifndef AUTOBOOST_MPL_AUX_HAS_APPLY_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_HAS_APPLY_HPP_INCLUDED

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

#include <autoboost/mpl/has_xxx.hpp>
#include <autoboost/mpl/aux_/config/has_apply.hpp>

namespace autoboost { namespace mpl { namespace aux {
#if !defined(AUTOBOOST_MPL_CFG_NO_HAS_APPLY)
AUTOBOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_apply, apply, false)
#else
template< typename T, typename fallback_ = false_ >
struct has_apply
    : fallback_
{
};
#endif
}}}

#endif // AUTOBOOST_MPL_AUX_HAS_APPLY_HPP_INCLUDED
