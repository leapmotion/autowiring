
#ifndef AUTOBOOST_MPL_UNIQUE_HPP_INCLUDED
#define AUTOBOOST_MPL_UNIQUE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
// Copyright John R. Bandela 2000-2002
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
#include <autoboost/mpl/reverse_fold.hpp>
#include <autoboost/mpl/eval_if.hpp>
#include <autoboost/mpl/and.hpp>
#include <autoboost/mpl/identity.hpp>
#include <autoboost/mpl/pair.hpp>
#include <autoboost/mpl/apply.hpp>
#include <autoboost/mpl/aux_/inserter_algorithm.hpp>
#include <autoboost/mpl/aux_/na.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_spec.hpp>

namespace autoboost { namespace mpl {

namespace aux {

template< typename Predicate, typename Operation >
struct unique_op
{
    template< typename Pair, typename T > struct apply
    {
        typedef typename Pair::first seq_;
        typedef typename Pair::second prior_;
        typedef typename eval_if<
              and_< is_not_na<prior_>, apply2<Predicate,prior_,T> >
            , identity<seq_>
            , apply2<Operation,seq_,T>
            >::type new_seq_;

        typedef pair<new_seq_,T> type;
    };
};

template<
      typename Sequence
    , typename Predicate
    , typename Inserter
    >
struct unique_impl
    : first< typename fold<
          Sequence
        , pair< typename Inserter::state,na >
        , protect< aux::unique_op<Predicate,typename Inserter::operation> >
        >::type >
{
};

template<
      typename Sequence
    , typename Predicate
    , typename Inserter
    >
struct reverse_unique_impl
    : first< typename reverse_fold<
          Sequence
        , pair< typename Inserter::state,na >
        , protect< aux::unique_op<Predicate,typename Inserter::operation> >
        >::type >
{
};

} // namespace aux

AUTOBOOST_MPL_AUX_INSERTER_ALGORITHM_DEF(3, unique)

}}

#endif // AUTOBOOST_MPL_UNIQUE_HPP_INCLUDED
