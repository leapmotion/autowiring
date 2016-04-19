
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_NOTHROW_COPY_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_NOTHROW_COPY_HPP_INCLUDED

#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/integral_constant.hpp>

#ifdef AUTOBOOST_HAS_NOTHROW_COPY

#if defined(AUTOBOOST_CLANG) || defined(__GNUC__) || defined(__ghs__) || defined(__CODEGEARC__) || defined(__SUNPRO_CC)
#include <autoboost/type_traits/is_volatile.hpp>
#include <autoboost/type_traits/is_copy_constructible.hpp>
#include <autoboost/type_traits/is_reference.hpp>
#include <autoboost/type_traits/is_array.hpp>
#ifdef AUTOBOOST_INTEL
#include <autoboost/type_traits/is_pod.hpp>
#endif
#elif defined(AUTOBOOST_MSVC) || defined(AUTOBOOST_INTEL)
#include <autoboost/type_traits/has_trivial_copy.hpp>
#include <autoboost/type_traits/is_array.hpp>
#ifdef AUTOBOOST_INTEL
#include <autoboost/type_traits/add_lvalue_reference.hpp>
#include <autoboost/type_traits/add_const.hpp>
#endif
#endif

namespace autoboost {

template <class T> struct has_nothrow_copy_constructor : public integral_constant<bool, AUTOBOOST_HAS_NOTHROW_COPY(T)>{};

#elif !defined(AUTOBOOST_NO_CXX11_NOEXCEPT)

#include <autoboost/type_traits/declval.hpp>
#include <autoboost/type_traits/is_copy_constructible.hpp>

namespace autoboost{

namespace detail{

template <class T, bool b>
struct has_nothrow_copy_constructor_imp : public autoboost::integral_constant<bool, false>{};
template <class T>
struct has_nothrow_copy_constructor_imp<T, true> : public autoboost::integral_constant<bool, noexcept(T(autoboost::declval<const T&>()))>{};

}

template <class T> struct has_nothrow_copy_constructor : public detail::has_nothrow_copy_constructor_imp<T, autoboost::is_copy_constructible<T>::value>{};

#else

#include <autoboost/type_traits/has_trivial_copy.hpp>

namespace autoboost{

template <class T> struct has_nothrow_copy_constructor : public integral_constant<bool, ::autoboost::has_trivial_copy<T>::value>{};

#endif

template <> struct has_nothrow_copy_constructor<void> : public false_type{};
template <class T> struct has_nothrow_copy_constructor<T volatile> : public false_type{};
template <class T> struct has_nothrow_copy_constructor<T&> : public false_type{};
#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
template <class T> struct has_nothrow_copy_constructor<T&&> : public false_type{};
#endif
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
template <> struct has_nothrow_copy_constructor<void const> : public false_type{};
template <> struct has_nothrow_copy_constructor<void volatile> : public false_type{};
template <> struct has_nothrow_copy_constructor<void const volatile> : public false_type{};
#endif

template <class T> struct has_nothrow_copy : public has_nothrow_copy_constructor<T>{};

} // namespace autoboost

#endif // AUTOBOOST_TT_HAS_NOTHROW_COPY_HPP_INCLUDED
