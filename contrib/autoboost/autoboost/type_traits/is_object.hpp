
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_OBJECT_HPP_INCLUDED
#define AUTOBOOST_TT_IS_OBJECT_HPP_INCLUDED

#include <autoboost/type_traits/is_reference.hpp>
#include <autoboost/type_traits/is_void.hpp>
#include <autoboost/type_traits/is_function.hpp>
#include <autoboost/type_traits/detail/ice_and.hpp>
#include <autoboost/type_traits/detail/ice_not.hpp>
#include <autoboost/config.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

namespace detail {

template <typename T>
struct is_object_impl
{
   AUTOBOOST_STATIC_CONSTANT(bool, value =
      (::autoboost::type_traits::ice_and<
         ::autoboost::type_traits::ice_not< ::autoboost::is_reference<T>::value>::value,
         ::autoboost::type_traits::ice_not< ::autoboost::is_void<T>::value>::value,
         ::autoboost::type_traits::ice_not< ::autoboost::is_function<T>::value>::value
      >::value));
};

} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_object,T,::autoboost::detail::is_object_impl<T>::value)

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_OBJECT_HPP_INCLUDED
