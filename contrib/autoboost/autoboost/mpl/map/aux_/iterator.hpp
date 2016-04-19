
#ifndef AUTOBOOST_MPL_MAP_AUX_ITERATOR_HPP_INCLUDED
#define AUTOBOOST_MPL_MAP_AUX_ITERATOR_HPP_INCLUDED

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

#include <autoboost/mpl/map/aux_/map0.hpp>
#include <autoboost/mpl/map/aux_/at_impl.hpp>
#include <autoboost/mpl/map/aux_/tag.hpp>
#include <autoboost/mpl/iterator_tags.hpp>
#include <autoboost/mpl/if.hpp>
#include <autoboost/mpl/next.hpp>
#include <autoboost/mpl/deref.hpp>
#include <autoboost/mpl/long.hpp>
#include <autoboost/mpl/void.hpp>
#include <autoboost/mpl/aux_/nttp_decl.hpp>
#include <autoboost/mpl/aux_/config/ctps.hpp>

namespace autoboost { namespace mpl {

#if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template<
      typename Map
    , long order
    , long max_order
    >
struct next_order
    : if_<
          is_void_< typename item_by_order<Map,order>::type >
        , next_order<Map,(order+1),max_order>
        , long_<order>
        >::type
{
};

template<
      typename Map
    , long max_order
    >
struct next_order<Map,max_order,max_order>
    : long_<max_order>
{
};


template< typename Map, long order, long max_order >
struct m_iter
{
    typedef forward_iterator_tag category;
    typedef typename item_by_order<Map,order>::type type;
};

template< typename Map, long max_order >
struct m_iter<Map,max_order,max_order>
{
    typedef forward_iterator_tag category;
};


template< typename Map, long order, long max_order >
struct next< m_iter<Map,order,max_order> >
{
    typedef m_iter<
          Map
        , next_order<Map,order+1,max_order>::value
        , max_order
        > type;
};

template< typename Map, long max_order >
struct next< m_iter<Map,max_order,max_order> >
{
};

#else

template<
      typename Map
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, order)
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, max_order)
    >
struct next_order;

template<
      typename Map
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, order)
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, max_order)
    >
struct next_order_impl
    : if_<
          is_void_< typename item_by_order<Map,order>::type >
        , next_order<Map,(order+1),max_order>
        , long_<order>
        >::type
    {
    };

template<
      typename Map
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, order)
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, max_order)
    >
struct next_order
    : if_c<
          (order != max_order)
        , next_order_impl<Map,order,max_order>
        , long_<order>
        >::type
{
};


template<
      typename Map
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, order)
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, max_order)
    >
struct m_iter;

struct m_iter_empty_base {};

template<
      typename Map
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, order)
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, max_order)
    >
struct m_iter_base
{
    typedef typename item_by_order<Map,order>::type type;

    typedef m_iter<
          Map
        , next_order<Map,order+1,max_order>::value
        , max_order
        > next;
};

template<
      typename Map
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, order)
    , AUTOBOOST_MPL_AUX_NTTP_DECL(long, max_order)
    >
struct m_iter
  : if_c<
          (order == max_order)
        , m_iter_empty_base
        , m_iter_base<Map,order,max_order>
        >::type
{
    typedef forward_iterator_tag category;
};

#endif // AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

}}

#endif // AUTOBOOST_MPL_MAP_AUX_ITERATOR_HPP_INCLUDED
