
#ifndef AUTOBOOST_MPL_FIND_IF_HPP_INCLUDED
#define AUTOBOOST_MPL_FIND_IF_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/find_if_pred.hpp>
#include <autoboost/mpl/arg.hpp>
#include <autoboost/mpl/iter_fold_if.hpp>
#include <autoboost/mpl/aux_/common_name_wknd.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>

namespace autoboost { namespace mpl {

AUTOBOOST_MPL_AUX_COMMON_NAME_WKND(find_if)

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(Predicate)
    >
struct find_if
{
    typedef typename iter_fold_if<
          Sequence
        , void
        , mpl::arg<1> // ignore
        , protect< aux::find_if_pred<Predicate> >
        >::type result_;

    typedef typename second<result_>::type type;

    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2,find_if,(Sequence,Predicate))
};

AUTOBOOST_MPL_AUX_NA_SPEC(2,find_if)

}}

#endif // AUTOBOOST_MPL_FIND_IF_HPP_INCLUDED
