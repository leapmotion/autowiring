
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
#include <autoboost/config.hpp>

namespace autoboost {

template <typename T>
struct is_stateless
 : public integral_constant<bool,
      (::autoboost::has_trivial_constructor<T>::value
      && ::autoboost::has_trivial_copy<T>::value
      && ::autoboost::has_trivial_destructor<T>::value
      && ::autoboost::is_class<T>::value
      && ::autoboost::is_empty<T>::value)>
{};

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_STATELESS_HPP_INCLUDED
