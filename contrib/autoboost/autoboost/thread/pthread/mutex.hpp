#ifndef AUTOBOOST_THREAD_PTHREAD_MUTEX_HPP
#define AUTOBOOST_THREAD_PTHREAD_MUTEX_HPP
// (C) Copyright 2007-8 Anthony Williams
// (C) Copyright 2011,2012,2015 Vicente J. Botet Escriba
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/assert.hpp>
#include <pthread.h>
#include <autoboost/throw_exception.hpp>
#include <autoboost/core/ignore_unused.hpp>
#include <autoboost/thread/exceptions.hpp>
#if defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
#include <autoboost/thread/lock_types.hpp>
#endif
#include <autoboost/thread/thread_time.hpp>
#include <autoboost/thread/xtime.hpp>
#include <autoboost/assert.hpp>
#include <errno.h>
#include <autoboost/thread/pthread/timespec.hpp>
#include <autoboost/thread/pthread/pthread_mutex_scoped_lock.hpp>
#ifdef AUTOBOOST_THREAD_USES_CHRONO
#include <autoboost/chrono/system_clocks.hpp>
#include <autoboost/chrono/ceil.hpp>
#endif
#include <autoboost/thread/detail/delete.hpp>

#if (defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS-0)>=200112L) \
 || (defined(__ANDROID__) && defined(__ANDROID_API__) && __ANDROID_API__ >= 21)
#ifndef AUTOBOOST_PTHREAD_HAS_TIMEDLOCK
#define AUTOBOOST_PTHREAD_HAS_TIMEDLOCK
#endif
#endif


#include <autoboost/config/abi_prefix.hpp>

#ifndef AUTOBOOST_THREAD_HAS_NO_EINTR_BUG
#define AUTOBOOST_THREAD_HAS_EINTR_BUG
#endif

namespace autoboost
{
  namespace posix {
#ifdef AUTOBOOST_THREAD_HAS_EINTR_BUG
    AUTOBOOST_FORCEINLINE int pthread_mutex_destroy(pthread_mutex_t* m)
    {
      int ret;
      do
      {
          ret = ::pthread_mutex_destroy(m);
      } while (ret == EINTR);
      return ret;
    }
    AUTOBOOST_FORCEINLINE int pthread_mutex_lock(pthread_mutex_t* m)
    {
      int ret;
      do
      {
          ret = ::pthread_mutex_lock(m);
      } while (ret == EINTR);
      return ret;
    }
    AUTOBOOST_FORCEINLINE int pthread_mutex_unlock(pthread_mutex_t* m)
    {
      int ret;
      do
      {
          ret = ::pthread_mutex_unlock(m);
      } while (ret == EINTR);
      return ret;
    }
#else
    AUTOBOOST_FORCEINLINE int pthread_mutex_destroy(pthread_mutex_t* m)
    {
      return ::pthread_mutex_destroy(m);
    }
    AUTOBOOST_FORCEINLINE int pthread_mutex_lock(pthread_mutex_t* m)
    {
      return ::pthread_mutex_lock(m);
    }
    AUTOBOOST_FORCEINLINE int pthread_mutex_unlock(pthread_mutex_t* m)
    {
      return ::pthread_mutex_unlock(m);
    }

#endif

  }
    class mutex
    {
    private:
        pthread_mutex_t m;
    public:
        AUTOBOOST_THREAD_NO_COPYABLE(mutex)

        mutex()
        {
            int const res=pthread_mutex_init(&m,NULL);
            if(res)
            {
                autoboost::throw_exception(thread_resource_error(res, "autoboost:: mutex constructor failed in pthread_mutex_init"));
            }
        }
        ~mutex()
        {
          int const res = posix::pthread_mutex_destroy(&m);
          autoboost::ignore_unused(res);
          AUTOBOOST_ASSERT(!res);
        }

        void lock()
        {
            int res = posix::pthread_mutex_lock(&m);
            if (res)
            {
                autoboost::throw_exception(lock_error(res,"autoboost: mutex lock failed in pthread_mutex_lock"));
            }
        }

        void unlock()
        {
            int res = posix::pthread_mutex_unlock(&m);
            (void)res;
            AUTOBOOST_ASSERT(res == 0);
//            if (res)
//            {
//                autoboost::throw_exception(lock_error(res,"autoboost: mutex unlock failed in pthread_mutex_unlock"));
//            }
        }

        bool try_lock()
        {
            int res;
            do
            {
                res = pthread_mutex_trylock(&m);
            } while (res == EINTR);
            if (res==EBUSY)
            {
                return false;
            }

            return !res;
        }

#define AUTOBOOST_THREAD_DEFINES_MUTEX_NATIVE_HANDLE
        typedef pthread_mutex_t* native_handle_type;
        native_handle_type native_handle()
        {
            return &m;
        }

#if defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
        typedef unique_lock<mutex> scoped_lock;
        typedef detail::try_lock_wrapper<mutex> scoped_try_lock;
#endif
    };

    typedef mutex try_mutex;

    class timed_mutex
    {
    private:
        pthread_mutex_t m;
#ifndef AUTOBOOST_PTHREAD_HAS_TIMEDLOCK
        pthread_cond_t cond;
        bool is_locked;
#endif
    public:
        AUTOBOOST_THREAD_NO_COPYABLE(timed_mutex)
        timed_mutex()
        {
            int const res=pthread_mutex_init(&m,NULL);
            if(res)
            {
                autoboost::throw_exception(thread_resource_error(res, "autoboost:: timed_mutex constructor failed in pthread_mutex_init"));
            }
#ifndef AUTOBOOST_PTHREAD_HAS_TIMEDLOCK
            int const res2=pthread_cond_init(&cond,NULL);
            if(res2)
            {
                AUTOBOOST_VERIFY(!posix::pthread_mutex_destroy(&m));
                //AUTOBOOST_VERIFY(!pthread_mutex_destroy(&m));
                autoboost::throw_exception(thread_resource_error(res2, "autoboost:: timed_mutex constructor failed in pthread_cond_init"));
            }
            is_locked=false;
#endif
        }
        ~timed_mutex()
        {
            AUTOBOOST_VERIFY(!posix::pthread_mutex_destroy(&m));
#ifndef AUTOBOOST_PTHREAD_HAS_TIMEDLOCK
            AUTOBOOST_VERIFY(!pthread_cond_destroy(&cond));
#endif
        }

#if defined AUTOBOOST_THREAD_USES_DATETIME
        template<typename TimeDuration>
        bool timed_lock(TimeDuration const & relative_time)
        {
            return timed_lock(get_system_time()+relative_time);
        }
        bool timed_lock(autoboost::xtime const & absolute_time)
        {
            return timed_lock(system_time(absolute_time));
        }
#endif
#ifdef AUTOBOOST_PTHREAD_HAS_TIMEDLOCK
        void lock()
        {
            int res = posix::pthread_mutex_lock(&m);
            if (res)
            {
                autoboost::throw_exception(lock_error(res,"autoboost: mutex lock failed in pthread_mutex_lock"));
            }
        }

        void unlock()
        {
            int res = posix::pthread_mutex_unlock(&m);
            (void)res;
            AUTOBOOST_ASSERT(res == 0);
//            if (res)
//            {
//                autoboost::throw_exception(lock_error(res,"autoboost: mutex unlock failed in pthread_mutex_unlock"));
//            }
        }

        bool try_lock()
        {
          int res;
          do
          {
              res = pthread_mutex_trylock(&m);
          } while (res == EINTR);
          if (res==EBUSY)
          {
              return false;
          }

          return !res;
        }


    private:
        bool do_try_lock_until(struct timespec const &timeout)
        {
          int const res=pthread_mutex_timedlock(&m,&timeout);
          AUTOBOOST_ASSERT(!res || res==ETIMEDOUT);
          return !res;
        }
    public:

#else
        void lock()
        {
            autoboost::pthread::pthread_mutex_scoped_lock const local_lock(&m);
            while(is_locked)
            {
                AUTOBOOST_VERIFY(!pthread_cond_wait(&cond,&m));
            }
            is_locked=true;
        }

        void unlock()
        {
            autoboost::pthread::pthread_mutex_scoped_lock const local_lock(&m);
            is_locked=false;
            AUTOBOOST_VERIFY(!pthread_cond_signal(&cond));
        }

        bool try_lock()
        {
            autoboost::pthread::pthread_mutex_scoped_lock const local_lock(&m);
            if(is_locked)
            {
                return false;
            }
            is_locked=true;
            return true;
        }

    private:
        bool do_try_lock_until(struct timespec const &timeout)
        {
            autoboost::pthread::pthread_mutex_scoped_lock const local_lock(&m);
            while(is_locked)
            {
                int const cond_res=pthread_cond_timedwait(&cond,&m,&timeout);
                if(cond_res==ETIMEDOUT)
                {
                    return false;
                }
                AUTOBOOST_ASSERT(!cond_res);
            }
            is_locked=true;
            return true;
        }
    public:
#endif

#if defined AUTOBOOST_THREAD_USES_DATETIME
        bool timed_lock(system_time const & abs_time)
        {
            struct timespec const ts=autoboost::detail::to_timespec(abs_time);
            return do_try_lock_until(ts);
        }
#endif
#ifdef AUTOBOOST_THREAD_USES_CHRONO
        template <class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time)
        {
          return try_lock_until(chrono::steady_clock::now() + rel_time);
        }
        template <class Clock, class Duration>
        bool try_lock_until(const chrono::time_point<Clock, Duration>& t)
        {
          using namespace chrono;
          system_clock::time_point     s_now = system_clock::now();
          typename Clock::time_point  c_now = Clock::now();
          return try_lock_until(s_now + ceil<nanoseconds>(t - c_now));
        }
        template <class Duration>
        bool try_lock_until(const chrono::time_point<chrono::system_clock, Duration>& t)
        {
          using namespace chrono;
          typedef time_point<system_clock, nanoseconds> nano_sys_tmpt;
          return try_lock_until(nano_sys_tmpt(ceil<nanoseconds>(t.time_since_epoch())));
        }
        bool try_lock_until(const chrono::time_point<chrono::system_clock, chrono::nanoseconds>& tp)
        {
          //using namespace chrono;
          chrono::nanoseconds d = tp.time_since_epoch();
          timespec ts = autoboost::detail::to_timespec(d);
          return do_try_lock_until(ts);
        }
#endif

#define AUTOBOOST_THREAD_DEFINES_TIMED_MUTEX_NATIVE_HANDLE
        typedef pthread_mutex_t* native_handle_type;
        native_handle_type native_handle()
        {
            return &m;
        }

#if defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
        typedef unique_lock<timed_mutex> scoped_timed_lock;
        typedef detail::try_lock_wrapper<timed_mutex> scoped_try_lock;
        typedef scoped_timed_lock scoped_lock;
#endif
    };

}

#include <autoboost/config/abi_suffix.hpp>


#endif
