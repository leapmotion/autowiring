
//  (C) Copyright John Maddock 2015.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_DEFAULT_CONSTRUCTIBLE_HPP_INCLUDED
#define AUTOBOOST_TT_IS_DEFAULT_CONSTRUCTIBLE_HPP_INCLUDED

#include <cstddef> // size_t
#include <autoboost/type_traits/integral_constant.hpp>
#include <autoboost/detail/workaround.hpp>

#if AUTOBOOST_WORKAROUND(AUTOBOOST_GCC_VERSION, < 40700)
#include <autoboost/type_traits/is_abstract.hpp>
#endif

#if !defined(AUTOBOOST_NO_CXX11_DECLTYPE) && !AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, < 1800) && !AUTOBOOST_WORKAROUND(AUTOBOOST_GCC_VERSION, < 40500)

#include <autoboost/type_traits/detail/yes_no_type.hpp>

namespace autoboost{

   namespace detail{

      struct is_default_constructible_imp
      {
         template<typename _Tp, typename = decltype(_Tp())>
         static autoboost::type_traits::yes_type test(int);

         template<typename>
         static autoboost::type_traits::no_type test(...);
      };
#if AUTOBOOST_WORKAROUND(AUTOBOOST_GCC_VERSION, < 40700)
      template<class T, bool b>
      struct is_default_constructible_abstract_filter
      {
          static const bool value = sizeof(is_default_constructible_imp::test<T>(0)) == sizeof(autoboost::type_traits::yes_type);
      };
      template<class T>
      struct is_default_constructible_abstract_filter<T, true>
      {
          static const bool value = false;
      };
#endif
   }

#if AUTOBOOST_WORKAROUND(AUTOBOOST_GCC_VERSION, < 40700)
   template <class T> struct is_default_constructible : public integral_constant<bool, detail::is_default_constructible_abstract_filter<T, autoboost::is_abstract<T>::value>::value>{};
#else
   template <class T> struct is_default_constructible : public integral_constant<bool, sizeof(detail::is_default_constructible_imp::test<T>(0)) == sizeof(autoboost::type_traits::yes_type)>{};
#endif
   template <class T, std::size_t N> struct is_default_constructible<T[N]> : public is_default_constructible<T>{};
   template <class T> struct is_default_constructible<T[]> : public is_default_constructible<T>{};
   template <class T> struct is_default_constructible<T&> : public integral_constant<bool, false>{};
#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
   template <class T> struct is_default_constructible<T&&> : public integral_constant<bool, false>{};
#endif
   template <> struct is_default_constructible<void> : public integral_constant<bool, false>{};
   template <> struct is_default_constructible<void const> : public integral_constant<bool, false>{};
   template <> struct is_default_constructible<void volatile> : public integral_constant<bool, false>{};
   template <> struct is_default_constructible<void const volatile> : public integral_constant<bool, false>{};

#else

#include <autoboost/type_traits/is_pod.hpp>

namespace autoboost{

   // We don't know how to implement this, note we can not use has_trivial_constructor here
   // because the correct implementation of that trait requires this one:
   template <class T> struct is_default_constructible : public is_pod<T>{};
   template <> struct is_default_constructible<void> : public integral_constant<bool, false>{};
   template <> struct is_default_constructible<void const> : public integral_constant<bool, false>{};
   template <> struct is_default_constructible<void volatile> : public integral_constant<bool, false>{};
   template <> struct is_default_constructible<void const volatile> : public integral_constant<bool, false>{};

#endif

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_DEFAULT_CONSTRUCTIBLE_HPP_INCLUDED
