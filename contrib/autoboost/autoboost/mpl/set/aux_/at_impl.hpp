
#ifndef AUTOBOOST_MPL_SET_AUX_AT_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_SET_AUX_AT_IMPL_HPP_INCLUDED

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

#include <autoboost/mpl/at_fwd.hpp>
#include <autoboost/mpl/set/aux_/has_key_impl.hpp>
#include <autoboost/mpl/set/aux_/tag.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/void.hpp>

namespace autoboost { namespace mpl {

template<>
struct at_impl< aux::set_tag >
{
    template< typename Set, typename T > struct apply
    {
        typedef typename if_<
              has_key_impl<aux::set_tag>::apply<Set,T>
            , T
            , void_
            >::type type;
    };
};

}}

#endif // AUTOBOOST_MPL_SET_AUX_AT_IMPL_HPP_INCLUDED
