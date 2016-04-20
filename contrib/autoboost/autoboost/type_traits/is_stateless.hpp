
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_STATELESS_HPP_INCLUDED
#define AUTOBOOST_TT_IS_STATELESS_HPP_INCLUDED

#include <autoboost/type_traits/has_trivial_constructor.hpp>
#include <autoboost/type_traits/has_trivial_copy.hpp>
#include <autoboost/type_traits/has_trivial_destructor.hpp>
#include <autoboost/type_traits/is_class.hpp>
#include <autoboost/type_traits/is_empty.hpp>
#include <autoboost/type_traits/detail/ice_and.hpp>
#include <autoboost/config.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

namespace detail {

template <typename T>
struct is_stateless_impl
{
  AUTOBOOST_STATIC_CONSTANT(bool, value =
    (::autoboost::type_traits::ice_and<
       ::autoboost::has_trivial_constructor<T>::value,
       ::autoboost::has_trivial_copy<T>::value,
       ::autoboost::has_trivial_destructor<T>::value,
       ::autoboost::is_class<T>::value,
       ::autoboost::is_empty<T>::value
     >::value));
};

} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_stateless,T,::autoboost::detail::is_stateless_impl<T>::value)

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_STATELESS_HPP_INCLUDED
