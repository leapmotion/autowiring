
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_TRIVIAL_ASSIGN_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_TRIVIAL_ASSIGN_HPP_INCLUDED

#include <cstddef> // size_t
#include <autoboost/type_traits/detail/config.hpp>
#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/integral_constant.hpp>

#if !defined(AUTOBOOST_HAS_TRIVIAL_ASSIGN) || defined(AUTOBOOST_MSVC) || defined(__GNUC__) || defined(AUTOBOOST_INTEL) || defined(__SUNPRO_CC) || defined(__clang__)
#include <autoboost/type_traits/is_pod.hpp>
#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#include <autoboost/type_traits/is_assignable.hpp>
#endif

namespace autoboost {

   template <typename T>
   struct has_trivial_assign : public integral_constant < bool,
#ifdef AUTOBOOST_HAS_TRIVIAL_ASSIGN
      AUTOBOOST_HAS_TRIVIAL_ASSIGN(T)
#else
      ::autoboost::is_pod<T>::value && !::autoboost::is_const<T>::value && !::autoboost::is_volatile<T>::value
#endif
   > {};

   template<> struct has_trivial_assign<void> : public false_type{};
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
   template<> struct has_trivial_assign<void const> : public false_type{};
   template<> struct has_trivial_assign<void const volatile> : public false_type{};
   template<> struct has_trivial_assign<void volatile> : public false_type{};
#endif
   template <class T> struct has_trivial_assign<T volatile> : public false_type{};
   template <class T> struct has_trivial_assign<T&> : public false_type{};
#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
   template <class T> struct has_trivial_assign<T&&> : public false_type{};
#endif
   // Arrays are not explictly assignable:
   template <typename T, std::size_t N> struct has_trivial_assign<T[N]> : public false_type{};
   template <typename T> struct has_trivial_assign<T[]> : public false_type{};

} // namespace autoboost

#endif // AUTOBOOST_TT_HAS_TRIVIAL_ASSIGN_HPP_INCLUDED
