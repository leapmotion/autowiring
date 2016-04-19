
#ifndef AUTOBOOST_MPL_AUX_VECTOR_ITERATOR_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_VECTOR_ITERATOR_HPP_INCLUDED

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

#include <autoboost/mpl/vector/aux_/at.hpp>
#include <autoboost/mpl/iterator_tags.hpp>
#include <autoboost/mpl/plus.hpp>
#include <autoboost/mpl/minus.hpp>
#include <autoboost/mpl/advance_fwd.hpp>
#include <autoboost/mpl/distance_fwd.hpp>
#include <autoboost/mpl/next.hpp>
#include <autoboost/mpl/prior.hpp>
#include <autoboost/mpl/aux_/nttp_decl.hpp>
#include <autoboost/mpl/aux_/value_wknd.hpp>
#include <autoboost/mpl/aux_/config/ctps.hpp>
#include <autoboost/mpl/aux_/config/workaround.hpp>

namespace autoboost { namespace mpl {

template<
      typename Vector
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, n_)
    >
struct v_iter
{
    typedef aux::v_iter_tag tag;
    typedef random_access_iterator_tag category;
    typedef typename v_at<Vector,n_>::type type;

    typedef Vector vector_;
    typedef mpl::long_<n_> pos;

#if defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
    enum {
          next_ = n_ + 1
        , prior_ = n_ - 1
        , pos_ = n_
    };

    typedef v_iter<Vector,next_> next;
    typedef v_iter<Vector,prior_> prior;
#endif

};


#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template<
      typename Vector
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, n_)
    >
struct next< v_iter<Vector,n_> >
{
    typedef v_iter<Vector,(n_ + 1)> type;
};

template<
      typename Vector
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, n_)
    >
struct prior< v_iter<Vector,n_> >
{
    typedef v_iter<Vector,(n_ - 1)> type;
};

template<
      typename Vector
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, n_)
    , typename Distance
    >
struct advance< v_iter<Vector,n_>,Distance>
{
    typedef v_iter<
          Vector
        , (n_ + AUTOBOOST_MPL_AUX_NESTED_VALUE_WKND(long, Distance))
        > type;
};

template<
      typename Vector
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, n_)
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, m_)
    >
struct distance< v_iter<Vector,n_>, v_iter<Vector,m_> >
    : mpl::long_<(m_ - n_)>
{
};

#else // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

template<> struct advance_impl<aux::v_iter_tag>
{
    template< typename Iterator, typename N > struct apply
    {
        enum { pos_ = Iterator::pos_, n_ = N::value };
        typedef v_iter<
              typename Iterator::vector_
            , (pos_ + n_)
            > type;
    };
};

template<> struct distance_impl<aux::v_iter_tag>
{
    template< typename Iter1, typename Iter2 > struct apply
    {
        enum { pos1_ = Iter1::pos_, pos2_ = Iter2::pos_ };
        typedef long_<( pos2_ - pos1_ )> type;
        AUTOBOOST_STATIC_CONSTANT(long, value = ( pos2_ - pos1_ ));
    };
};

#endif

}}

#endif // AUTOBOOST_MPL_AUX_VECTOR_ITERATOR_HPP_INCLUDED
