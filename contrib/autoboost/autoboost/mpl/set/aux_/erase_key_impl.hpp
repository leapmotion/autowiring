
#ifndef AUTOBOOST_MPL_SET_AUX_ERASE_KEY_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_SET_AUX_ERASE_KEY_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2007
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

#include <autoboost/mpl/erase_key_fwd.hpp>
#include <autoboost/mpl/set/aux_/has_key_impl.hpp>
#include <autoboost/mpl/set/aux_/item.hpp>
#include <autoboost/mpl/set/aux_/tag.hpp>
#include <autoboost/mpl/identity.hpp>
#include <autoboost/mpl/base.hpp>
#include <autoboost/mpl/eval_if.hpp>

#include <autoboost/type_traits/is_same.hpp>

namespace autoboost { namespace mpl {

template<>
struct erase_key_impl< aux::set_tag >
{
    template<
          typename Set
        , typename T
        >
    struct apply
        : eval_if<
              has_key_impl<aux::set_tag>::apply<Set,T>
            , eval_if<
                  is_same< T,typename Set::item_type_ >
                , base<Set>
                , identity< s_mask<T,typename Set::item_> >
                >
            , identity<Set>
            >
    {
    };
};

}}

#endif // AUTOBOOST_MPL_SET_AUX_ERASE_KEY_IMPL_HPP_INCLUDED
