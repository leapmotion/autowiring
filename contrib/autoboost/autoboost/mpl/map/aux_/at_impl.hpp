
#ifndef AUTOBOOST_MPL_MAP_AUX_AT_IMPL_HPP_INCLUDED
#define AUTOBOOST_MPL_MAP_AUX_AT_IMPL_HPP_INCLUDED

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

#include <autoboost/mpl/at_fwd.hpp>
#include <autoboost/mpl/long.hpp>
#include <autoboost/mpl/map/aux_/tag.hpp>
#include <autoboost/mpl/aux_/order_impl.hpp>
#include <autoboost/mpl/aux_/overload_names.hpp>
#include <autoboost/mpl/aux_/type_wrapper.hpp>
#include <autoboost/mpl/aux_/ptr_to_ref.hpp>
#include <autoboost/mpl/aux_/static_cast.hpp>
#include <autoboost/mpl/aux_/config/typeof.hpp>
#include <autoboost/mpl/aux_/config/ctps.hpp>

#if !defined(AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES)
#   include <autoboost/mpl/eval_if.hpp>
#   include <autoboost/mpl/pair.hpp>
#   include <autoboost/mpl/void.hpp>
#   include <autoboost/mpl/aux_/config/static_constant.hpp>
#endif

namespace autoboost { namespace mpl {

#if defined(AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES)

template< typename Map, typename Key >
struct m_at
{
    typedef aux::type_wrapper<Key> key_;
    typedef __typeof__( AUTOBOOST_MPL_AUX_OVERLOAD_CALL_VALUE_BY_KEY(
          Map
        , AUTOBOOST_MPL_AUX_STATIC_CAST(key_*, 0)
        ) ) type;
};

template<>
struct at_impl< aux::map_tag >
{
    template< typename Map, typename Key > struct apply
        : aux::wrapped_type< typename m_at<
              Map
            , Key
            >::type >
    {
    };
};

// agurt 31/jan/04: two-step implementation for the sake of GCC 3.x
template< typename Map, long order >
struct item_by_order_impl
{
    typedef __typeof__( AUTOBOOST_MPL_AUX_OVERLOAD_CALL_ITEM_BY_ORDER(
          Map
        , AUTOBOOST_MPL_AUX_STATIC_CAST(long_<order>*, 0)
        ) ) type;
};

template< typename Map, long order >
struct item_by_order
    : aux::wrapped_type<
          typename item_by_order_impl<Map,order>::type
        >
{
};

#else // AUTOBOOST_MPL_CFG_TYPEOF_BASED_SEQUENCES

#   if !defined(AUTOBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template< typename Map, long n > struct m_at
{
    typedef void_ type;
};

#   else

template< long n > struct m_at_impl
{
    template< typename Map > struct result_
    {
        typedef void_ type;
    };
};

template< typename Map, long n > struct m_at
{
    typedef typename m_at_impl<n>::result_<Map>::type type;
};

#   endif


template<>
struct at_impl< aux::map_tag >
{
    template< typename Map, typename Key > struct apply
    {
        typedef typename m_at< Map, (x_order_impl<Map,Key>::value - 2) >::type item_;
        typedef typename eval_if<
              is_void_<item_>
            , void_
            , second<item_>
            >::type type;
    };
};

template< typename Map, long order > struct is_item_masked
{
    AUTOBOOST_STATIC_CONSTANT(bool, value =
          sizeof( AUTOBOOST_MPL_AUX_OVERLOAD_CALL_IS_MASKED(
              Map
            , AUTOBOOST_MPL_AUX_STATIC_CAST(long_<order>*, 0)
            ) ) == sizeof(aux::yes_tag)
        );
};

template< typename Map, long order > struct item_by_order
{
    typedef typename eval_if_c<
          is_item_masked<Map,order>::value
        , void_
        , m_at<Map,(order - 2)>
        >::type type;
};

#endif

}}

#endif // AUTOBOOST_MPL_SET_AUX_AT_IMPL_HPP_INCLUDED
