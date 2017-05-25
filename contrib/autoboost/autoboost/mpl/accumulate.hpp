
#ifndef AUTOBOOST_MPL_ACCUMULATE_HPP_INCLUDED
#define AUTOBOOST_MPL_ACCUMULATE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
// Copyright David Abrahams 2001-2002
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
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(State)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(ForwardOp)
    >
struct accumulate
    : fold<Sequence,State,ForwardOp>
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(3,accumulate,(Sequence,State,ForwardOp))
};

AUTOBOOST_MPL_AUX_NA_SPEC(3, accumulate)

}}

#endif // AUTOBOOST_MPL_ACCUMULATE_HPP_INCLUDED
