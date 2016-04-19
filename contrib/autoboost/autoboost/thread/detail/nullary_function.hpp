// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/09 Vicente J. Botet Escriba
//    Adapt to autoboost from CCIA C++11 implementation
//    Make use of Boost.Move

#ifndef AUTOBOOST_THREAD_DETAIL_NULLARY_FUNCTION_HPP
#define AUTOBOOST_THREAD_DETAIL_NULLARY_FUNCTION_HPP

#include <autoboost/config.hpp>
#include <autoboost/thread/detail/memory.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/csbl/memory/shared_ptr.hpp>
#include <autoboost/type_traits/decay.hpp>

namespace autoboost
{
  namespace detail
  {

    template <typename F>
    class nullary_function;
    template <>
    class nullary_function<void()>
    {
      struct impl_base
      {
        virtual void call()=0;
        virtual ~impl_base()
        {
        }
      };
      csbl::shared_ptr<impl_base> impl;
      template <typename F>
      struct impl_type: impl_base
      {
        F f;
#ifdef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
        impl_type(F &f_)
          : f(f_)
        {}
#endif
        impl_type(AUTOBOOST_THREAD_RV_REF(F) f_)
          : f(autoboost::move(f_))
        {}

        void call()
        {
          f();
        }
      };
      struct impl_type_ptr: impl_base
      {
        void (*f)();
        impl_type_ptr(void (*f_)())
          : f(f_)
        {}
        void call()
        {
          f();
        }
      };
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(nullary_function)

      explicit nullary_function(void (*f)()):
      impl(new impl_type_ptr(f))
      {}

#ifdef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
      template<typename F>
      explicit nullary_function(F& f):
      impl(new impl_type<F>(f))
      {}
#endif
      template<typename F>
      nullary_function(AUTOBOOST_THREAD_RV_REF(F) f):
      impl(new impl_type<typename decay<F>::type>(thread_detail::decay_copy(autoboost::forward<F>(f))))
      {}

      nullary_function()
        : impl()
      {
      }
      nullary_function(nullary_function const& other) AUTOBOOST_NOEXCEPT :
      impl(other.impl)
      {
      }
      nullary_function(AUTOBOOST_THREAD_RV_REF(nullary_function) other) AUTOBOOST_NOEXCEPT :
#if defined AUTOBOOST_NO_CXX11_SMART_PTR
      impl(AUTOBOOST_THREAD_RV(other).impl)
      {
        AUTOBOOST_THREAD_RV(other).impl.reset();
      }
#else
      impl(autoboost::move(other.impl))
      {
      }
#endif
      ~nullary_function()
      {
      }

      nullary_function& operator=(AUTOBOOST_THREAD_COPY_ASSIGN_REF(nullary_function) other) AUTOBOOST_NOEXCEPT
      {
        impl=other.impl;
        return *this;
      }
      nullary_function& operator=(AUTOBOOST_THREAD_RV_REF(nullary_function) other) AUTOBOOST_NOEXCEPT
      {
#if defined AUTOBOOST_NO_CXX11_SMART_PTR
        impl=AUTOBOOST_THREAD_RV(other).impl;
        AUTOBOOST_THREAD_RV(other).impl.reset();
#else
        impl = autoboost::move(other.impl);
#endif
        return *this;
      }


      void operator()()
      { if (impl) impl->call();}

    };

    template <typename R>
    class nullary_function<R()>
    {
      struct impl_base
      {
        virtual R call()=0;
        virtual ~impl_base()
        {
        }
      };
      csbl::shared_ptr<impl_base> impl;
      template <typename F>
      struct impl_type: impl_base
      {
        F f;
#ifdef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
        impl_type(F &f_)
          : f(f_)
        {}
#endif
        impl_type(AUTOBOOST_THREAD_RV_REF(F) f_)
          : f(autoboost::move(f_))
        {}

        R call()
        {
          return f();
        }
      };
      struct impl_type_ptr: impl_base
      {
        R (*f)();
        impl_type_ptr(R (*f_)())
          : f(f_)
        {}

        R call()
        {
          return f();
        }
      };
    public:
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(nullary_function)

      nullary_function(R (*f)()):
      impl(new impl_type_ptr(f))
      {}
#ifdef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
      template<typename F>
      nullary_function(F& f):
      impl(new impl_type<F>(f))
      {}
#endif
      template<typename F>
      nullary_function(AUTOBOOST_THREAD_RV_REF(F) f):
      impl(new impl_type<typename decay<F>::type>(thread_detail::decay_copy(autoboost::forward<F>(f))))
      {}

      nullary_function(nullary_function const& other) AUTOBOOST_NOEXCEPT :
      impl(other.impl)
      {
      }
      nullary_function(AUTOBOOST_THREAD_RV_REF(nullary_function) other) AUTOBOOST_NOEXCEPT :
#if defined AUTOBOOST_NO_CXX11_SMART_PTR
      impl(AUTOBOOST_THREAD_RV(other).impl)
      {
        AUTOBOOST_THREAD_RV(other).impl.reset();
      }
#else
      impl(autoboost::move(other.impl))
      {
      }
#endif
      nullary_function()
        : impl()
      {
      }
      ~nullary_function()
      {
      }

      nullary_function& operator=(AUTOBOOST_THREAD_COPY_ASSIGN_REF(nullary_function) other) AUTOBOOST_NOEXCEPT
      {
        impl=other.impl;
        return *this;
      }
      nullary_function& operator=(AUTOBOOST_THREAD_RV_REF(nullary_function) other) AUTOBOOST_NOEXCEPT
      {
#if defined AUTOBOOST_NO_CXX11_SMART_PTR
        impl=AUTOBOOST_THREAD_RV(other).impl;
        AUTOBOOST_THREAD_RV(other).impl.reset();
#else
        impl = autoboost::move(other.impl);
#endif
        return *this;
      }

      R operator()()
      { if (impl) return impl->call(); else return R();}

    };
  }
  AUTOBOOST_THREAD_DCL_MOVABLE_BEG(F) detail::nullary_function<F> AUTOBOOST_THREAD_DCL_MOVABLE_END
}

#endif // header
