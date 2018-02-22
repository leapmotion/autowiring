// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2009-2012 Anthony Williams
// (C) Copyright 2012 Vicente J. Botet Escriba

// Based on the Anthony's idea of scoped_thread in CCiA

#ifndef AUTOBOOST_THREAD_THREAD_FUNCTORS_HPP
#define AUTOBOOST_THREAD_THREAD_FUNCTORS_HPP

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/delete.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/thread_only.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{

  struct detach
  {
    template <class Thread>
    void operator()(Thread& t)
    {
      t.detach();
    }
  };

  struct detach_if_joinable
  {
    template <class Thread>
    void operator()(Thread& t)
    {
      if (t.joinable())
      {
        t.detach();
      }
    }
  };

  struct join_if_joinable
  {
    template <class Thread>
    void operator()(Thread& t)
    {
      if (t.joinable())
      {
        t.join();
      }
    }
  };

#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
  struct interrupt_and_join_if_joinable
  {
    template <class Thread>
    void operator()(Thread& t)
    {
      if (t.joinable())
      {
        t.interrupt();
        t.join();
      }
    }
  };
#endif
}
#include <autoboost/config/abi_suffix.hpp>

#endif
