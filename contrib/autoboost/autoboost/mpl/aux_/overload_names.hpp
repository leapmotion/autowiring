
#ifndef AUTOBOOST_MPL_AUX_OVERLOAD_NAMES_HPP_INCLUDED
#define AUTOBOOST_MPL_AUX_OVERLOAD_NAMES_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2004
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <autoboost/mpl/aux_/ptr_to_ref.hpp>
#include <autoboost/mpl/aux_/config/operators.hpp>

#if defined(AUTOBOOST_MPL_CFG_USE_OPERATORS_OVERLOADING)

#   include <autoboost/mpl/aux_/static_cast.hpp>

#   define AUTOBOOST_MPL_AUX_OVERLOAD_VALUE_BY_KEY  operator/
#   define AUTOBOOST_MPL_AUX_OVERLOAD_ITEM_BY_ORDER operator|
#   define AUTOBOOST_MPL_AUX_OVERLOAD_ORDER_BY_KEY  operator||
#   define AUTOBOOST_MPL_AUX_OVERLOAD_IS_MASKED     operator%

#   define AUTOBOOST_MPL_AUX_OVERLOAD_CALL_VALUE_BY_KEY(T, x)   AUTOBOOST_MPL_AUX_PTR_TO_REF(T) / x
#   define AUTOBOOST_MPL_AUX_OVERLOAD_CALL_ITEM_BY_ORDER(T, x)  AUTOBOOST_MPL_AUX_PTR_TO_REF(T) | x
#   define AUTOBOOST_MPL_AUX_OVERLOAD_CALL_ORDER_BY_KEY(T, x)   AUTOBOOST_MPL_AUX_PTR_TO_REF(T) || x
#   define AUTOBOOST_MPL_AUX_OVERLOAD_CALL_IS_MASKED(T, x)      AUTOBOOST_MPL_AUX_PTR_TO_REF(T) % x

#else

#   define AUTOBOOST_MPL_AUX_OVERLOAD_VALUE_BY_KEY  value_by_key_
#   define AUTOBOOST_MPL_AUX_OVERLOAD_ITEM_BY_ORDER item_by_order_
#   define AUTOBOOST_MPL_AUX_OVERLOAD_ORDER_BY_KEY  order_by_key_
#   define AUTOBOOST_MPL_AUX_OVERLOAD_IS_MASKED     is_masked_

#   define AUTOBOOST_MPL_AUX_OVERLOAD_CALL_VALUE_BY_KEY(T, x)   T::AUTOBOOST_MPL_AUX_OVERLOAD_VALUE_BY_KEY( AUTOBOOST_MPL_AUX_PTR_TO_REF(T), x )
#   define AUTOBOOST_MPL_AUX_OVERLOAD_CALL_ITEM_BY_ORDER(T, x)  T::AUTOBOOST_MPL_AUX_OVERLOAD_ITEM_BY_ORDER( AUTOBOOST_MPL_AUX_PTR_TO_REF(T), x )
#   define AUTOBOOST_MPL_AUX_OVERLOAD_CALL_ORDER_BY_KEY(T, x)   T::AUTOBOOST_MPL_AUX_OVERLOAD_ORDER_BY_KEY( AUTOBOOST_MPL_AUX_PTR_TO_REF(T), x )
#   define AUTOBOOST_MPL_AUX_OVERLOAD_CALL_IS_MASKED(T, x)      T::AUTOBOOST_MPL_AUX_OVERLOAD_IS_MASKED( AUTOBOOST_MPL_AUX_PTR_TO_REF(T), x )

#endif

#endif // AUTOBOOST_MPL_AUX_OVERLOAD_NAMES_HPP_INCLUDED
