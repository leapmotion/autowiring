
//  (C) Copyright John Maddock 2015.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_ASSIGNABLE_HPP_INCLUDED
#define AUTOBOOST_TT_IS_ASSIGNABLE_HPP_INCLUDED

#include <autoboost/type_traits/integral_constant.hpp>
#include <autoboost/detail/workaround.hpp>

namespace autoboost{

   template <class T, class U = T> struct is_assignable;

}

#if !defined(AUTOBOOST_NO_CXX11_DECLTYPE) && !AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1800)

#include <autoboost/type_traits/detail/yes_no_type.hpp>
#include <autoboost/type_traits/declval.hpp>

namespace autoboost{

   namespace detail{

      struct is_assignable_imp
      {
         template<typename T, typename U, typename = decltype(autoboost::declval<T>() = autoboost::declval<U>())>
         static autoboost::type_traits::yes_type test(int);

         template<typename, typename>
         static autoboost::type_traits::no_type test(...);
      };

   }

   template <class T, class U> struct is_assignable : public integral_constant<bool, sizeof(detail::is_assignable_imp::test<T, U>(0)) == sizeof(autoboost::type_traits::yes_type)>{};
   template <class T, std::size_t N, class U> struct is_assignable<T[N], U> : public is_assignable<T, U>{};
   template <class T, std::size_t N, class U> struct is_assignable<T(&)[N], U> : public is_assignable<T&, U>{};
   template <class T, class U> struct is_assignable<T[], U> : public is_assignable<T, U>{};
   template <class T, class U> struct is_assignable<T(&)[], U> : public is_assignable<T&, U>{};
   template <class U> struct is_assignable<void, U> : public integral_constant<bool, false>{};
   template <class U> struct is_assignable<void const, U> : public integral_constant<bool, false>{};
   template <class U> struct is_assignable<void volatile, U> : public integral_constant<bool, false>{};
   template <class U> struct is_assignable<void const volatile, U> : public integral_constant<bool, false>{};

#else

#include <autoboost/type_traits/has_trivial_assign.hpp>
#include <autoboost/type_traits/remove_reference.hpp>

namespace autoboost{

   // We don't know how to implement this:
   template <class T, class U> struct is_assignable : public integral_constant<bool, false>{};
   template <class T, class U> struct is_assignable<T&, U> : public integral_constant<bool, is_pod<T>::value && is_pod<typename remove_reference<U>::type>::value>{};
   template <class T, class U> struct is_assignable<const T&, U> : public integral_constant<bool, false>{};
   template <class U> struct is_assignable<void, U> : public integral_constant<bool, false>{};
   template <class U> struct is_assignable<void const, U> : public integral_constant<bool, false>{};
   template <class U> struct is_assignable<void volatile, U> : public integral_constant<bool, false>{};
   template <class U> struct is_assignable<void const volatile, U> : public integral_constant<bool, false>{};
   /*
   template <> struct is_assignable<void, void> : public integral_constant<bool, false>{};
   template <> struct is_assignable<void const, void const> : public integral_constant<bool, false>{};
   template <> struct is_assignable<void volatile, void volatile> : public integral_constant<bool, false>{};
   template <> struct is_assignable<void const volatile, void const volatile> : public integral_constant<bool, false>{};
   */
#endif

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_ASSIGNABLE_HPP_INCLUDED
