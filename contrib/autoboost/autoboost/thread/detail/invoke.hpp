// Copyright (C) 2012-2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// 2013/04 Vicente J. Botet Escriba
//    Provide implementation up to 9 parameters when AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES is defined.
//    Make use of Boost.Move
//    Make use of Boost.Tuple (movable)
// 2012 Vicente J. Botet Escriba
//    Provide implementation _RET using bind when AUTOBOOST_NO_CXX11_HDR_FUNCTIONAL and AUTOBOOST_NO_SFINAE_EXPR are not defined
// 2012 Vicente J. Botet Escriba
//    Adapt to autoboost libc++ implementation

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
// The invoke code is based on the one from libcxx.
//===----------------------------------------------------------------------===//

#ifndef AUTOBOOST_THREAD_DETAIL_INVOKE_HPP
#define AUTOBOOST_THREAD_DETAIL_INVOKE_HPP

#include <autoboost/config.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/core/enable_if.hpp>
#include <autoboost/mpl/bool.hpp>
#include <autoboost/type_traits/is_base_of.hpp>
#include <autoboost/type_traits/is_pointer.hpp>
#include <autoboost/type_traits/is_member_function_pointer.hpp>
#include <autoboost/type_traits/remove_reference.hpp>
#ifndef AUTOBOOST_NO_CXX11_HDR_FUNCTIONAL
#include <functional>
#endif

namespace autoboost
{
  namespace detail
  {


#if ! defined(AUTOBOOST_NO_SFINAE_EXPR) && \
    ! defined(AUTOBOOST_NO_CXX11_DECLTYPE) && \
    ! defined(AUTOBOOST_NO_CXX11_DECLTYPE_N3276) && \
    ! defined(AUTOBOOST_NO_CXX11_TRAILING_RESULT_TYPES)

#define AUTOBOOST_THREAD_PROVIDES_INVOKE

#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    // bullets 1 and 2

    template <class Fp, class A0, class ...Args>
    inline auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
        -> decltype((autoboost::forward<A0>(a0).*f)(autoboost::forward<Args>(args)...))
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<Args>(args)...);
    }
    template <class R, class Fp, class A0, class ...Args>
    inline auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
        -> decltype((autoboost::forward<A0>(a0).*f)(autoboost::forward<Args>(args)...))
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<Args>(args)...);
    }

    template <class Fp, class A0, class ...Args>
    inline auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
        -> decltype(((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<Args>(args)...))
    {
        return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<Args>(args)...);
    }
    template <class R, class Fp, class A0, class ...Args>
    inline auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
        -> decltype(((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<Args>(args)...))
    {
        return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<Args>(args)...);
    }

    // bullets 3 and 4

    template <class Fp, class A0>
    inline auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype(autoboost::forward<A0>(a0).*f)
    {
        return autoboost::forward<A0>(a0).*f;
    }

    template <class Fp, class A0>
    inline auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype((*autoboost::forward<A0>(a0)).*f)
    {
        return (*autoboost::forward<A0>(a0)).*f;
    }

    template <class R, class Fp, class A0>
    inline auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype(autoboost::forward<A0>(a0).*f)
    {
        return autoboost::forward<A0>(a0).*f;
    }

    template <class R, class Fp, class A0>
    inline auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype((*autoboost::forward<A0>(a0)).*f)
    {
        return (*autoboost::forward<A0>(a0)).*f;
    }


    // bullet 5

    template <class R, class Fp, class ...Args>
    inline auto invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    -> decltype(autoboost::forward<Fp>(f)(autoboost::forward<Args>(args)...))
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<Args>(args)...);
    }
    template <class Fp, class ...Args>
    inline auto invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    -> decltype(autoboost::forward<Fp>(f)(autoboost::forward<Args>(args)...))
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<Args>(args)...);
    }

#else // AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES

    // bullets 1 and 2

    template <class Fp, class A0>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype((autoboost::forward<A0>(a0).*f)())
    {
        return (autoboost::forward<A0>(a0).*f)();
    }
    template <class R, class Fp, class A0>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype((autoboost::forward<A0>(a0).*f)())
    {
        return (autoboost::forward<A0>(a0).*f)();
    }
    template <class Fp, class A0, class A1>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
        -> decltype((autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1)))
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1));
    }
    template <class R, class Fp, class A0, class A1>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
        -> decltype((autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1)))
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1));
    }
    template <class Fp, class A0, class A1, class A2>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
        -> decltype((autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2)))
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class R, class Fp, class A0, class A1, class A2>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
        -> decltype((autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2)))
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }

    template <class Fp, class A0>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype(((*autoboost::forward<A0>(a0)).*f)())
    {
        return ((*autoboost::forward<A0>(a0)).*f)();
    }
    template <class R, class Fp, class A0>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype(((*autoboost::forward<A0>(a0)).*f)())
    {
        return ((*autoboost::forward<A0>(a0)).*f)();
    }
    template <class Fp, class A0, class A1>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
        -> decltype(((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1)))
    {
        return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1));
    }
    template <class R, class Fp, class A0, class A1>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
        -> decltype(((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1)))
    {
        return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1));
    }
    template <class Fp, class A0, class A1, class A2>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
        -> decltype(((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2)))
    {
        return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class R, class Fp, class A0, class A1, class A2>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
        -> decltype(((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2)))
    {
        return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }

    // bullets 3 and 4

    template <class Fp, class A0>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype(autoboost::forward<A0>(a0).*f)
    {
        return autoboost::forward<A0>(a0).*f;
    }
    template <class R, class Fp, class A0>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype(autoboost::forward<A0>(a0).*f)
    {
        return autoboost::forward<A0>(a0).*f;
    }

    template <class Fp, class A0>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype((*autoboost::forward<A0>(a0)).*f)
    {
        return (*autoboost::forward<A0>(a0)).*f;
    }
    template <class R, class Fp, class A0>
    inline
    auto
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A0) a0)
        -> decltype((*autoboost::forward<A0>(a0)).*f)
    {
        return (*autoboost::forward<A0>(a0)).*f;
    }

    // bullet 5

    template <class Fp>
    inline
    auto invoke(AUTOBOOST_THREAD_RV_REF(Fp) f)
    -> decltype(autoboost::forward<Fp>(f)())
    {
      return autoboost::forward<Fp>(f)();
    }
    template <class Fp, class A1>
    inline
    auto invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1)
    -> decltype(autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1)))
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1));
    }    template <class Fp, class A1, class A2>
    inline
    auto invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    -> decltype(autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2)))
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Fp, class A1, class A2, class A3>
    inline
    auto invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    -> decltype(autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3)))
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }


    template <class R, class Fp>
    inline
    auto invoke(AUTOBOOST_THREAD_RV_REF(Fp) f)
    -> decltype(autoboost::forward<Fp>(f)())
    {
      return autoboost::forward<Fp>(f)();
    }
    template <class R, class Fp, class A1>
    inline
    auto invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1)
    -> decltype(autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1)))
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1));
    }
    template <class R, class Fp, class A1, class A2>
    inline
    auto invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    -> decltype(autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2)))
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class R, class Fp, class A1, class A2, class A3>
    inline
    auto invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    -> decltype(autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3)))
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }

#endif // AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES

#elif ! defined(AUTOBOOST_NO_SFINAE_EXPR) && \
    ! defined AUTOBOOST_NO_CXX11_HDR_FUNCTIONAL && \
    defined  AUTOBOOST_MSVC

    template <class Ret, class Fp>
    inline
    Ret invoke(AUTOBOOST_THREAD_RV_REF(Fp) f)
    {
      return f();
    }
    template <class Ret, class Fp, class A1>
    inline
    Ret invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
      return std::bind(autoboost::forward<Fp>(f), autoboost::forward<A1>(a1))();
    }
    template <class Ret, class Fp, class A1, class A2>
    inline
    Ret invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    {
      return std::bind(autoboost::forward<Fp>(f), autoboost::forward<A1>(a1), autoboost::forward<A2>(a2))();
    }
    template <class Ret, class Fp, class A1, class A2, class A3>
    inline
    Ret invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    {
      return std::bind(autoboost::forward<Fp>(f), autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3))();
    }

#define AUTOBOOST_THREAD_PROVIDES_INVOKE_RET

#elif ! defined  AUTOBOOST_MSVC
//!!!!!  WARNING !!!!! THIS DOESN'T WORKS YET
#define AUTOBOOST_THREAD_PROVIDES_INVOKE_RET

#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

    // bullet 1
    // (t1.*f)(t2, ..., tN) when f is a pointer to a member function of a class T and t1 is an object of
    // type T or a reference to an object of type T or a reference to an object of a type derived from T
    template <class Ret, class A, class A0, class ...Args>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(Args...), AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<Args>(args)...);
    }

    template <class Ret, class A, class A0, class ...Args>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(Args...) const, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<Args>(args)...);
    }

    template <class Ret, class A, class A0, class ...Args>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(Args...) volatile, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<Args>(args)...);
    }

    template <class Ret, class A, class A0, class ...Args>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(Args...) const volatile, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<Args>(args)...);
    }

    // bullet 2
    // ((*t1).*f)(t2, ..., tN) when f is a pointer to a member function of a class T and t1 is not one of
    // the types described in the previous item;
    template <class Ret, class A, class A0, class ...Args>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(Args...), AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<Args>(args)...);
    }

    template <class Ret, class A, class A0, class ...Args>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(Args...) const, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<Args>(args)...);
    }

    template <class Ret, class A, class A0, class ...Args>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(Args...) volatile, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<Args>(args)...);
    }

    template <class Ret, class A, class A0, class ...Args>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(Args...) const volatile, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<Args>(args)...);
    }

    // bullet 3
    // t1.*f when N == 1 and f is a pointer to member data of a class T and t1 is an object of type T or a
    // reference to an object of type T or a reference to an object of a type derived from T;
//    template <class Ret, class A, class A0>
//    inline
//    typename enable_if_c
//    <
//        is_base_of<A, typename remove_reference<A0>::type>::value,
//        typename detail::apply_cv<A0, A>::type&
//    >::type
//    invoke(Ret A::* f, AUTOBOOST_THREAD_RV_REF(A0) a0)
//    {
//        return autoboost::forward<A0>(a0).*f;
//    }

    // bullet 4
    // (*t1).*f when N == 1 and f is a pointer to member data of a class T and t1 is not one of the types
    //described in the previous item;

//    template <class A0, class Ret, bool>
//    struct d4th_helper
//    {
//    };
//
//    template <class A0, class Ret>
//    struct d4th_helper<A0, Ret, true>
//    {
//        typedef typename apply_cv<decltype(*declval<A0>()), Ret>::type type;
//    };
//
//    template <class Ret, class A, class A0>
//    inline
//    typename detail::4th_helper<A, Ret,
//                          !is_base_of<A,
//                                      typename remove_reference<A0>::type
//                                     >::value
//                         >::type&
//    invoke(Ret A::* f, AUTOBOOST_THREAD_RV_REF(A0) a0)
//    {
//        return (*autoboost::forward<A0>(a0)).*f;
//    }

//    template <class Ret, class A, class A0>
//    inline
//    typename enable_if_c
//    <
//        !is_base_of<A, typename remove_reference<A0>::type>::value,
//        typename detail::ref_return1<Ret A::*, A0>::type
//    >::type
//    invoke(Ret A::* f, AUTOBOOST_THREAD_RV_REF(A0) a0)
//    {
//        return (*autoboost::forward<A0>(a0)).*f;
//    }

    // bullet 5
    // f(t1, t2, ..., tN) in all other cases.

    template <class Ret, class Fp, class ...Args>
    inline Ret do_invoke(mpl::false_, AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<Args>(args)...);
    }

    template <class Ret, class Fp, class ...Args>
    inline Ret do_invoke(mpl::true_, AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
      return f(autoboost::forward<Args>(args)...);
    }

    template <class Ret, class Fp, class ...Args>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(AUTOBOOST_THREAD_RV_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(Args) ...args)
    {
      return autoboost::detail::do_invoke<Ret>(autoboost::is_pointer<Fp>(), autoboost::forward<Fp>(f), autoboost::forward<Args>(args)...);
    }
#else // AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
    // bullet 1
    // (t1.*f)(t2, ..., tN) when f is a pointer to a member function of a class T and t1 is an object of
    // type T or a reference to an object of type T or a reference to an object of a type derived from T

    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(), A0& a0)
    {
        return (a0.*f)();
    }
    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(), A0* a0)
    {
        return ((*a0).*f)();
    }

    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1),
        A0& a0, AUTOBOOST_THREAD_RV_REF(A1) a1
        )
    {
        return (a0.*f)(autoboost::forward<A1>(a1));
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1), A0& a0, A1 a1)
    {
        return (a0.*f)(a1);
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1), A0* a0, AUTOBOOST_THREAD_RV_REF(A1) a1
        )
    {
        return (*(a0).*f)(autoboost::forward<A1>(a1));
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1), A0* a0, A1 a1)
    {
        return (*a0.*f)(a1);
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2),
        A0& a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2
        )
    {
        return (a0.*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2), A0* a0, A1 a1, A2 a2)
    {
        return ((*a0).*f)(a1, a2);
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3),
        A0& a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    {
        return (a0.*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3), A0* a0, A1 a1, A2 a2, A3 a3)
    {
        return ((*a0).*f)(a1, a2, a3);
    }

///
    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)() const, A0 const& a0)
    {
        return (a0.*f)();
    }
    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)() const, A0 const* a0)
    {
        return ((*a0).*f)();
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) const, A0 const& a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
        return (a0.*f)(autoboost::forward<A1>(a1));
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) const, A0 const* a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
        return ((*a0).*f)(autoboost::forward<A1>(a1));
    }

    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) const, A0 const& a0, A1 a1)
    {
        return (a0.*f)(a1);
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) const,
        A0 const& a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2
        )
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2)
            );
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) const, A0 const& a0, A1 a1, A2 a2)
    {
        return (a0.*f)(a1, a2);
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) const,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3
        )
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) const, A0 a0, A1 a1, A2 a2, A3 a3)
    {
        return (a0.*f)(a1, a2, a3);
    }
    ///
    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)() volatile, AUTOBOOST_THREAD_RV_REF(A0) a0)
    {
        return (autoboost::forward<A0>(a0).*f)();
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) volatile, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1));
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) volatile, A0 a0, A1 a1)
    {
        return (a0.*f)(a1);
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) volatile,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) volatile, A0 a0, A1 a1, A2 a2 )
    {
        return (a0.*f)(a1, a2);
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) volatile,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3
        )
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) volatile, A0 a0, A1 a1, A2 a2, A3 a3)
    {
        return (a0.*f)(a1, a2, a3);
    }
    ///
    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)() const volatile, AUTOBOOST_THREAD_RV_REF(A0) a0)
    {
        return (autoboost::forward<A0>(a0).*f)();
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) const volatile, AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1));
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) const volatile, A0 a0, A1 a1)
    {
        return (a0.*f)(a1);
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) const volatile,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2
        )
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) const volatile,
        A0 a0, A1 a1, A2 a2
        )
    {
        return (a0.*f)(a1, a2);
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) const volatile,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3
        )
    {
        return (autoboost::forward<A0>(a0).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) const volatile,
        A0 a0, A1 a1, A2 a2, A3 a3
        )
    {
        return (a0.*f)(a1, a2, a3);
    }

    // bullet 2
    // ((*t1).*f)(t2, ..., tN) when f is a pointer to a member function of a class T and t1 is not one of
    // the types described in the previous item;
    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(), AUTOBOOST_THREAD_RV_REF(A0) a0)
    {
      return ((*autoboost::forward<A0>(a0)).*f)();
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1), AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1));
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1), A0 a0, A1 a1)
    {
      return ((*a0).*f)(a1);
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, AUTOBOOST_THREAD_RV_REF(A2)),
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2), A0 a0, A1 a1, A2 a2)
    {
      return ((*a0).*f)(a1, a2);
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, AUTOBOOST_THREAD_RV_REF(A2), AUTOBOOST_THREAD_RV_REF(A3)),
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3)
          );
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3), A0 a0, A1 a1, A2 a2, A3 a3)
    {
      return ((*a0).*f)(a1, a2, a3);
    }

///
    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)() const, AUTOBOOST_THREAD_RV_REF(A0) a0)
    {
      return ((*autoboost::forward<A0>(a0)).*f)();
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) const,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1));
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) const, AUTOBOOST_THREAD_RV_REF(A0) a0, A1 a1)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(a1);
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) const, A0 a0, A1 a1)
    {
      return ((*a0).*f)(a1);
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) const,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) const, A0 a0, A1 a1, A2 a2)
    {
      return ((*a0).*f)(a1, a2);
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) const,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) const,
        A0 a0, A1 a1, A2 a2, A3 a3)
    {
      return ((*a0).*f)(a1, a2, a3);
    }
    ///
    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)() volatile, AUTOBOOST_THREAD_RV_REF(A0) a0)
    {
      return ((*autoboost::forward<A0>(a0)).*f)();
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) volatile,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1));
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) volatile, A0 a0, A1 a1)
    {
      return ((*a0).*f)(a1);
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) volatile,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) volatile, A0 a0, A1 a1, A2 a2)
    {
      return ((*a0).*f)(a1, a2);
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) volatile,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) volatile, A0 a0, A1 a1, A2 a2, A3 a3)
    {
      return ((*a0).*f)(a1, a2, a3);
    }
    ///
    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)() const volatile, AUTOBOOST_THREAD_RV_REF(A0) a0)
    {
      return ((*autoboost::forward<A0>(a0)).*f)();
    }
    template <class Ret, class A, class A0>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)() const volatile, A0 a0)
    {
      return ((*a0).*f)();
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) const volatile,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1));
    }
    template <class Ret, class A, class A0, class A1>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1) const volatile, A0 a0, A1 a1)
    {
      return ((*a0).*f)(a1);
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) const volatile,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Ret, class A, class A0, class A1, class A2>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2) const volatile,
        A0 a0, A1 a1, A2 a2)
    {
      return ((*a0).*f)(a1, a2);
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) const volatile,
        AUTOBOOST_THREAD_RV_REF(A0) a0, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    {
      return ((*autoboost::forward<A0>(a0)).*f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }
    template <class Ret, class A, class A0, class A1, class A2, class A3>
    inline
    typename enable_if_c
    <
        ! is_base_of<A, typename remove_reference<A0>::type>::value,
        Ret
    >::type
    invoke(Ret (A::*f)(A1, A2, A3) const volatile,
        A0 a0, A1 a1, A2 a2, A3 a3)
    {
      return ((*a0).*f)(a1, a2, a3);
    }
    // bullet 3
    // t1.*f when N == 1 and f is a pointer to member data of a class T and t1 is an object of type T or a
    // reference to an object of type T or a reference to an object of a type derived from T;
//    template <class Ret, class A, class A0>
//    inline
//    typename enable_if_c
//    <
//        is_base_of<A, typename remove_reference<A0>::type>::value,
//        typename detail::apply_cv<A0, A>::type&
//    >::type
//    invoke(Ret A::* f, AUTOBOOST_THREAD_RV_REF(A0) a0)
//    {
//        return autoboost::forward<A0>(a0).*f;
//    }

    // bullet 4
    // (*t1).*f when N == 1 and f is a pointer to member data of a class T and t1 is not one of the types
    //described in the previous item;

//    template <class A0, class Ret, bool>
//    struct d4th_helper
//    {
//    };
//
//    template <class A0, class Ret>
//    struct d4th_helper<A0, Ret, true>
//    {
//        typedef typename apply_cv<decltype(*declval<A0>()), Ret>::type type;
//    };
//
//    template <class Ret, class A, class A0>
//    inline
//    typename detail::4th_helper<A, Ret,
//                          !is_base_of<A,
//                                      typename remove_reference<A0>::type
//                                     >::value
//                         >::type&
//    invoke(Ret A::* f, AUTOBOOST_THREAD_RV_REF(A0) a0)
//    {
//        return (*autoboost::forward<A0>(a0)).*f;
//    }

//    template <class Ret, class A, class A0>
//    inline
//    typename enable_if_c
//    <
//        !is_base_of<A, typename remove_reference<A0>::type>::value,
//        typename detail::ref_return1<Ret A::*, A0>::type
//    >::type
//    invoke(Ret A::* f, AUTOBOOST_THREAD_RV_REF(A0) a0)
//    {
//        return (*autoboost::forward<A0>(a0)).*f;
//    }

    // bullet 5
    // f(t1, t2, ..., tN) in all other cases.

    template <class Ret, class Fp>
    inline Ret do_invoke(mpl::false_, AUTOBOOST_THREAD_FWD_REF(Fp) f)
    {
      return autoboost::forward<Fp>(f)();
    }
    template <class Ret, class Fp>
    inline Ret do_invoke(mpl::true_, AUTOBOOST_THREAD_FWD_REF(Fp) f)
    {
      return f();
    }
    template <class Ret, class Fp>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(AUTOBOOST_THREAD_FWD_REF(Fp) f)
    {
      return autoboost::detail::do_invoke<Ret>(autoboost::is_pointer<Fp>(), autoboost::forward<Fp>(f));
    }

    template <class Ret, class Fp, class A1>
    inline Ret do_invoke(mpl::false_, AUTOBOOST_THREAD_FWD_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1));
    }
    template <class Ret, class Fp, class A1>
    inline Ret do_invoke(mpl::true_, AUTOBOOST_THREAD_FWD_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
      return f(autoboost::forward<A1>(a1));
    }
    template <class Ret, class Fp, class A1>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(AUTOBOOST_THREAD_FWD_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
      return autoboost::detail::do_invoke<Ret>(autoboost::is_pointer<Fp>(), autoboost::forward<Fp>(f), autoboost::forward<A1>(a1));
    }

    template <class Ret, class Fp, class A1, class A2>
    inline Ret do_invoke(mpl::false_, AUTOBOOST_THREAD_FWD_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Ret, class Fp, class A1, class A2>
    inline Ret do_invoke(mpl::true_, AUTOBOOST_THREAD_FWD_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    {
      return f(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Ret, class Fp, class A1, class A2>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(AUTOBOOST_THREAD_FWD_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    {
      return autoboost::detail::do_invoke<Ret>(autoboost::is_pointer<Fp>(), autoboost::forward<Fp>(f), autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }

    template <class Ret, class Fp, class A1, class A2, class A3>
    inline Ret do_invoke(mpl::false_, AUTOBOOST_THREAD_FWD_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    {
      return autoboost::forward<Fp>(f)(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }
    template <class Ret, class Fp, class A1, class A2, class A3>
    inline Ret do_invoke(mpl::true_, AUTOBOOST_THREAD_FWD_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    {
      return f(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }
    template <class Ret, class Fp, class A1, class A2, class A3>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(AUTOBOOST_THREAD_FWD_REF(Fp) f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    {
      return autoboost::detail::do_invoke<Ret>(autoboost::is_pointer<Fp>(), autoboost::forward<Fp>(f), autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }


    template <class Ret, class Fp, class A1>
    inline Ret do_invoke(mpl::false_, AUTOBOOST_THREAD_FWD_REF(Fp) f, A1 a1)
    {
      return autoboost::forward<Fp>(f)(a1);
    }
    template <class Ret, class Fp, class A1>
    inline Ret do_invoke(mpl::true_, AUTOBOOST_THREAD_FWD_REF(Fp) f, A1 a1)
    {
      return f(a1);
    }
    template <class Ret, class Fp, class A1>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(AUTOBOOST_THREAD_FWD_REF(Fp) f, A1 a1)
    {
      return autoboost::detail::do_invoke<Ret>(autoboost::is_pointer<Fp>(), autoboost::forward<Fp>(f), a1);
    }

    template <class Ret, class Fp, class A1, class A2>
    inline Ret do_invoke(mpl::false_, AUTOBOOST_THREAD_FWD_REF(Fp) f, A1 a1, A2 a2)
    {
      return autoboost::forward<Fp>(f)(a1, a2);
    }
    template <class Ret, class Fp, class A1, class A2>
    inline Ret do_invoke(mpl::true_, AUTOBOOST_THREAD_FWD_REF(Fp) f, A1 a1, A2 a2)
    {
      return f(a1, a2);
    }
    template <class Ret, class Fp, class A1, class A2>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(AUTOBOOST_THREAD_FWD_REF(Fp) f, A1 a1, A2 a2)
    {
      return autoboost::detail::do_invoke<Ret>(autoboost::is_pointer<Fp>(), autoboost::forward<Fp>(f), a1, a2);
    }

    template <class Ret, class Fp, class A1, class A2, class A3>
    inline Ret do_invoke(mpl::false_, AUTOBOOST_THREAD_FWD_REF(Fp) f, A1 a1, A2 a2, A3 a3)
    {
      return autoboost::forward<Fp>(f)(a1, a2, a3);
    }
    template <class Ret, class Fp, class A1, class A2, class A3>
    inline Ret do_invoke(mpl::true_, AUTOBOOST_THREAD_FWD_REF(Fp) f, A1 a1, A2 a2, A3 a3)
    {
      return f(a1, a2, a3);
    }
    template <class Ret, class Fp, class A1, class A2, class A3>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(AUTOBOOST_THREAD_FWD_REF(Fp) f, A1 a1, A2 a2, A3 a3)
    {
      return autoboost::detail::do_invoke<Ret>(autoboost::is_pointer<Fp>(), autoboost::forward<Fp>(f), a1, a2, a3);
    }


    ///
    template <class Ret, class Fp>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(Fp &f)
    {
      return f();
    }
    template <class Ret, class Fp, class A1>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(Fp &f, AUTOBOOST_THREAD_RV_REF(A1) a1)
    {
      return f(autoboost::forward<A1>(a1));
    }
    template <class Ret, class Fp, class A1>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(Fp &f, A1 a1)
    {
      return f(a1);
    }
    template <class Ret, class Fp, class A1, class A2>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(Fp &f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2)
    {
      return f(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2));
    }
    template <class Ret, class Fp, class A1, class A2>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(Fp &f, A1 a1, A2 a2)
    {
      return f(a1, a2);
    }
    template <class Ret, class Fp, class A1, class A2, class A3>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(Fp &f, AUTOBOOST_THREAD_RV_REF(A1) a1, AUTOBOOST_THREAD_RV_REF(A2) a2, AUTOBOOST_THREAD_RV_REF(A3) a3)
    {
      return f(autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3));
    }
    template <class Ret, class Fp, class A1, class A2, class A3>
    inline
    typename disable_if_c
    <
        is_member_function_pointer<Fp>::value,
        Ret
    >::type
    invoke(Fp &f, A1 a1, A2 a2, A3 a3)
    {
      return f(a1, a2, a3);
    }
    ///

#endif // AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES

#endif  // all
      }
    }

#endif // header
