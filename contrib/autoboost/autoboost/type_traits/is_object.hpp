
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_OBJECT_HPP_INCLUDED
#define AUTOBOOST_TT_IS_OBJECT_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/type_traits/is_reference.hpp>
#include <autoboost/type_traits/is_void.hpp>
#include <autoboost/type_traits/is_function.hpp>

namespace autoboost {

template <class T> struct is_object
   : public
      integral_constant<
         bool,
         ! ::autoboost::is_reference<T>::value && ! ::autoboost::is_void<T>::value && ! ::autoboost::is_function<T>::value >
{};

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_OBJECT_HPP_INCLUDED
