
#ifndef AUTOBOOST_MPL_ITER_FOLD_IF_HPP_INCLUDED
#define AUTOBOOST_MPL_ITER_FOLD_IF_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
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

#include <autoboost/mpl/begin_end.hpp>
#include <autoboost/mpl/logical.hpp>
#include <autoboost/mpl/always.hpp>
#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/pair.hpp>
#include <autoboost/mpl/apply.hpp>
#include <autoboost/mpl/aux_/iter_fold_if_impl.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>
#include <autoboost/mpl/aux_/config/forwarding.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

#include <autoboost/type_traits/is_same.hpp>

namespace autoboost { namespace mpl {

namespace aux {

template< typename Predicate, typename LastIterator >
struct iter_fold_if_pred
{
    template< typename State, typename Iterator > struct apply
#if !defined(AUTOBOOST_MPL_CFG_NO_NESTED_FORWARDING)
        : and_<
              not_< is_same<Iterator,LastIterator> >
            , apply1<Predicate,Iterator>
            >
    {
#else
    {
        typedef and_<
              not_< is_same<Iterator,LastIterator> >
            , apply1<Predicate,Iterator>
            > type;
#endif
    };
};

} // namespace aux

template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(State)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(ForwardOp)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(ForwardPredicate)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(BackwardOp)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(BackwardPredicate)
    >
struct iter_fold_if
{

    typedef typename begin<Sequence>::type first_;
    typedef typename end<Sequence>::type last_;

    typedef typename eval_if<
          is_na<BackwardPredicate>
        , if_< is_na<BackwardOp>, always<false_>, always<true_> >
        , identity<BackwardPredicate>
        >::type backward_pred_;

// cwpro8 doesn't like 'cut-off' type here (use typedef instead)
#if !AUTOBOOST_WORKAROUND(__MWERKS__, AUTOBOOST_TESTED_AT(0x3003)) && !AUTOBOOST_WORKAROUND(__IBMCPP__, AUTOBOOST_TESTED_AT(600))
    struct result_ :
#else
    typedef
#endif
        aux::iter_fold_if_impl<
          first_
        , State
        , ForwardOp
        , protect< aux::iter_fold_if_pred< ForwardPredicate,last_ > >
        , BackwardOp
        , backward_pred_
        >
#if !AUTOBOOST_WORKAROUND(__MWERKS__, AUTOBOOST_TESTED_AT(0x3003)) && !AUTOBOOST_WORKAROUND(__IBMCPP__, AUTOBOOST_TESTED_AT(600))
    { };
#else
    result_;
#endif

public:

    typedef pair<
          typename result_::state
        , typename result_::iterator
        > type;

    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(
          6
        , iter_fold_if
        , (Sequence,State,ForwardOp,ForwardPredicate,BackwardOp,BackwardPredicate)
        )
};

AUTOBOOST_MPL_AUX_NA_SPEC(6, iter_fold_if)

}}

#endif // AUTOBOOST_MPL_ITER_FOLD_IF_HPP_INCLUDED
