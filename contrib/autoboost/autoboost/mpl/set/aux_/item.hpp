
#ifndef AUTOBOOST_MPL_SET_AUX_ITEM_HPP_INCLUDED
#define AUTOBOOST_MPL_SET_AUX_ITEM_HPP_INCLUDED

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

#include <autoboost/mpl/long.hpp>
#include <autoboost/mpl/void.hpp>
#include <autoboost/mpl/next.hpp>
#include <autoboost/mpl/prior.hpp>
#include <autoboost/mpl/set/aux_/set0.hpp>
#include <autoboost/mpl/aux_/type_wrapper.hpp>
#include <autoboost/mpl/aux_/config/arrays.hpp>

namespace autoboost { namespace mpl {

template< typename T, typename Base >
struct s_item
    : Base
{
    typedef s_item<T,Base> item_;
    typedef void_       last_masked_;
    typedef T           item_type_;
    typedef typename Base::item_ base;
    typedef s_item type;

    typedef typename next< typename Base::size >::type  size;
    typedef typename next< typename Base::order >::type order;

#if defined(AUTOBOOST_MPL_CFG_NO_DEPENDENT_ARRAY_TYPES)
    typedef typename aux::weighted_tag<AUTOBOOST_MPL_AUX_MSVC_VALUE_WKND(order)::value>::type order_tag_;
#else
    typedef char (&order_tag_)[AUTOBOOST_MPL_AUX_MSVC_VALUE_WKND(order)::value];
#endif

    AUTOBOOST_MPL_AUX_SET_OVERLOAD( order_tag_, ORDER_BY_KEY, s_item, aux::type_wrapper<T>* );
    AUTOBOOST_MPL_AUX_SET_OVERLOAD( aux::no_tag, IS_MASKED, s_item, aux::type_wrapper<T>* );
};


template< typename T, typename Base >
struct s_mask
    : Base
{
    typedef s_mask<T,Base> item_;
    typedef T       last_masked_;
    typedef void_   item_type_;
    typedef typename Base::item_ base;
    typedef typename prior< typename Base::size >::type  size;
    typedef s_mask type;

    AUTOBOOST_MPL_AUX_SET_OVERLOAD( aux::yes_tag, IS_MASKED, s_mask, aux::type_wrapper<T>* );
};


template< typename T, typename Base >
struct s_unmask
    : Base
{
    typedef s_unmask<T,Base> item_;
    typedef void_   last_masked_;
    typedef T       item_type_;
    typedef typename Base::item_ base;
    typedef typename next< typename Base::size >::type  size;

    AUTOBOOST_MPL_AUX_SET_OVERLOAD( aux::no_tag, IS_MASKED, s_unmask, aux::type_wrapper<T>* );
};

}}

#endif // AUTOBOOST_MPL_SET_AUX_ITEM_HPP_INCLUDED
