
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  (C) Copyright Eric Friedman 2002-2003.
//  (C) Copyright Antony Polukhin 2013.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef AUTOBOOST_TT_IS_NOTHROW_MOVE_ASSIGNABLE_HPP_INCLUDED
#define AUTOBOOST_TT_IS_NOTHROW_MOVE_ASSIGNABLE_HPP_INCLUDED

#include <autoboost/config.hpp>
#include <autoboost/type_traits/has_trivial_move_assign.hpp>
#include <autoboost/type_traits/has_nothrow_assign.hpp>
#include <autoboost/type_traits/is_array.hpp>
#include <autoboost/type_traits/is_reference.hpp>
#include <autoboost/utility/enable_if.hpp>
#include <autoboost/type_traits/declval.hpp>

namespace autoboost {

#ifdef AUTOBOOST_IS_NOTHROW_MOVE_ASSIGN

template <class T>
struct is_nothrow_move_assignable : public integral_constant<bool, AUTOBOOST_IS_NOTHROW_MOVE_ASSIGN(T)>{};
template <class T> struct is_nothrow_move_assignable<T const> : public false_type{};
template <class T> struct is_nothrow_move_assignable<T volatile> : public false_type{};
template <class T> struct is_nothrow_move_assignable<T const volatile> : public false_type{};
template <class T> struct is_nothrow_move_assignable<T&> : public false_type{};
#if !defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
template <class T> struct is_nothrow_move_assignable<T&&> : public false_type{};
#endif

#elif !defined(AUTOBOOST_NO_CXX11_NOEXCEPT) && !defined(AUTOBOOST_NO_SFINAE_EXPR) && !AUTOBOOST_WORKAROUND(AUTOBOOST_GCC_VERSION, < 40700)

namespace detail{

template <class T, class Enable = void>
struct false_or_cpp11_noexcept_move_assignable: public ::autoboost::false_type {};

template <class T>
struct false_or_cpp11_noexcept_move_assignable <
        T,
        typename ::autoboost::enable_if_c<sizeof(T) && AUTOBOOST_NOEXCEPT_EXPR(::autoboost::declval<T&>() = ::autoboost::declval<T>())>::type
    > : public ::autoboost::integral_constant<bool, AUTOBOOST_NOEXCEPT_EXPR(::autoboost::declval<T&>() = ::autoboost::declval<T>())>
{};

}

template <class T>
struct is_nothrow_move_assignable : public integral_constant<bool, ::autoboost::detail::false_or_cpp11_noexcept_move_assignable<T>::value>{};

template <class T> struct is_nothrow_move_assignable<T const> : public ::autoboost::false_type {};
template <class T> struct is_nothrow_move_assignable<T const volatile> : public ::autoboost::false_type{};
template <class T> struct is_nothrow_move_assignable<T volatile> : public ::autoboost::false_type{};
template <class T> struct is_nothrow_move_assignable<T&> : public ::autoboost::false_type{};
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
template <class T> struct is_nothrow_move_assignable<T&&> : public ::autoboost::false_type{};
#endif

#else

template <class T>
struct is_nothrow_move_assignable : public integral_constant<bool,
   (::autoboost::has_trivial_move_assign<T>::value || ::autoboost::has_nothrow_assign<T>::value) &&  ! ::autoboost::is_array<T>::value>{};

#endif


template <> struct is_nothrow_move_assignable<void> : public false_type{};
#ifndef AUTOBOOST_NO_CV_VOID_SPECIALIZATIONS
template <> struct is_nothrow_move_assignable<void const> : public false_type{};
template <> struct is_nothrow_move_assignable<void const volatile> : public false_type{};
template <> struct is_nothrow_move_assignable<void volatile> : public false_type{};
#endif

} // namespace autoboost

#endif // AUTOBOOST_TT_IS_NOTHROW_MOVE_ASSIGNABLE_HPP_INCLUDED
