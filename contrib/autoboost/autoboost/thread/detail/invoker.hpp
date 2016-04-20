// Copyright (C) 2012 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// 2013/04 Vicente J. Botet Escriba
//    Provide implementation up to 9 parameters when AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES is defined.
//    Make use of Boost.Move
//    Make use of Boost.Tuple (movable)
// 2012/11 Vicente J. Botet Escriba
//    Adapt to autoboost libc++ implementation

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
// The invoker code is based on the one from libcxx.
//===----------------------------------------------------------------------===//

#ifndef AUTOBOOST_THREAD_DETAIL_INVOKER_HPP
#define AUTOBOOST_THREAD_DETAIL_INVOKER_HPP

#include <autoboost/config.hpp>

#include <autoboost/utility/result_of.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/detail/invoke.hpp>
#include <autoboost/thread/detail/make_tuple_indices.hpp>
#include <autoboost/thread/csbl/tuple.hpp>
#include <autoboost/tuple/tuple.hpp>

#include <autoboost/thread/detail/variadic_header.hpp>

namespace autoboost
{
  namespace detail
  {

#if defined(AUTOBOOST_THREAD_PROVIDES_INVOKE) && ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) && ! defined(AUTOBOOST_NO_CXX11_HDR_TUPLE)

    template <class Fp, class ... Args>
    class invoker
    {
      //typedef typename decay<Fp>::type Fpd;
      //typedef tuple<typename decay<Args>::type...> Argsd;

      //csbl::tuple<Fpd, Argsd...> f_;
      csbl::tuple<Fp, Args...> f_;

    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE( invoker)
      //typedef typename invoke_of<_Fp, _Args...>::type Rp;
      typedef typename result_of<Fp(Args...)>::type result_type;

      template <class F, class ... As>
      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(As)... args)
      : f_(autoboost::forward<F>(f), autoboost::forward<As>(args)...)
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f) : f_(autoboost::move(AUTOBOOST_THREAD_RV(f).f_))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker( const invoker& f) : f_(f.f_)
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker& operator=(AUTOBOOST_THREAD_RV_REF(invoker) f)
      {
        f_ = autoboost::move(AUTOBOOST_THREAD_RV(f).f_);
      }

      AUTOBOOST_SYMBOL_VISIBLE
      invoker& operator=( AUTOBOOST_THREAD_COPY_ASSIGN_REF(invoker) f)
      {
        f_ = f.f_;
      }

      result_type operator()()
      {
        typedef typename make_tuple_indices<1+sizeof...(Args), 1>::type Index;
        return execute(Index());
      }
    private:
      template <size_t ...Indices>
      result_type
      execute(tuple_indices<Indices...>)
      {
        return invoke(autoboost::move(csbl::get<0>(f_)), autoboost::move(csbl::get<Indices>(f_))...);
      }
    };

    template <class R, class Fp, class ... Args>
    class invoker_ret
    {
      //typedef typename decay<Fp>::type Fpd;
      //typedef tuple<typename decay<Args>::type...> Argsd;

      //csbl::tuple<Fpd, Argsd...> f_;
      csbl::tuple<Fp, Args...> f_;

    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE( invoker_ret)
      typedef R result_type;

      template <class F, class ... As>
      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker_ret(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(As)... args)
      : f_(autoboost::forward<F>(f), autoboost::forward<As>(args)...)
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker_ret(AUTOBOOST_THREAD_RV_REF(invoker_ret) f) : f_(autoboost::move(AUTOBOOST_THREAD_RV(f).f_))
      {}

      result_type operator()()
      {
        typedef typename make_tuple_indices<1+sizeof...(Args), 1>::type Index;
        return execute(Index());
      }
    private:
      template <size_t ...Indices>
      result_type
      execute(tuple_indices<Indices...>)
      {
        return invoke<R>(autoboost::move(csbl::get<0>(f_)), autoboost::move(csbl::get<Indices>(f_))...);
      }
    };
  //AUTOBOOST_THREAD_DCL_MOVABLE_BEG(X) invoker<Fp> AUTOBOOST_THREAD_DCL_MOVABLE_END
#else

#if ! defined AUTOBOOST_MSVC

#define AUTOBOOST_THREAD_RV_REF_ARG_T(z, n, unused) AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_THREAD_RV_REF(Arg##n)
#define AUTOBOOST_THREAD_RV_REF_A_T(z, n, unused) AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_THREAD_RV_REF(A##n)
#define AUTOBOOST_THREAD_RV_REF_ARG(z, n, unused) , AUTOBOOST_THREAD_RV_REF(Arg##n) arg##n
#define AUTOBOOST_THREAD_FWD_REF_A(z, n, unused)   , AUTOBOOST_THREAD_FWD_REF(A##n) arg##n
#define AUTOBOOST_THREAD_FWD_REF_ARG(z, n, unused) , AUTOBOOST_THREAD_FWD_REF(Arg##n) arg##n
#define AUTOBOOST_THREAD_FWD_PARAM(z, n, unused) , autoboost::forward<Arg##n>(arg##n)
#define AUTOBOOST_THREAD_FWD_PARAM_A(z, n, unused) , autoboost::forward<A##n>(arg##n)
#define AUTOBOOST_THREAD_DCL(z, n, unused) Arg##n v##n;
#define AUTOBOOST_THREAD_MOVE_PARAM(z, n, unused) , v##n(autoboost::move(arg##n))
#define AUTOBOOST_THREAD_FORWARD_PARAM_A(z, n, unused) , v##n(autoboost::forward<A##n>(arg##n))
#define AUTOBOOST_THREAD_MOVE_RHS_PARAM(z, n, unused) , v##n(autoboost::move(x.v##n))
#define AUTOBOOST_THREAD_MOVE_DCL(z, n, unused) , autoboost::move(v##n)
#define AUTOBOOST_THREAD_MOVE_DCL_T(z, n, unused) AUTOBOOST_PP_COMMA_IF(n) autoboost::move(v##n)
#define AUTOBOOST_THREAD_ARG_DEF(z, n, unused) , class Arg##n = tuples::null_type

  template  <class Fp, class Arg = tuples::null_type
    AUTOBOOST_PP_REPEAT(AUTOBOOST_THREAD_MAX_ARGS, AUTOBOOST_THREAD_ARG_DEF, ~)
    >
    class invoker;

#define AUTOBOOST_THREAD_ASYNC_FUNCT(z, n, unused) \
    template <class Fp AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_PP_ENUM_PARAMS(n, class Arg) > \
    class invoker<Fp AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_PP_ENUM_PARAMS(n, Arg)> \
    { \
      Fp fp_; \
      AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_DCL, ~) \
    public: \
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker) \
      typedef typename result_of<Fp(AUTOBOOST_PP_ENUM_PARAMS(n, Arg))>::type result_type; \
      \
      template <class F AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_PP_ENUM_PARAMS(n, class A) > \
      AUTOBOOST_SYMBOL_VISIBLE \
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(F) f \
          AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_FWD_REF_A, ~) \
      ) \
      : fp_(autoboost::forward<F>(f)) \
      AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_FORWARD_PARAM_A, ~) \
      {} \
      \
      AUTOBOOST_SYMBOL_VISIBLE \
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) x) \
      : fp_(autoboost::move(x.fp_)) \
      AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_MOVE_RHS_PARAM, ~) \
      {} \
      \
      result_type operator()() { \
        return invoke(autoboost::move(fp_) \
            AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_MOVE_DCL, ~) \
        ); \
      } \
    }; \
    \
    template <class R AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_PP_ENUM_PARAMS(n, class Arg) > \
    class invoker<R(*)(AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_RV_REF_ARG_T, ~)) AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_PP_ENUM_PARAMS(n, Arg)> \
    { \
      typedef R(*Fp)(AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_RV_REF_ARG_T, ~)); \
      Fp fp_; \
      AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_DCL, ~) \
    public: \
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker) \
      typedef typename result_of<Fp(AUTOBOOST_PP_ENUM_PARAMS(n, Arg))>::type result_type; \
      \
      template <class R2 AUTOBOOST_PP_COMMA_IF(n) AUTOBOOST_PP_ENUM_PARAMS(n, class A) > \
      AUTOBOOST_SYMBOL_VISIBLE \
      explicit invoker(R2(*f)(AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_RV_REF_A_T, ~))  \
          AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_FWD_REF_A, ~) \
      ) \
      : fp_(f) \
      AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_FORWARD_PARAM_A, ~) \
      {} \
      \
      AUTOBOOST_SYMBOL_VISIBLE \
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) x) \
      : fp_(x.fp_) \
      AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_MOVE_RHS_PARAM, ~) \
      {} \
      \
      result_type operator()() { \
        return fp_( \
            AUTOBOOST_PP_REPEAT(n, AUTOBOOST_THREAD_MOVE_DCL_T, ~) \
        ); \
      } \
    };

    AUTOBOOST_PP_REPEAT(AUTOBOOST_THREAD_MAX_ARGS, AUTOBOOST_THREAD_ASYNC_FUNCT, ~)

    #undef AUTOBOOST_THREAD_RV_REF_ARG_T
    #undef AUTOBOOST_THREAD_RV_REF_ARG
    #undef AUTOBOOST_THREAD_FWD_REF_ARG
    #undef AUTOBOOST_THREAD_FWD_REF_A
    #undef AUTOBOOST_THREAD_FWD_PARAM
    #undef AUTOBOOST_THREAD_FWD_PARAM_A
    #undef AUTOBOOST_THREAD_DCL
    #undef AUTOBOOST_THREAD_MOVE_PARAM
    #undef AUTOBOOST_THREAD_MOVE_RHS_PARAM
    #undef AUTOBOOST_THREAD_MOVE_DCL
    #undef AUTOBOOST_THREAD_ARG_DEF
    #undef AUTOBOOST_THREAD_ASYNC_FUNCT

#else

    template <class Fp,
    class T0 = tuples::null_type, class T1 = tuples::null_type, class T2 = tuples::null_type,
    class T3 = tuples::null_type, class T4 = tuples::null_type, class T5 = tuples::null_type,
    class T6 = tuples::null_type, class T7 = tuples::null_type, class T8 = tuples::null_type
    , class T9 = tuples::null_type
    >
    class invoker;

    template <class Fp,
    class T0 , class T1 , class T2 ,
    class T3 , class T4 , class T5 ,
    class T6 , class T7 , class T8 >
    class invoker<Fp, T0, T1, T2, T3, T4, T5, T6, T7, T8>
    {
      Fp fp_;
      T0 v0_;
      T1 v1_;
      T2 v2_;
      T3 v3_;
      T4 v4_;
      T5 v5_;
      T6 v6_;
      T7 v7_;
      T8 v8_;
      //::autoboost::tuple<Fp, T0, T1, T2, T3, T4, T5, T6, T7, T8> f_;

    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp(T0, T1, T2, T3, T4, T5, T6, T7, T8)>::type result_type;

      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(Fp) f
          , AUTOBOOST_THREAD_RV_REF(T0) a0
          , AUTOBOOST_THREAD_RV_REF(T1) a1
          , AUTOBOOST_THREAD_RV_REF(T2) a2
          , AUTOBOOST_THREAD_RV_REF(T3) a3
          , AUTOBOOST_THREAD_RV_REF(T4) a4
          , AUTOBOOST_THREAD_RV_REF(T5) a5
          , AUTOBOOST_THREAD_RV_REF(T6) a6
          , AUTOBOOST_THREAD_RV_REF(T7) a7
          , AUTOBOOST_THREAD_RV_REF(T8) a8
      )
      : fp_(autoboost::move(f))
      , v0_(autoboost::move(a0))
      , v1_(autoboost::move(a1))
      , v2_(autoboost::move(a2))
      , v3_(autoboost::move(a3))
      , v4_(autoboost::move(a4))
      , v5_(autoboost::move(a5))
      , v6_(autoboost::move(a6))
      , v7_(autoboost::move(a7))
      , v8_(autoboost::move(a8))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(autoboost::move(AUTOBOOST_THREAD_RV(f).fp))
      , v0_(autoboost::move(AUTOBOOST_THREAD_RV(f).v0_))
      , v1_(autoboost::move(AUTOBOOST_THREAD_RV(f).v1_))
      , v2_(autoboost::move(AUTOBOOST_THREAD_RV(f).v2_))
      , v3_(autoboost::move(AUTOBOOST_THREAD_RV(f).v3_))
      , v4_(autoboost::move(AUTOBOOST_THREAD_RV(f).v4_))
      , v5_(autoboost::move(AUTOBOOST_THREAD_RV(f).v5_))
      , v6_(autoboost::move(AUTOBOOST_THREAD_RV(f).v6_))
      , v7_(autoboost::move(AUTOBOOST_THREAD_RV(f).v7_))
      , v8_(autoboost::move(AUTOBOOST_THREAD_RV(f).v8_))
      {}

      result_type operator()()
      {
        return invoke(autoboost::move(fp_)
            , autoboost::move(v0_)
            , autoboost::move(v1_)
            , autoboost::move(v2_)
            , autoboost::move(v3_)
            , autoboost::move(v4_)
            , autoboost::move(v5_)
            , autoboost::move(v6_)
            , autoboost::move(v7_)
            , autoboost::move(v8_)
        );
      }
    };
    template <class Fp, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7 >
    class invoker<Fp, T0, T1, T2, T3, T4, T5, T6, T7>
    {
      Fp fp_;
      T0 v0_;
      T1 v1_;
      T2 v2_;
      T3 v3_;
      T4 v4_;
      T5 v5_;
      T6 v6_;
      T7 v7_;
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp(T0, T1, T2, T3, T4, T5, T6, T7)>::type result_type;

      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(Fp) f
          , AUTOBOOST_THREAD_RV_REF(T0) a0
          , AUTOBOOST_THREAD_RV_REF(T1) a1
          , AUTOBOOST_THREAD_RV_REF(T2) a2
          , AUTOBOOST_THREAD_RV_REF(T3) a3
          , AUTOBOOST_THREAD_RV_REF(T4) a4
          , AUTOBOOST_THREAD_RV_REF(T5) a5
          , AUTOBOOST_THREAD_RV_REF(T6) a6
          , AUTOBOOST_THREAD_RV_REF(T7) a7
      )
      : fp_(autoboost::move(f))
      , v0_(autoboost::move(a0))
      , v1_(autoboost::move(a1))
      , v2_(autoboost::move(a2))
      , v3_(autoboost::move(a3))
      , v4_(autoboost::move(a4))
      , v5_(autoboost::move(a5))
      , v6_(autoboost::move(a6))
      , v7_(autoboost::move(a7))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(autoboost::move(AUTOBOOST_THREAD_RV(f).fp))
      , v0_(autoboost::move(AUTOBOOST_THREAD_RV(f).v0_))
      , v1_(autoboost::move(AUTOBOOST_THREAD_RV(f).v1_))
      , v2_(autoboost::move(AUTOBOOST_THREAD_RV(f).v2_))
      , v3_(autoboost::move(AUTOBOOST_THREAD_RV(f).v3_))
      , v4_(autoboost::move(AUTOBOOST_THREAD_RV(f).v4_))
      , v5_(autoboost::move(AUTOBOOST_THREAD_RV(f).v5_))
      , v6_(autoboost::move(AUTOBOOST_THREAD_RV(f).v6_))
      , v7_(autoboost::move(AUTOBOOST_THREAD_RV(f).v7_))
      {}

      result_type operator()()
      {
        return invoke(autoboost::move(fp_)
            , autoboost::move(v0_)
            , autoboost::move(v1_)
            , autoboost::move(v2_)
            , autoboost::move(v3_)
            , autoboost::move(v4_)
            , autoboost::move(v5_)
            , autoboost::move(v6_)
            , autoboost::move(v7_)
        );
      }
    };
    template <class Fp, class T0, class T1, class T2, class T3, class T4, class T5, class T6>
    class invoker<Fp, T0, T1, T2, T3, T4, T5, T6>
    {
      Fp fp_;
      T0 v0_;
      T1 v1_;
      T2 v2_;
      T3 v3_;
      T4 v4_;
      T5 v5_;
      T6 v6_;
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp(T0, T1, T2, T3, T4, T5, T6)>::type result_type;

      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(Fp) f
          , AUTOBOOST_THREAD_RV_REF(T0) a0
          , AUTOBOOST_THREAD_RV_REF(T1) a1
          , AUTOBOOST_THREAD_RV_REF(T2) a2
          , AUTOBOOST_THREAD_RV_REF(T3) a3
          , AUTOBOOST_THREAD_RV_REF(T4) a4
          , AUTOBOOST_THREAD_RV_REF(T5) a5
          , AUTOBOOST_THREAD_RV_REF(T6) a6
      )
      : fp_(autoboost::move(f))
      , v0_(autoboost::move(a0))
      , v1_(autoboost::move(a1))
      , v2_(autoboost::move(a2))
      , v3_(autoboost::move(a3))
      , v4_(autoboost::move(a4))
      , v5_(autoboost::move(a5))
      , v6_(autoboost::move(a6))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(autoboost::move(AUTOBOOST_THREAD_RV(f).fp))
      , v0_(autoboost::move(AUTOBOOST_THREAD_RV(f).v0_))
      , v1_(autoboost::move(AUTOBOOST_THREAD_RV(f).v1_))
      , v2_(autoboost::move(AUTOBOOST_THREAD_RV(f).v2_))
      , v3_(autoboost::move(AUTOBOOST_THREAD_RV(f).v3_))
      , v4_(autoboost::move(AUTOBOOST_THREAD_RV(f).v4_))
      , v5_(autoboost::move(AUTOBOOST_THREAD_RV(f).v5_))
      , v6_(autoboost::move(AUTOBOOST_THREAD_RV(f).v6_))
      {}

      result_type operator()()
      {
        return invoke(autoboost::move(fp_)
            , autoboost::move(v0_)
            , autoboost::move(v1_)
            , autoboost::move(v2_)
            , autoboost::move(v3_)
            , autoboost::move(v4_)
            , autoboost::move(v5_)
            , autoboost::move(v6_)
        );
      }
    };
    template <class Fp, class T0, class T1, class T2, class T3, class T4, class T5>
    class invoker<Fp, T0, T1, T2, T3, T4, T5>
    {
      Fp fp_;
      T0 v0_;
      T1 v1_;
      T2 v2_;
      T3 v3_;
      T4 v4_;
      T5 v5_;
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp(T0, T1, T2, T3, T4, T5)>::type result_type;

      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(Fp) f
          , AUTOBOOST_THREAD_RV_REF(T0) a0
          , AUTOBOOST_THREAD_RV_REF(T1) a1
          , AUTOBOOST_THREAD_RV_REF(T2) a2
          , AUTOBOOST_THREAD_RV_REF(T3) a3
          , AUTOBOOST_THREAD_RV_REF(T4) a4
          , AUTOBOOST_THREAD_RV_REF(T5) a5
      )
      : fp_(autoboost::move(f))
      , v0_(autoboost::move(a0))
      , v1_(autoboost::move(a1))
      , v2_(autoboost::move(a2))
      , v3_(autoboost::move(a3))
      , v4_(autoboost::move(a4))
      , v5_(autoboost::move(a5))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(autoboost::move(AUTOBOOST_THREAD_RV(f).fp))
      , v0_(autoboost::move(AUTOBOOST_THREAD_RV(f).v0_))
      , v1_(autoboost::move(AUTOBOOST_THREAD_RV(f).v1_))
      , v2_(autoboost::move(AUTOBOOST_THREAD_RV(f).v2_))
      , v3_(autoboost::move(AUTOBOOST_THREAD_RV(f).v3_))
      , v4_(autoboost::move(AUTOBOOST_THREAD_RV(f).v4_))
      , v5_(autoboost::move(AUTOBOOST_THREAD_RV(f).v5_))
      {}

      result_type operator()()
      {
        return invoke(autoboost::move(fp_)
            , autoboost::move(v0_)
            , autoboost::move(v1_)
            , autoboost::move(v2_)
            , autoboost::move(v3_)
            , autoboost::move(v4_)
            , autoboost::move(v5_)
        );
      }
    };
    template <class Fp, class T0, class T1, class T2, class T3, class T4>
    class invoker<Fp, T0, T1, T2, T3, T4>
    {
      Fp fp_;
      T0 v0_;
      T1 v1_;
      T2 v2_;
      T3 v3_;
      T4 v4_;
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp(T0, T1, T2, T3, T4)>::type result_type;

      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(Fp) f
          , AUTOBOOST_THREAD_RV_REF(T0) a0
          , AUTOBOOST_THREAD_RV_REF(T1) a1
          , AUTOBOOST_THREAD_RV_REF(T2) a2
          , AUTOBOOST_THREAD_RV_REF(T3) a3
          , AUTOBOOST_THREAD_RV_REF(T4) a4
      )
      : fp_(autoboost::move(f))
      , v0_(autoboost::move(a0))
      , v1_(autoboost::move(a1))
      , v2_(autoboost::move(a2))
      , v3_(autoboost::move(a3))
      , v4_(autoboost::move(a4))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(autoboost::move(AUTOBOOST_THREAD_RV(f).fp))
      , v0_(autoboost::move(AUTOBOOST_THREAD_RV(f).v0_))
      , v1_(autoboost::move(AUTOBOOST_THREAD_RV(f).v1_))
      , v2_(autoboost::move(AUTOBOOST_THREAD_RV(f).v2_))
      , v3_(autoboost::move(AUTOBOOST_THREAD_RV(f).v3_))
      , v4_(autoboost::move(AUTOBOOST_THREAD_RV(f).v4_))
      {}

      result_type operator()()
      {
        return invoke(autoboost::move(fp_)
            , autoboost::move(v0_)
            , autoboost::move(v1_)
            , autoboost::move(v2_)
            , autoboost::move(v3_)
            , autoboost::move(v4_)
        );
      }
    };
    template <class Fp, class T0, class T1, class T2, class T3>
    class invoker<Fp, T0, T1, T2, T3>
    {
      Fp fp_;
      T0 v0_;
      T1 v1_;
      T2 v2_;
      T3 v3_;
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp(T0, T1, T2, T3)>::type result_type;

      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(Fp) f
          , AUTOBOOST_THREAD_RV_REF(T0) a0
          , AUTOBOOST_THREAD_RV_REF(T1) a1
          , AUTOBOOST_THREAD_RV_REF(T2) a2
          , AUTOBOOST_THREAD_RV_REF(T3) a3
      )
      : fp_(autoboost::move(f))
      , v0_(autoboost::move(a0))
      , v1_(autoboost::move(a1))
      , v2_(autoboost::move(a2))
      , v3_(autoboost::move(a3))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(autoboost::move(AUTOBOOST_THREAD_RV(f).fp))
      , v0_(autoboost::move(AUTOBOOST_THREAD_RV(f).v0_))
      , v1_(autoboost::move(AUTOBOOST_THREAD_RV(f).v1_))
      , v2_(autoboost::move(AUTOBOOST_THREAD_RV(f).v2_))
      , v3_(autoboost::move(AUTOBOOST_THREAD_RV(f).v3_))
      {}

      result_type operator()()
      {
        return invoke(autoboost::move(fp_)
            , autoboost::move(v0_)
            , autoboost::move(v1_)
            , autoboost::move(v2_)
            , autoboost::move(v3_)
        );
      }
    };
    template <class Fp, class T0, class T1, class T2>
    class invoker<Fp, T0, T1, T2>
    {
      Fp fp_;
      T0 v0_;
      T1 v1_;
      T2 v2_;
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp(T0, T1, T2)>::type result_type;

      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(Fp) f
          , AUTOBOOST_THREAD_RV_REF(T0) a0
          , AUTOBOOST_THREAD_RV_REF(T1) a1
          , AUTOBOOST_THREAD_RV_REF(T2) a2
      )
      : fp_(autoboost::move(f))
      , v0_(autoboost::move(a0))
      , v1_(autoboost::move(a1))
      , v2_(autoboost::move(a2))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(autoboost::move(AUTOBOOST_THREAD_RV(f).fp))
      , v0_(autoboost::move(AUTOBOOST_THREAD_RV(f).v0_))
      , v1_(autoboost::move(AUTOBOOST_THREAD_RV(f).v1_))
      , v2_(autoboost::move(AUTOBOOST_THREAD_RV(f).v2_))
      {}

      result_type operator()()
      {
        return invoke(autoboost::move(fp_)
            , autoboost::move(v0_)
            , autoboost::move(v1_)
            , autoboost::move(v2_)
        );
      }
    };
    template <class Fp, class T0, class T1>
    class invoker<Fp, T0, T1>
    {
      Fp fp_;
      T0 v0_;
      T1 v1_;
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp(T0, T1)>::type result_type;

      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(Fp) f
          , AUTOBOOST_THREAD_RV_REF(T0) a0
          , AUTOBOOST_THREAD_RV_REF(T1) a1
      )
      : fp_(autoboost::move(f))
      , v0_(autoboost::move(a0))
      , v1_(autoboost::move(a1))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(autoboost::move(AUTOBOOST_THREAD_RV(f).fp))
      , v0_(autoboost::move(AUTOBOOST_THREAD_RV(f).v0_))
      , v1_(autoboost::move(AUTOBOOST_THREAD_RV(f).v1_))
      {}

      result_type operator()()
      {
        return invoke(autoboost::move(fp_)
            , autoboost::move(v0_)
            , autoboost::move(v1_)
        );
      }
    };
    template <class Fp, class T0>
    class invoker<Fp, T0>
    {
      Fp fp_;
      T0 v0_;
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp(T0)>::type result_type;

      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(Fp) f
          , AUTOBOOST_THREAD_RV_REF(T0) a0
      )
      : fp_(autoboost::move(f))
      , v0_(autoboost::move(a0))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(autoboost::move(AUTOBOOST_THREAD_RV(f).fp))
      , v0_(autoboost::move(AUTOBOOST_THREAD_RV(f).v0_))
      {}

      result_type operator()()
      {
        return invoke(autoboost::move(fp_)
            , autoboost::move(v0_)
        );
      }
    };
    template <class Fp>
    class invoker<Fp>
    {
      Fp fp_;
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp()>::type result_type;
      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(AUTOBOOST_THREAD_FWD_REF(Fp) f)
      : fp_(autoboost::move(f))
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(autoboost::move(f.fp_))
      {}
      result_type operator()()
      {
        return fp_();
      }
    };
    template <class R>
    class invoker<R(*)()>
    {
      typedef R(*Fp)();
      Fp fp_;
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(invoker)
      typedef typename result_of<Fp()>::type result_type;
      AUTOBOOST_SYMBOL_VISIBLE
      explicit invoker(Fp f)
      : fp_(f)
      {}

      AUTOBOOST_SYMBOL_VISIBLE
      invoker(AUTOBOOST_THREAD_RV_REF(invoker) f)
      : fp_(f.fp_)
      {}
      result_type operator()()
      {
        return fp_();
      }
    };
#endif
#endif

  }
}

#include <autoboost/thread/detail/variadic_footer.hpp>

#endif // header
