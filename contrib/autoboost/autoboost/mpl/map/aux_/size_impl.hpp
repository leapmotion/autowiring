
#ifndef AUTOBOOST_MPL_MAP_AUX_SIZE_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_MAP_AUX_SIZE_IMPL_HPP_INCLUDED

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

#include <autoboost/mpl/size_fwd.hpp>
#include <autoboost/mpl/map/aux_/tag.hpp>

namespace autoboost { namespace mpl {

template<>
struct size_impl< aux::map_tag >
{
    template< typename Map > struct apply
        : Map::size
    {
    };
};

}}

#endif // AUTOBOOST_MPL_MAP_AUX_SIZE_IMPL_HPP_INCLUDED
