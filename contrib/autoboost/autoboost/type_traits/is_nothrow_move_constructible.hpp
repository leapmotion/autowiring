
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  (C) Copyright Eric Friedman 2002-2003.
//  (C) Copyright Antony Polukhin 2013.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_HPP_INCLUDED
#define AUTOBOOST_TT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_HPP_INCLUDED

#include <cstddef> // size_t
#include <autoboost/config.hpp>
#include <autoboost/type_traits/intrinsics.hpp>
#include <autoboost/type_traits/integral_constant.hpp>
#include <autoboost/detail/workaround.hpp>

#ifdef AUTOBOOST_IS_NOTHROW_MOVE_CONSTRUCT

namespace autoboost {

template <class T>
struct is_nothrow_move_constructible : public integral_constant<bool, AUTOBOOST_IS_NOTHROW_MOVE_CONSTRUCT(T)>{};

template <class T> struct is_nothrow_move_constructible<volatile T> : public ::autoboost::false_type {};
template <class T> struct is_nothrow_move_constructible<const volatile T> : public ::autoboost::false_type{};

#elif !defined(AUTOBOOST_NO_CXX11_NOEXCEPT) && !defined(AUTOBOOST_NO_SFINAE_EXPR) && !AUTOBOOST_WORKAROUND(AUTOBOOST_GCC_VERSION, < 40800)

#include <autoboost/type_traits/declval.hpp>
#include <autoboost/utility/enable_if.hpp>

namespace autoboost{ namespace detail{

template <class T, class Enable = void>
struct false_or_cpp11_noexcept_move_constructible: public ::autoboost::false_type {};

template <class T>
struct false_or_cpp11_noexcept_move_constructible <
        T,
        typename ::autoboost::enable_if_c<sizeof(T) && AUTOBOOST_NOEXCEPT_EXPR(T(::autoboost::declval<T>()))>::type
    > : public ::autoboost::integral_constant<bool, AUTOBOOST_NOEXCEPT_EXPR(T(::autoboost::declval<T>()))>
{};

}

template <class T> struct is_nothrow_move_constructible
   : public integral_constant<bool, ::autoboost::detail::false_or_cpp11_noexcept_move_constructible<T>::value>{};

template <class T> struct is_nothrow_move_constructible<volatile T> : public ::autoboost::false_type {};
template <class T> struct is_nothrow_move_constructible<const volatile T> : public ::autoboost::false_type{};
template <class T, std::size_t N> struct is_nothrow_move_constructible<T[N]> : public ::autoboost::false_type{};
template <class T> struct is_nothrow_move_constructible<T[]> : public ::autoboost::false_type{};

#else

#include <autoboost/type_traits/has_trivial_move_constructor.hpp>
#include <autoboost/type_traits/has_nothrow_copy.hpp>
#include <autoboost/type_traits/is_array.hpp>

namespace autoboost{

template <class T>
struct is_nothrow_move_constructible
   : public integral_constant<bool,
   (::autoboost::has_trivial_move_constructor<T>::value || ::autoboost::has_nothrow_copy<T>::value) && !::autoboost::is_array<T>::value>
{};

#endif

template <> struct is_nothrow_move_constructible<void> : false_type{};
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
template <> struct is_nothrow_move_constructible<void const> : false_type{};
template <> struct is_nothrow_move_constructible<void volatile> : false_type{};
template <> struct is_nothrow_move_constructible<void const volatile> : false_type{};
#endif
// References are always trivially constructible, even if the thing they reference is not:
template <class T> struct is_nothrow_move_constructible<T&> : public ::autoboost::true_type{};
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
template <class T> struct is_nothrow_move_constructible<T&&> : public ::autoboost::true_type{};
#endif

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_NOTHROW_MOVE_CONSTRUCTIBLE_HPP_INCLUDED
