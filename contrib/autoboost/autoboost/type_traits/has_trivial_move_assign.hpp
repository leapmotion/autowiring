
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  (C) Copyright Eric Friedman 2002-2003.
//  (C) Copyright Antony Polukhin 2013.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_TRIVIAL_MOVE_ASSIGN_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_TRIVIAL_MOVE_ASSIGN_HPP_INCLUDED

#include <autoboost/type_traits/config.hpp>
#include <autoboost/type_traits/is_pod.hpp>
#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#include <autoboost/type_traits/detail/ice_and.hpp>
#include <autoboost/type_traits/detail/ice_not.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

namespace detail {

template <typename T>
struct has_trivial_move_assign_impl
{
#ifdef AUTOBOOST_HAS_TRIVIAL_MOVE_ASSIGN
   AUTOBOOST_STATIC_CONSTANT(bool, value = (AUTOBOOST_HAS_TRIVIAL_MOVE_ASSIGN(T)));
#else
   AUTOBOOST_STATIC_CONSTANT(bool, value =
           (::autoboost::type_traits::ice_and<
              ::autoboost::is_pod<T>::value,
              ::autoboost::type_traits::ice_not< ::autoboost::is_const<T>::value >::value,
              ::autoboost::type_traits::ice_not< ::autoboost::is_volatile<T>::value >::value
           >::value));
#endif
};

} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(has_trivial_move_assign,T,::autoboost::detail::has_trivial_move_assign_impl<T>::value)
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_move_assign,void,false)
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_move_assign,void const,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_move_assign,void const volatile,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_move_assign,void volatile,false)
#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_HAS_TRIVIAL_MOVE_ASSIGN_HPP_INCLUDED
