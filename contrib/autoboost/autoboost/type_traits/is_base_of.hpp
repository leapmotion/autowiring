
//  (C) Copyright Rani Sharoni 2003-2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_BASE_OF_HPP_INCLUDED
#define AUTOBOOST_TT_IS_BASE_OF_HPP_INCLUDED

#include <autoboost/type_traits/is_base_and_derived.hpp>
#include <autoboost/type_traits/is_same.hpp>
#include <autoboost/type_traits/is_class.hpp>

namespace autoboost {

   namespace detail{
      template <class B, class D>
      struct is_base_of_imp
      {
          typedef typename remove_cv<B>::type ncvB;
          typedef typename remove_cv<D>::type ncvD;
          AUTOBOOST_STATIC_CONSTANT(bool, value = (
            (::autoboost::detail::is_base_and_derived_impl<ncvB,ncvD>::value) ||
            (::autoboost::is_same<ncvB,ncvD>::value && ::autoboost::is_class<ncvB>::value)));
      };
   }

   template <class Base, class Derived> struct is_base_of
      : public integral_constant<bool, (::autoboost::detail::is_base_of_imp<Base, Derived>::value)> {};

   template <class Base, class Derived> struct is_base_of<Base, Derived&> : false_type{};
   template <class Base, class Derived> struct is_base_of<Base&, Derived&> : false_type{};
   template <class Base, class Derived> struct is_base_of<Base&, Derived> : false_type{};

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_BASE_AND_DERIVED_HPP_INCLUDED
