// Copyright (C) 2012 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_THREAD_DETAIL_LOG_HPP
#define AUTOBOOST_THREAD_DETAIL_LOG_HPP

#include <autoboost/thread/detail/config.hpp>
#if defined AUTOBOOST_THREAD_USES_LOG
#include <autoboost/thread/recursive_mutex.hpp>
#include <autoboost/thread/lock_guard.hpp>
#if defined AUTOBOOST_THREAD_USES_LOG_THREAD_ID
#include <autoboost/thread/thread.hpp>
#endif
#include <iostream>

namespace autoboost
{
  namespace thread_detail
  {
    inline autoboost::recursive_mutex& terminal_mutex()
    {
      static autoboost::recursive_mutex mtx;
      return mtx;
    }

  }
}
#if defined AUTOBOOST_THREAD_USES_LOG_THREAD_ID

#define AUTOBOOST_THREAD_LOG \
  { \
    autoboost::lock_guard<autoboost::recursive_mutex> _lk_(autoboost::thread_detail::terminal_mutex()); \
    std::cout << autoboost::this_thread::get_id() << " - "<<__FILE__<<"["<<__LINE__<<"] " <<std::dec
#else

#define AUTOBOOST_THREAD_LOG \
{ \
  autoboost::lock_guard<autoboost::recursive_mutex> _lk_(autoboost::thread_detail::terminal_mutex()); \
  std::cout << __FILE__<<"["<<__LINE__<<"] " <<std::dec

#endif
#define AUTOBOOST_THREAD_END_LOG \
    std::dec << std::endl; \
  }

#else

namespace autoboost
{
  namespace thread_detail
  {
    struct dummy_stream_t
    {
    };

    template <typename T>
    inline dummy_stream_t const& operator<<(dummy_stream_t const& os, T)
    {
      return os;
    }

    inline dummy_stream_t const& operator<<(dummy_stream_t const& os, dummy_stream_t const&)
    {
      return os;
    }


    AUTOBOOST_CONSTEXPR_OR_CONST dummy_stream_t dummy_stream = {};

  }
}

#define AUTOBOOST_THREAD_LOG if (true) {} else autoboost::thread_detail::dummy_stream
#define AUTOBOOST_THREAD_END_LOG autoboost::thread_detail::dummy_stream

#endif

#define AUTOBOOST_THREAD_TRACE AUTOBOOST_THREAD_LOG << AUTOBOOST_THREAD_END_LOG


#endif // header
