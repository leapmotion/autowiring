
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

#include <autoboost/type_traits/is_lvalue_reference.hpp>
#include <autoboost/type_traits/is_rvalue_reference.hpp>

namespace autoboost {

template <class T> struct is_reference
   : public
   integral_constant<
      bool,
      ::autoboost::is_lvalue_reference<T>::value || ::autoboost::is_rvalue_reference<T>::value>
{};

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_REFERENCE_HPP_INCLUDED

