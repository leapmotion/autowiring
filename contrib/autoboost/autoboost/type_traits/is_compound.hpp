
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_COMPOUND_HPP_INCLUDED
#define AUTOBOOST_TT_IS_COMPOUND_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/type_traits/is_fundamental.hpp>
#include <autoboost/type_traits/detail/ice_not.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

#if !defined( __CODEGEARC__ )
namespace detail {

template <typename T>
struct is_compound_impl
{
   AUTOBOOST_STATIC_CONSTANT(bool, value =
      (::autoboost::type_traits::ice_not<
         ::autoboost::is_fundamental<T>::value
       >::value));
};

} // namespace detail
#endif // !defined( __CODEGEARC__ )

#if defined( __CODEGEARC__ )
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_compound,T,__is_compound(T))
#else
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_compound,T,::autoboost::detail::is_compound_impl<T>::value)
#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_COMPOUND_HPP_INCLUDED
