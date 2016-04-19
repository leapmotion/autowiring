
#ifndef AUTOBOOST_MPL_LIST_AUX_PUSH_BACK_HPP_INCLUDED
#define AUTOBOOST_MPL_LIST_AUX_PUSH_BACK_HPP_INCLUDED

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

#include <autoboost/mpl/push_back_fwd.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/mpl/list/aux_/tag.hpp>

namespace autoboost { namespace mpl {

template< typename Tag > struct has_push_back_impl;

template<>
struct has_push_back_impl< aux::list_tag >
{
    template< typename Seq > struct apply
        : false_
    {
    };
};

}}

#endif // AUTOBOOST_MPL_LIST_AUX_PUSH_BACK_HPP_INCLUDED
