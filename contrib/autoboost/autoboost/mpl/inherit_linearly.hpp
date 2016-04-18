
#ifndef AUTOBOOST_MPL_INHERIT_FRONT_TO_BACK_HPP_INCLUDED
#define AUTOBOOST_MPL_INHERIT_FRONT_TO_BACK_HPP_INCLUDED

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

#include <autoboost/mpl/fold.hpp>
#include <autoboost/mpl/empty_base.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Types_)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(Node_)
    , typename Root_ = empty_base
    >
struct inherit_linearly
    : fold<Types_,Root_,Node_>
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(3,inherit_linearly,(Types_,Node_,Root_))
};

AUTOBOOST_MPL_AUX_NA_SPEC(2, inherit_linearly)

}}

#endif // AUTOBOOST_MPL_INHERIT_FRONT_TO_BACK_HPP_INCLUDED
