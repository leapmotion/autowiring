
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_REMOVE_CONST_HPP_INCLUDED
#define AUTOBOOST_TT_REMOVE_CONST_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <cstddef>
#include <autoboost/detail/workaround.hpp>

namespace autoboost {

   //  convert a type T to a non-cv-qualified type - remove_const<T>
   template <class T> struct remove_const{ typedef T type; };
   template <class T> struct remove_const<T const>{ typedef T type; };

#if !defined(AUTOBOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
   template <class T, std::size_t N> struct remove_const<T const[N]>{ typedef T type[N]; };
#if !AUTOBOOST_WORKAROUND(__BORLANDC__, < 0x600) && !defined(__IBMCPP__) &&  !AUTOBOOST_WORKAROUND(__DMC__, AUTOBOOST_TESTED_AT(0x840))
   template <class T> struct remove_const<T const[]>{ typedef T type[]; };
#endif
#endif

} // namespace autoboost

#endif // AUTOBOOST_TT_REMOVE_CONST_HPP_INCLUDED
