
#ifndef AUTOBOOST_MPL_MAP_AUX_CLEAR_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_MAP_AUX_CLEAR_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2004
// Copyright David Abrahams 2003-2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/clear_fwd.hpp>
#include <autoboost/mpl/map/aux_/map0.hpp>
#include <autoboost/mpl/map/aux_/tag.hpp>

namespace autoboost { namespace mpl {

template<>
struct clear_impl< aux::map_tag >
{
    template< typename Map > struct apply
    {
        typedef map0<> type;
    };
};

}}

#endif // AUTOBOOST_MPL_MAP_AUX_CLEAR_IMPL_HPP_INCLUDED
