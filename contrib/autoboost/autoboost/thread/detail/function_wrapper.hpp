// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/09 Vicente J. Botet Escriba
//    Adapt to autoboost from CCIA C++11 implementation
//    Make use of Boost.Move

#ifndef AUTOBOOST_THREAD_DETAIL_FUNCTION_WRAPPER_HPP
#define AUTOBOOST_THREAD_DETAIL_FUNCTION_WRAPPER_HPP

#include <autoboost/config.hpp>
#include <autoboost/thread/detail/memory.hpp>
#include <autoboost/thread/detail/move.hpp>

#include <autoboost/thread/csbl/memory/unique_ptr.hpp>

#include <memory>
#include <functional>

namespace autoboost
{
  namespace detail
  {
    class function_wrapper
    {
      struct impl_base
      {
        virtual void call()=0;
        virtual ~impl_base()
        {
        }
      };
      typedef autoboost::csbl::unique_ptr<impl_base> impl_base_type;
      impl_base_type impl;
      template <typename F>
      struct impl_type: impl_base
      {
        F f;
        impl_type(F const &f_)
          : f(f_)
        {}
        impl_type(AUTOBOOST_THREAD_RV_REF(F) f_)
          : f(autoboost::move(f_))
        {}

        void call()
        {
          if (impl) f();
        }
      };
    public:
      AUTOBOOST_THREAD_MOVABLE_ONLY(function_wrapper)

//#if ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
      template<typename F>
      function_wrapper(F const& f):
      impl(new impl_type<F>(f))
      {}
//#endif
      template<typename F>
      function_wrapper(AUTOBOOST_THREAD_RV_REF(F) f):
      impl(new impl_type<F>(autoboost::forward<F>(f)))
      {}
      function_wrapper(AUTOBOOST_THREAD_RV_REF(function_wrapper) other) AUTOBOOST_NOEXCEPT :
      impl(other.impl)
      {
        other.impl = 0;
      }
      function_wrapper()
        : impl(0)
      {
      }
      ~function_wrapper()
      {
      }

      function_wrapper& operator=(AUTOBOOST_THREAD_RV_REF(function_wrapper) other) AUTOBOOST_NOEXCEPT
      {
        impl=other.impl;
        other.impl=0;
        return *this;
      }

      void operator()()
      { impl->call();}

    };
  }
}

#endif // header
