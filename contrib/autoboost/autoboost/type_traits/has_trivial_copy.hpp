
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_HAS_TRIVIAL_COPY_HPP_INCLUDED
#define AUTOBOOST_TT_HAS_TRIVIAL_COPY_HPP_INCLUDED

#include <cstddef> // size_t
#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/is_pod.hpp>
#include <autoboost/type_traits/is_reference.hpp>

#if (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 409)) || defined(AUTOBOOST_CLANG) || (defined(__SUNPRO_CC) && defined(AUTOBOOST_HAS_TRIVIAL_COPY))
#include <autoboost/type_traits/is_copy_constructible.hpp>
#define AUTOBOOST_TT_TRIVIAL_CONSTRUCT_FIX && is_copy_constructible<T>::value
#else
#define AUTOBOOST_TT_TRIVIAL_CONSTRUCT_FIX
#endif

#ifdef AUTOBOOST_INTEL
#include <autoboost/type_traits/add_const.hpp>
#include <autoboost/type_traits/add_lvalue_reference.hpp>
#endif

namespace autoboost {

template <typename T> struct has_trivial_copy
: public integral_constant<bool,
#ifdef AUTOBOOST_HAS_TRIVIAL_COPY
   AUTOBOOST_HAS_TRIVIAL_COPY(T) AUTOBOOST_TT_TRIVIAL_CONSTRUCT_FIX
#else
   ::autoboost::is_pod<T>::value
#endif
>{};
// Arrays are not explicitly copyable:
template <typename T, std::size_t N> struct has_trivial_copy<T[N]> : public false_type{};
template <typename T> struct has_trivial_copy<T[]> : public false_type{};
// Are volatile types ever trivial?  We don't really know, so assume not:
template <typename T> struct has_trivial_copy<T volatile> : public false_type{};

template <> struct has_trivial_copy<void> : public false_type{};
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
template <> struct has_trivial_copy<void const> : public false_type{};
template <> struct has_trivial_copy<void volatile> : public false_type{};
template <> struct has_trivial_copy<void const volatile> : public false_type{};
#endif

template <class T> struct has_trivial_copy<T&> : public false_type{};
#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
template <class T> struct has_trivial_copy<T&&> : public false_type{};
#endif

template <class T> struct has_trivial_copy_constructor : public has_trivial_copy<T>{};

#undef AUTOBOOST_TT_TRIVIAL_CONSTRUCT_FIX

} // namespace autoboost

#endif // AUTOBOOST_TT_HAS_TRIVIAL_COPY_HPP_INCLUDED
