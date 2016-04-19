
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  (C) Copyright Eric Friedman 2002-2003.
//  (C) Copyright Antony Polukhin 2013.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_TRIVIAL_MOVE_ASSIGN_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_TRIVIAL_MOVE_ASSIGN_HPP_INCLUDED

#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/integral_constant.hpp>

#if !defined(AUTOBOOST_HAS_TRIVIAL_MOVE_ASSIGN) || defined(AUTOBOOST_MSVC) || defined(AUTOBOOST_INTEL)
#include <autoboost/type_traits/is_pod.hpp>
#include <autoboost/type_traits/is_const.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#ifdef AUTOBOOST_MSVC
#include <autoboost/type_traits/is_reference.hpp>
#endif
#endif

#if defined(__GNUC__) || defined(__clang)
#include <autoboost/type_traits/is_assignable.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#endif

#ifdef __SUNPRO_CC
#include <autoboost/type_traits/is_assignable.hpp>
#include <autoboost/type_traits/remove_const.hpp>
#if __cplusplus >= 201103
#define SOLARIS_EXTRA_CHECK && is_assignable<typename remove_const<T>::type&, typename remove_const<T>::type&&>::value
#endif
#endif

#ifndef SOLARIS_EXTRA_CHECK
#define SOLARIS_EXTRA_CHECK
#endif

namespace autoboost{

template <typename T>
struct has_trivial_move_assign : public integral_constant<bool,
#ifdef AUTOBOOST_HAS_TRIVIAL_MOVE_ASSIGN
   AUTOBOOST_HAS_TRIVIAL_MOVE_ASSIGN(T)
#else
   ::autoboost::is_pod<T>::value && !::autoboost::is_const<T>::value && !::autoboost::is_volatile<T>::value SOLARIS_EXTRA_CHECK
#endif
   > {};

template <> struct has_trivial_move_assign<void> : public false_type{};
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
template <> struct has_trivial_move_assign<void const> : public false_type{};
template <> struct has_trivial_move_assign<void const volatile> : public false_type{};
template <> struct has_trivial_move_assign<void volatile> : public false_type{};
#endif
template <class T> struct has_trivial_move_assign<T&> : public false_type{};
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
template <class T> struct has_trivial_move_assign<T&&> : public false_type{};
#endif
// Array types are not assignable:
template <class T, std::size_t N> struct has_trivial_move_assign<T[N]> : public false_type{};
template <class T> struct has_trivial_move_assign<T[]> : public false_type{};

} // namespace autoboost

#undef SOLARIS_EXTRA_CHECK

#endif // AUTOBOOST_TT_HAS_TRIVIAL_MOVE_ASSIGN_HPP_INCLUDED
