
//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_IS_MEMBER_OBJECT_POINTER_HPP_INCLUDED
#define AUTOBOOST_TT_IS_MEMBER_OBJECT_POINTER_HPP_INCLUDED

#include <autoboost/type_traits/config.hpp>
#include <autoboost/type_traits/is_member_pointer.hpp>
#include <autoboost/type_traits/is_member_function_pointer.hpp>
#include <autoboost/type_traits/detail/ice_and.hpp>
#include <autoboost/type_traits/detail/ice_not.hpp>

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

namespace detail{

template <typename T>
struct is_member_object_pointer_impl
{
   AUTOBOOST_STATIC_CONSTANT(
      bool, value = (::autoboost::type_traits::ice_and<
         ::autoboost::is_member_pointer<T>::value,
         ::autoboost::type_traits::ice_not<
            ::autoboost::is_member_function_pointer<T>::value
         >::value
      >::value ));
};

} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(is_member_object_pointer,T,::autoboost::detail::is_member_object_pointer_impl<T>::value)

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED
