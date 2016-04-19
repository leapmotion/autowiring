
#ifndef AUTOBOOST_MPL_REVERSE_FOLD_HPP_INCLUDED
#define AUTOBOOST_MPL_REVERSE_FOLD_HPP_INCLUDED

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

#include <autoboost/mpl/begin_end.hpp>
#include <autoboost/mpl/O1_size.hpp>
#include <autoboost/mpl/arg.hpp>
#include <autoboost/mpl/aux_/reverse_fold_impl.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(State)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(BackwardOp)
    , typename ForwardOp = arg<1>
    >
struct reverse_fold
{
    typedef typename aux::reverse_fold_impl<
          ::autoboost::mpl::O1_size<Sequence>::value
        , typename begin<Sequence>::type
        , typename end<Sequence>::type
        , State
        , BackwardOp
        , ForwardOp
        >::state type;

    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(3,reverse_fold,(Sequence,State,BackwardOp))
};

AUTOBOOST_MPL_AUX_NA_SPEC(3, reverse_fold)

}}

#endif // AUTOBOOST_MPL_REVERSE_FOLD_HPP_INCLUDED
