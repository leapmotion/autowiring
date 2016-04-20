
#ifndef AUTOBOOST_MPL_MAP_AUX_CONTAINS_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_MAP_AUX_CONTAINS_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/contains_fwd.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/map/aux_/at_impl.hpp>
#include <autoboost/mpl/map/aux_/tag.hpp>

#include <autoboost/type_traits/is_same.hpp>

namespace autoboost { namespace mpl {

template<>
struct contains_impl< aux::map_tag >
{
    template< typename Map, typename Pair > struct apply
        : is_same<
              typename at_impl<aux::map_tag>::apply<
                  Map
                , typename Pair::first
                >::type
            , typename Pair::second
            >
    {
    };
};

}}

#endif // AUTOBOOST_MPL_MAP_AUX_CONTAINS_IMPL_HPP_INCLUDED
