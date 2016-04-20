
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef AUTOBOOST_TT_DETAIL_CV_TRAITS_IMPL_HPP_INCLUDED
#define AUTOBOOST_TT_DETAIL_CV_TRAITS_IMPL_HPP_INCLUDED

#include <cstddef>
#include <autoboost/config.hpp>
#include <autoboost/detail/workaround.hpp>


// implementation helper:


namespace autoboost {
namespace detail {

#if AUTOBOOST_WORKAROUND(AUTOBOOST_MSVC, == 1700)
#define AUTOBOOST_TT_AUX_CV_TRAITS_IMPL_PARAM(X) X
   template <typename T>
   struct cv_traits_imp
   {
      AUTOBOOST_STATIC_CONSTANT(bool, is_const = false);
      AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = false);
      typedef T unqualified_type;
   };

   template <typename T>
   struct cv_traits_imp<T[]>
   {
      AUTOBOOST_STATIC_CONSTANT(bool, is_const = false);
      AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = false);
      typedef T unqualified_type[];
   };

   template <typename T>
   struct cv_traits_imp<const T[]>
   {
      AUTOBOOST_STATIC_CONSTANT(bool, is_const = true);
      AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = false);
      typedef T unqualified_type[];
   };

   template <typename T>
   struct cv_traits_imp<volatile T[]>
   {
      AUTOBOOST_STATIC_CONSTANT(bool, is_const = false);
      AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = true);
      typedef T unqualified_type[];
   };

   template <typename T>
   struct cv_traits_imp<const volatile T[]>
   {
      AUTOBOOST_STATIC_CONSTANT(bool, is_const = true);
      AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = true);
      typedef T unqualified_type[];
   };

   template <typename T, std::size_t N>
   struct cv_traits_imp<T[N]>
   {
      AUTOBOOST_STATIC_CONSTANT(bool, is_const = false);
      AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = false);
      typedef T unqualified_type[N];
   };

   template <typename T, std::size_t N>
   struct cv_traits_imp<const T[N]>
   {
      AUTOBOOST_STATIC_CONSTANT(bool, is_const = true);
      AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = false);
      typedef T unqualified_type[N];
   };

   template <typename T, std::size_t N>
   struct cv_traits_imp<volatile T[N]>
   {
      AUTOBOOST_STATIC_CONSTANT(bool, is_const = false);
      AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = true);
      typedef T unqualified_type[N];
   };

   template <typename T, std::size_t N>
   struct cv_traits_imp<const volatile T[N]>
   {
      AUTOBOOST_STATIC_CONSTANT(bool, is_const = true);
      AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = true);
      typedef T unqualified_type[N];
   };

#else
#define AUTOBOOST_TT_AUX_CV_TRAITS_IMPL_PARAM(X) X *
template <typename T> struct cv_traits_imp {};

template <typename T>
struct cv_traits_imp<T*>
{
    AUTOBOOST_STATIC_CONSTANT(bool, is_const = false);
    AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = false);
    typedef T unqualified_type;
};
#endif

template <typename T>
struct cv_traits_imp<AUTOBOOST_TT_AUX_CV_TRAITS_IMPL_PARAM(const T)>
{
    AUTOBOOST_STATIC_CONSTANT(bool, is_const = true);
    AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = false);
    typedef T unqualified_type;
};

template <typename T>
struct cv_traits_imp<AUTOBOOST_TT_AUX_CV_TRAITS_IMPL_PARAM(volatile T)>
{
    AUTOBOOST_STATIC_CONSTANT(bool, is_const = false);
    AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = true);
    typedef T unqualified_type;
};

template <typename T>
struct cv_traits_imp<AUTOBOOST_TT_AUX_CV_TRAITS_IMPL_PARAM(const volatile T)>
{
    AUTOBOOST_STATIC_CONSTANT(bool, is_const = true);
    AUTOBOOST_STATIC_CONSTANT(bool, is_volatile = true);
    typedef T unqualified_type;
};

} // namespace detail
} // namespace autoboost


#endif // AUTOBOOST_TT_DETAIL_CV_TRAITS_IMPL_HPP_INCLUDED
