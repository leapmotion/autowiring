
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_TRIVIAL_COPY_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_TRIVIAL_COPY_HPP_INCLUDED

#include <autoboost/type_traits/config.hpp>
#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#include <autoboost/type_traits/is_pod.hpp>
#include <autoboost/type_traits/detail/ice_and.hpp>
#include <autoboost/type_traits/detail/ice_or.hpp>
#include <autoboost/type_traits/detail/ice_not.hpp>

#ifdef __clang__
#include <autoboost/type_traits/is_copy_constructible.hpp>
#endif

// should be the last #include
#include <autoboost/type_traits/detail/bool_trait_def.hpp>

namespace autoboost {

namespace detail {

template <typename T>
struct has_trivial_copy_impl
{
#ifdef AUTOBOOST_HAS_TRIVIAL_COPY
#  ifdef __clang__
   AUTOBOOST_STATIC_CONSTANT(bool, value = AUTOBOOST_HAS_TRIVIAL_COPY(T) && autoboost::is_copy_constructible<T>::value);
#  else
   AUTOBOOST_STATIC_CONSTANT(bool, value = AUTOBOOST_HAS_TRIVIAL_COPY(T));
#  endif
#else
   AUTOBOOST_STATIC_CONSTANT(bool, value =
      (::autoboost::type_traits::ice_and<
         ::autoboost::is_pod<T>::value,
         ::autoboost::type_traits::ice_not< ::autoboost::is_volatile<T>::value >::value
      >::value));
#endif
};

#ifdef __clang__

template <typename T, std::size_t N>
struct has_trivial_copy_impl<T[N]>
{
   static const bool value = has_trivial_copy_impl<T>::value;
};

#endif

} // namespace detail

AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(has_trivial_copy,T,::autoboost::detail::has_trivial_copy_impl<T>::value)
AUTOBOOST_TT_AUX_BOOL_TRAIT_DEF1(has_trivial_copy_constructor,T,::autoboost::detail::has_trivial_copy_impl<T>::value)

AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_copy,void,false)
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_copy,void const,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_copy,void const volatile,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_copy,void volatile,false)
#endif

AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_copy_constructor,void,false)
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_copy_constructor,void const,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_copy_constructor,void const volatile,false)
AUTOBOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_copy_constructor,void volatile,false)
#endif

} // namespace autoboost

#include <autoboost/type_traits/detail/bool_trait_undef.hpp>

#endif // AUTOBOOST_TT_HAS_TRIVIAL_COPY_HPP_INCLUDED
