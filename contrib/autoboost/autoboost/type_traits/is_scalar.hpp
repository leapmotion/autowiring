
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_SCALAR_HPP_INCLUDED
#define AUTOBOOST_TT_IS_SCALAR_HPP_INCLUDED

#include <autoboost/type_traits/is_arithmetic.hpp>
#include <autoboost/type_traits/is_enum.hpp>
#include <autoboost/type_traits/is_pointer.hpp>
#include <autoboost/type_traits/is_member_pointer.hpp>
#include <autoboost/type_traits/detail/ice_or.hpp>
#include <autoboost/config.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

namespace detail {

template <typename T>
struct is_scalar_impl
{
   AUTOBOOST_STATIC_CONSTANT(bool, value =
      (::autoboost::type_traits::ice_or<
         ::autoboost::is_arithmetic<T>::value,
         ::autoboost::is_enum<T>::value,
         ::autoboost::is_pointer<T>::value,
         ::autoboost::is_member_pointer<T>::value
      >::value));
};

// these specializations are only really needed for compilers
// without partial specialization support:
template <> struct is_scalar_impl<void>{ AUTOBOOST_STATIC_CONSTANT(bool, value = false ); };
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
template <> struct is_scalar_impl<void const>{ AUTOBOOST_STATIC_CONSTANT(bool, value = false ); };
template <> struct is_scalar_impl<void volatile>{ AUTOBOOST_STATIC_CONSTANT(bool, value = false ); };
template <> struct is_scalar_impl<void const volatile>{ AUTOBOOST_STATIC_CONSTANT(bool, value = false ); };
#endif

} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_scalar,T,::autoboost::detail::is_scalar_impl<T>::value)

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_SCALAR_HPP_INCLUDED
