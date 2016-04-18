
#ifndef AUTOBOOST_MPL_AUX_INSERT_RANGE_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_INSERT_RANGE_IMPL_HPP_INCLUDED

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

#include <autoboost/mpl/copy.hpp>
#include <autoboost/mpl/clear.hpp>
#include <autoboost/mpl/front_inserter.hpp>
#include <autoboost/mpl/joint_view.hpp>
#include <autoboost/mpl/iterator_range.hpp>
#include <autoboost/mpl/aux_/na_spec.hpp>
#include <autoboost/mpl/aux_/iter_push_front.hpp>
#include <autoboost/mpl/aux_/traits_lambda_spec.hpp>
#include <autoboost/mpl/aux_/config/forwarding.hpp>

#include <autoboost/type_traits/same_traits.hpp>

namespace autoboost { namespace mpl {

// default implementation; conrete sequences might override it by
// specializing either the 'insert_range_impl' or the primary
// 'insert_range' template


template< typename Tag >
struct insert_range_impl
{
    template<
          typename Sequence
        , typename Pos
        , typename Range
        >
    struct apply
#if !defined(AUTOBOOST_MPL_CFG_NO_NESTED_FORWARDING)
        : reverse_copy<
              joint_view<
                  iterator_range<typename begin<Sequence>::type,Pos>
                , joint_view<
                      Range
                    , iterator_range<Pos,typename end<Sequence>::type>
                    >
                >
            , front_inserter< typename clear<Sequence>::type >
            >
    {
#else
    {
        typedef typename reverse_copy<
              joint_view<
                  iterator_range<typename begin<Sequence>::type,Pos>
                , joint_view<
                      Range
                    , iterator_range<Pos,typename end<Sequence>::type>
                    >
                >
            , front_inserter< typename clear<Sequence>::type >
            >::type type;
#endif
    };
};

AUTOBOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(3,insert_range_impl)

}}

#endif // AUTOBOOST_MPL_AUX_INSERT_RANGE_IMPL_HPP_INCLUDED
