
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  (C) Copyright Eric Friedman 2002-2003.
//  (C) Copyright Antony Polukhin 2013.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_TRIVIAL_MOVE_CONSTRUCTOR_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_TRIVIAL_MOVE_CONSTRUCTOR_HPP_INCLUDED

#include <cstddef> // size_t
#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/integral_constant.hpp>

#ifdef AUTOBOOST_HAS_TRIVIAL_MOVE_CONSTRUCTOR

#if defined(AUTOBOOST_MSVC) || defined(AUTOBOOST_INTEL)
#include <autoboost/type_traits/is_pod.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#endif

#if defined(__GNUC__) || defined(__clang)
#include <autoboost/type_traits/is_constructible.hpp>
#include <autoboost/type_traits/is_volatile.hpp>
#endif


namespace autoboost {

template <typename T> struct has_trivial_move_constructor : public integral_constant<bool, AUTOBOOST_HAS_TRIVIAL_MOVE_CONSTRUCTOR(T)>{};

#else

#ifdef __SUNPRO_CC
#include <autoboost/type_traits/is_constructible.hpp>
#include <autoboost/type_traits/remove_const.hpp>
#if __cplusplus >= 201103
#define SOLARIS_EXTRA_CHECK && is_constructible<typename remove_const<T>::type, typename remove_const<T>::type&&>::value
#endif
#endif

#ifndef SOLARIS_EXTRA_CHECK
#define SOLARIS_EXTRA_CHECK
#endif

#include <autoboost/type_traits/is_pod.hpp>
#include <autoboost/type_traits/is_volatile.hpp>

namespace autoboost {

template <typename T> struct has_trivial_move_constructor
   : public integral_constant<bool, ::autoboost::is_pod<T>::value && !::autoboost::is_volatile<T>::value SOLARIS_EXTRA_CHECK>{};

#undef SOLARIS_EXTRA_CHECK

#endif

template <> struct has_trivial_move_constructor<void> : public false_type{};
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
template <> struct has_trivial_move_constructor<void const> : public false_type{};
template <> struct has_trivial_move_constructor<void volatile> : public false_type{};
template <> struct has_trivial_move_constructor<void const volatile> : public false_type{};
#endif
// What should we do with reference types??? The standard seems to suggest these are trivial, even if the thing they reference is not:
template <class T> struct has_trivial_move_constructor<T&> : public true_type{};
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
template <class T> struct has_trivial_move_constructor<T&&> : public true_type{};
#endif
// Arrays can not be explicitly copied:
template <class T, std::size_t N> struct has_trivial_move_constructor<T[N]> : public false_type{};
template <class T> struct has_trivial_move_constructor<T[]> : public false_type{};

} // namespace autoboost

#endif // AUTOBOOST_TT_HAS_TRIVIAL_MOVE_CONSTRUCTOR_HPP_INCLUDED
