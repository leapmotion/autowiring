
#ifndef AUTOBOOST_MPL_INDEX_IF_HPP_INCLUDED
#define AUTOBOOST_MPL_INDEX_IF_HPP_INCLUDED

// Copyright Eric Friedman 2003
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
#include <autoboost/mpl/begin_end.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/int.hpp>
#include <autoboost/mpl/iter_fold_if.hpp>
#include <autoboost/mpl/next.hpp>
#include <autoboost/mpl/void.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>
#include <autoboost/type_traits/is_same.hpp>

namespace autoboost { namespace mpl {

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(Predicate)
    >
struct index_if
{
    typedef typename iter_fold_if<
          Sequence
        , int_<0>
        , next<>
        , aux::find_if_pred<Predicate>
        >::type result_;

    typedef typename end<Sequence>::type not_found_;
    typedef typename first<result_>::type result_index_;
    typedef typename second<result_>::type result_iterator_;

    typedef typename if_<
          is_same< result_iterator_,not_found_ >
        , void_
        , result_index_
        >::type type;

    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2,index_if,(Sequence,Predicate))
};

AUTOBOOST_MPL_AUX_NA_SPEC(2, index_if)

}}

#endif // AUTOBOOST_MPL_INDEX_IF_HPP_INCLUDED
