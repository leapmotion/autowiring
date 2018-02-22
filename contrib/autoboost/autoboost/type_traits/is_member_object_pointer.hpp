
//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_IS_MEMBER_OBJECT_POINTER_HPP_INCLUDED
#define AUTOBOOST_TT_IS_MEMBER_OBJECT_POINTER_HPP_INCLUDED

#include <autoboost/type_traits/is_member_pointer.hpp>
#include <autoboost/type_traits/is_member_function_pointer.hpp>

namespace autoboost {

template <class T> struct is_member_object_pointer
   : public integral_constant<bool,
   ::autoboost::is_member_pointer<T>::value && !::autoboost::is_member_function_pointer<T>::value>{};

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED
