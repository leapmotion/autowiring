
//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_HAS_VIRTUAL_DESTRUCTOR_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_VIRTUAL_DESTRUCTOR_HPP_INCLUDED

#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/integral_constant.hpp>

namespace autoboost {

#ifdef AUTOBOOST_HAS_VIRTUAL_DESTRUCTOR
   template <class T> struct has_virtual_destructor : public integral_constant<bool, AUTOBOOST_HAS_VIRTUAL_DESTRUCTOR(T)>{};
#else
   template <class T> struct has_virtual_destructor : public integral_constant<bool, false>{};
#endif

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_MEMBER_FUNCTION_POINTER_HPP_INCLUDED
