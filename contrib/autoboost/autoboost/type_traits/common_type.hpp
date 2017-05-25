#ifndef AUTOBOOST_TYPE_TRAITS_COMMON_TYPE_HPP_INCLUDED
#define AUTOBOOST_TYPE_TRAITS_COMMON_TYPE_HPP_INCLUDED

//
//  Copyright 2015 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//

#include <autoboost/config.hpp>
#include <autoboost/type_traits/decay.hpp>
#include <autoboost/type_traits/declval.hpp>
#include <autoboost/detail/workaround.hpp>

#if defined(AUTOBOOST_NO_CXX11_DECLTYPE)
#include <autoboost/type_traits/detail/common_type_impl.hpp>
#endif

#if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES) && !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <autoboost/type_traits/detail/mp_defer.hpp>
#endif

namespace autoboost
{

// variadic common_type

#if !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

template<class... T> struct common_type
{
};

#if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES)

template<class... T> using common_type_t = typename common_type<T...>::type;

namespace type_traits_detail
{

template<class T1, class T2, class... T> using common_type_fold = common_type_t<common_type_t<T1, T2>, T...>;

} // namespace type_traits_detail

template<class T1, class T2, class... T>
struct common_type<T1, T2, T...>: type_traits_detail::mp_defer<type_traits_detail::common_type_fold, T1, T2, T...>
{
};

#else

template<class T1, class T2, class... T>
struct common_type<T1, T2, T...>: common_type<typename common_type<T1, T2>::type, T...>
{
};

#endif // !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES)

#else

template<
    class T1 = void, class T2 = void, class T3 = void,
    class T4 = void, class T5 = void, class T6 = void,
    class T7 = void, class T8 = void, class T9 = void
>
struct common_type: common_type<typename common_type<T1, T2>::type, T3, T4, T5, T6, T7, T8, T9>
{
};

#endif // !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

// one argument

template<class T> struct common_type<T>: autoboost::decay<T>
{
};

// two arguments

namespace type_traits_detail
{

// binary common_type

#if !defined(AUTOBOOST_NO_CXX11_DECLTYPE)

#if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES) && !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#if !defined(AUTOBOOST_MSVC) || AUTOBOOST_MSVC > 1800

// internal compiler error on msvc-12.0

template<class T1, class T2> using builtin_common_type = typename autoboost::decay<decltype( autoboost::declval<bool>()? autoboost::declval<T1>(): autoboost::declval<T2>() )>::type;

template<class T1, class T2> struct common_type_impl: mp_defer<builtin_common_type, T1, T2>
{
};

#else

template<class T1, class T2> using builtin_common_type = decltype( autoboost::declval<bool>()? autoboost::declval<T1>(): autoboost::declval<T2>() );

template<class T1, class T2> struct common_type_impl_2: mp_defer<builtin_common_type, T1, T2>
{
};

template<class T1, class T2> using decay_common_type = typename autoboost::decay<typename common_type_impl_2<T1, T2>::type>::type;

template<class T1, class T2> struct common_type_impl: mp_defer<decay_common_type, T1, T2>
{
};

#endif // !defined(AUTOBOOST_MSVC) || AUTOBOOST_MSVC > 1800

#else

template<class T1, class T2> struct common_type_impl: autoboost::decay<decltype( autoboost::declval<bool>()? autoboost::declval<T1>(): autoboost::declval<T2>() )>
{
};

#endif // #if !defined(AUTOBOOST_NO_CXX11_TEMPLATE_ALIASES) && !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

#endif // #if !defined(AUTOBOOST_NO_CXX11_DECLTYPE)

// decay helper

template<class T1, class T2, class T1d = typename autoboost::decay<T1>::type, class T2d = typename autoboost::decay<T2>::type> struct common_type_decay_helper: autoboost::common_type<T1d, T2d>
{
};

template<class T1, class T2> struct common_type_decay_helper<T1, T2, T1, T2>: common_type_impl<T1, T2>
{
};

} // type_traits_detail

template<class T1, class T2> struct common_type<T1, T2>: type_traits_detail::common_type_decay_helper<T1, T2>
{
};

} // namespace autoboost

#endif // #ifndef AUTOBOOST_TYPE_TRAITS_COMMON_TYPE_HPP_INCLUDED
