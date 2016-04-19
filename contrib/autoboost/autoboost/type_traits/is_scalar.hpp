
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_SCALAR_HPP_INCLUDED
#define AUTOBOOST_TT_IS_SCALAR_HPP_INCLUDED

#include <autoboost/type_traits/is_arithmetic.hpp>
#include <autoboost/type_traits/is_enum.hpp>
#include <autoboost/type_traits/is_pointer.hpp>
#include <autoboost/type_traits/is_member_pointer.hpp>
#include <autoboost/config.hpp>

namespace autoboost {

template <typename T>
struct is_scalar
   : public integral_constant<bool, ::autoboost::is_arithmetic<T>::value || ::autoboost::is_enum<T>::value || ::autoboost::is_pointer<T>::value || ::autoboost::is_member_pointer<T>::value>
{};

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_SCALAR_HPP_INCLUDED
