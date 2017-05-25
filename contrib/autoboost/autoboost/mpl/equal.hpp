
#ifndef AUTOBOOST_MPL_EQUAL_HPP_INCLUDED
#define AUTOBOOST_MPL_EQUAL_HPP_INCLUDED

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

#include <autoboost/mpl/aux_/iter_fold_if_impl.hpp>
#include <autoboost/mpl/aux_/iter_apply.hpp>
#include <autoboost/mpl/and.hpp>
#include <autoboost/mpl/not.hpp>
#include <autoboost/mpl/begin_end.hpp>
#include <autoboost/mpl/next.hpp>
#include <autoboost/mpl/always.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/lambda.hpp>
#include <autoboost/mpl/bind.hpp>
#include <autoboost/mpl/apply.hpp>
#include <autoboost/mpl/void.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/lambda_support.hpp>
#include <autoboost/mpl/aux_/msvc_eti_base.hpp>

#include <autoboost/type_traits/is_same.hpp>

namespace autoboost { namespace mpl {

namespace aux {

template<
      typename Predicate
    , typename LastIterator1
    , typename LastIterator2
    >
struct equal_pred
{
    template<
          typename Iterator2
        , typename Iterator1
        >
    struct apply
    {
        typedef typename and_<
              not_< is_same<Iterator1,LastIterator1> >
            , not_< is_same<Iterator2,LastIterator2> >
            , aux::iter_apply2<Predicate,Iterator1,Iterator2>
            >::type type;
    };
};

template<
      typename Sequence1
    , typename Sequence2
    , typename Predicate
    >
struct equal_impl
{
    typedef typename begin<Sequence1>::type first1_;
    typedef typename begin<Sequence2>::type first2_;
    typedef typename end<Sequence1>::type last1_;
    typedef typename end<Sequence2>::type last2_;

    typedef aux::iter_fold_if_impl<
          first1_
        , first2_
        , next<>
        , protect< aux::equal_pred<Predicate,last1_,last2_> >
        , void_
        , always<false_>
        > fold_;

    typedef typename fold_::iterator iter1_;
    typedef typename fold_::state iter2_;
    typedef and_<
          is_same<iter1_,last1_>
        , is_same<iter2_,last2_>
        > result_;

    typedef typename result_::type type;
};


} // namespace aux


template<
      typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence1)
    , typename AUTOBOOST_MPL_AUX_NA_PARAM(Sequence2)
    , typename Predicate = is_same<_,_>
    >
struct equal
    : aux::msvc_eti_base<
          typename aux::equal_impl<Sequence1,Sequence2,Predicate>::type
        >::type
{
    AUTOBOOST_MPL_AUX_LAMBDA_SUPPORT(2,equal,(Sequence1,Sequence2))
};

AUTOBOOST_MPL_AUX_NA_SPEC(2, equal)

}}

#endif // AUTOBOOST_MPL_EQUAL_HPP_INCLUDED
