
#ifndef AUTOBOOST_MPL_SET_AUX_INSERT_RANGE_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_SET_AUX_INSERT_RANGE_IMPL_HPP_INCLUDED

// Copyright Bruno Dutra 2015
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/insert_range_fwd.hpp>
#include <autoboost/mpl/set/aux_/tag.hpp>
#include <autoboost/mpl/placeholders.hpp>
#include <autoboost/mpl/fold.hpp>
#include <autoboost/mpl/insert.hpp>

namespace autoboost { namespace mpl {

template<>
struct insert_range_impl< aux::set_tag >
{
    template<
          typename Sequence
        , typename /*Pos*/
        , typename Range
        >
    struct apply
        : fold<Range, Sequence, insert<_1, _2> >
    {
    };
};

}}

#endif // AUTOBOOST_MPL_SET_AUX_INSERT_RANGE_IMPL_HPP_INCLUDED
