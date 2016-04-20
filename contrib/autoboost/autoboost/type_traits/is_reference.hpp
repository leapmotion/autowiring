
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes,
//      Howard Hinnant and John Maddock 2000, 2010.
//  (C) Copyright Mat Marcus, Jesse Jones and Adobe Systems Inc 2001

//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_REFERENCE_HPP_INCLUDED
#define AUTOBOOST_TT_IS_REFERENCE_HPP_INCLUDED

#include <autoboost/type_traits/config.hpp>
#include <autoboost/type_traits/is_lvalue_reference.hpp>
#include <autoboost/type_traits/is_rvalue_reference.hpp>
#include <autoboost/type_traits/ice.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

namespace detail {

template <typename T>
struct is_reference_impl
{
   AUTOBOOST_STATIC_CONSTANT(bool, value =
      (::autoboost::type_traits::ice_or<
         ::autoboost::is_lvalue_reference<T>::value, ::autoboost::is_rvalue_reference<T>::value
       >::value));
};

} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_reference,T,::autoboost::detail::is_reference_impl<T>::value)

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_REFERENCE_HPP_INCLUDED

