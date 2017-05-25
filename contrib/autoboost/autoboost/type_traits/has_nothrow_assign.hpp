
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_NOTHROW_ASSIGN_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_NOTHROW_ASSIGN_HPP_INCLUDED

#include <cstddef> // size_t
#include <autoboost/type_traits/integral_constant.hpp>
#include <autoboost/type_traits/intrinsics.hpp>

#if !defined(AUTOBOOST_HAS_NOTHROW_ASSIGN) || defined(AUTOBOOST_MSVC) || defined(AUTOBOOST_INTEL)
#include <autoboost/type_traits/has_trivial_assign.hpp>
#if !defined(AUTOBOOST_NO_CXX11_NOEXCEPT) && !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
#include <autoboost/type_traits/declval.hpp>
#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#include <autoboost/type_traits/is_reference.hpp>
#include <autoboost/type_traits/is_assignable.hpp>
#include <autoboost/type_traits/add_reference.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#endif
#endif
#if defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__clang__)
#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#include <autoboost/type_traits/is_assignable.hpp>
#include <autoboost/type_traits/is_array.hpp>
#ifdef AUTOBOOST_INTEL
#include <autoboost/type_traits/is_pod.hpp>
#endif
#endif

namespace autoboost {

#if !defined(AUTOBOOST_HAS_NOTHROW_ASSIGN) && !defined(AUTOBOOST_NO_CXX11_NOEXCEPT) && !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)

   namespace detail
   {
      template <class T, bool b1, bool b2> struct has_nothrow_assign_imp{ static const bool value = false; };
      template <class T>          struct has_nothrow_assign_imp<T, false, true>{ static const bool value = noexcept(autoboost::declval<typename add_reference<T>::type>() = autoboost::declval<typename add_reference<T const>::type>()); };
      template <class T, std::size_t N> struct has_nothrow_assign_imp<T[N], false, true>{ static const bool value = has_nothrow_assign_imp<T, false, true>::value; };
      template <class T>          struct has_nothrow_assign_imp<T[], false, true>{ static const bool value = has_nothrow_assign_imp<T, false, true>::value; };
   }

#endif

   template <class T>
   struct has_nothrow_assign : public integral_constant < bool,
#ifndef AUTOBOOST_HAS_NOTHROW_ASSIGN
#if !defined(AUTOBOOST_NO_CXX11_NOEXCEPT) && !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
      // Portable C++11 version:
      detail::has_nothrow_assign_imp<T,
      (is_const<typename remove_reference<T>::type>::value || is_volatile<typename remove_reference<T>::type>::value || is_reference<T>::value),
      is_assignable<typename add_reference<T>::type, typename add_reference<const T>::type>::value
      >::value
#else
      ::autoboost::has_trivial_assign<T>::value
#endif
#else
      AUTOBOOST_HAS_NOTHROW_ASSIGN(T)
#endif
   > {};

template <class T, std::size_t N> struct has_nothrow_assign <T[N]> : public has_nothrow_assign<T> {};
template <> struct has_nothrow_assign<void> : public false_type{};
template <class T> struct has_nothrow_assign<T volatile> : public false_type{};
template <class T> struct has_nothrow_assign<T&> : public false_type{};
#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
template <class T> struct has_nothrow_assign<T&&> : public false_type{};
#endif
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
template <> struct has_nothrow_assign<void const> : public false_type{};
template <> struct has_nothrow_assign<void const volatile> : public false_type{};
template <> struct has_nothrow_assign<void volatile> : public false_type{};
#endif

} // namespace autoboost

#endif // AUTOBOOST_TT_HAS_NOTHROW_ASSIGN_HPP_INCLUDED
