
#ifndef AUTOBOOST_MPL_AUX_ERASE_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_ERASE_IMPL_HPP_INCLUDED

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

#include <autoboost/mpl/clear.hpp>
#include <autoboost/mpl/push_front.hpp>
#include <autoboost/mpl/reverse_fold.hpp>
#include <autoboost/mpl/iterator_range.hpp>
#include <autoboost/mpl/next.hpp>
#include <autoboost/mpl/aux_/na.hpp>

namespace autoboost { namespace mpl {

// default implementation; conrete sequences might override it by
// specializing either the 'erase_impl' or the primary 'erase' template

template< typename Tag >
struct erase_impl
{
    template<
          typename Sequence
        , typename First
        , typename Last
        >
    struct apply
    {
        typedef typename if_na< Last,typename next<First>::type >::type last_;

        // 1st half: [begin, first)
        typedef iterator_range<
              typename begin<Sequence>::type
            , First
            > first_half_;

        // 2nd half: [last, end) ... that is, [last + 1, end)
        typedef iterator_range<
              last_
            , typename end<Sequence>::type
            > second_half_;

        typedef typename reverse_fold<
              second_half_
            , typename clear<Sequence>::type
            , push_front<_,_>
            >::type half_sequence_;

        typedef typename reverse_fold<
              first_half_
            , half_sequence_
            , push_front<_,_>
            >::type type;
    };
};

}}

#endif // AUTOBOOST_MPL_AUX_ERASE_IMPL_HPP_INCLUDED
