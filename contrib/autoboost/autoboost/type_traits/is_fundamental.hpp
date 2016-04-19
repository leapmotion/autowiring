
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_FUNDAMENTAL_HPP_INCLUDED
#define AUTOBOOST_TT_IS_FUNDAMENTAL_HPP_INCLUDED

#include <autoboost/type_traits/is_arithmetic.hpp>
#include <autoboost/type_traits/is_void.hpp>

namespace autoboost {

//* is a type T a fundamental type described in the standard (3.9.1)
#if defined( __CODEGEARC__ )
template <class T> struct is_fundamental : public integral_constant<bool, __is_fundamental(T)> {};
#else
template <class T> struct is_fundamental : public integral_constant<bool, ::autoboost::is_arithmetic<T>::value || ::autoboost::is_void<T>::value> {};
#endif

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_FUNDAMENTAL_HPP_INCLUDED
