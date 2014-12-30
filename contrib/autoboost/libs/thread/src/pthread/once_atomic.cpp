//  (C) Copyright 2013 Andrey Semashev
//  (C) Copyright 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//#define __STDC_CONSTANT_MACROS
#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/once.hpp>
#include <autoboost/thread/pthread/pthread_mutex_scoped_lock.hpp>
#include <autoboost/assert.hpp>
#include <autoboost/static_assert.hpp>
#include <autoboost/atomic.hpp>
#include <autoboost/memory_order.hpp>
#include <pthread.h>

namespace autoboost
{
  namespace thread_detail
  {

    enum flag_states
    {
      uninitialized, in_progress, initialized
    };


#ifndef AUTOBOOST_THREAD_PROVIDES_ONCE_CXX11
    AUTOBOOST_STATIC_ASSERT_MSG(sizeof(atomic_int_type) == sizeof(atomic_type), "Boost.Thread: unsupported platform");
#endif

    static pthread_mutex_t once_mutex = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t once_cv = PTHREAD_COND_INITIALIZER;

    AUTOBOOST_THREAD_DECL bool enter_once_region(once_flag& flag) AUTOBOOST_NOEXCEPT
    {
      atomic_type& f = get_atomic_storage(flag);
      if (f.load(memory_order_acquire) != initialized)
      {
        pthread::pthread_mutex_scoped_lock lk(&once_mutex);
        if (f.load(memory_order_acquire) != initialized)
        {
          while (true)
          {
            atomic_int_type expected = uninitialized;
            if (f.compare_exchange_strong(expected, in_progress, memory_order_acq_rel, memory_order_acquire))
            {
              // We have set the flag to in_progress
              return true;
            }
            else if (expected == initialized)
            {
              // Another thread managed to complete the initialization
              return false;
            }
            else
            {
              // Wait until the initialization is complete
              //pthread::pthread_mutex_scoped_lock lk(&once_mutex);
              AUTOBOOST_VERIFY(!pthread_cond_wait(&once_cv, &once_mutex));
            }
          }
        }
      }
      return false;
    }

    AUTOBOOST_THREAD_DECL void commit_once_region(once_flag& flag) AUTOBOOST_NOEXCEPT
    {
      atomic_type& f = get_atomic_storage(flag);
      {
        pthread::pthread_mutex_scoped_lock lk(&once_mutex);
        f.store(initialized, memory_order_release);
      }
      AUTOBOOST_VERIFY(!pthread_cond_broadcast(&once_cv));
    }

    AUTOBOOST_THREAD_DECL void rollback_once_region(once_flag& flag) AUTOBOOST_NOEXCEPT
    {
      atomic_type& f = get_atomic_storage(flag);
      {
        pthread::pthread_mutex_scoped_lock lk(&once_mutex);
        f.store(uninitialized, memory_order_release);
      }
      AUTOBOOST_VERIFY(!pthread_cond_broadcast(&once_cv));
    }

  } // namespace thread_detail

} // namespace autoboost
