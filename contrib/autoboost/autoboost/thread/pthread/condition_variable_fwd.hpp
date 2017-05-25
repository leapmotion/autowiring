#ifndef AUTOBOOST_THREAD_PTHREAD_CONDITION_VARIABLE_FWD_HPP
#define AUTOBOOST_THREAD_PTHREAD_CONDITION_VARIABLE_FWD_HPP
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2007-8 Anthony Williams
// (C) Copyright 2011-2012 Vicente J. Botet Escriba

#include <autoboost/assert.hpp>
#include <autoboost/throw_exception.hpp>
#include <pthread.h>
#include <autoboost/thread/cv_status.hpp>
#include <autoboost/thread/mutex.hpp>
#include <autoboost/thread/lock_types.hpp>
#include <autoboost/thread/thread_time.hpp>
#include <autoboost/thread/pthread/timespec.hpp>
#if defined AUTOBOOST_THREAD_USES_DATETIME
#include <autoboost/thread/xtime.hpp>
#endif

#ifdef AUTOBOOST_THREAD_USES_CHRONO
#include <autoboost/chrono/system_clocks.hpp>
#include <autoboost/chrono/ceil.hpp>
#endif
#include <autoboost/thread/detail/delete.hpp>
#include <autoboost/date_time/posix_time/posix_time_duration.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{
  namespace detail {
    inline int monotonic_pthread_cond_init(pthread_cond_t& cond) {

#ifdef AUTOBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC
            pthread_condattr_t attr;
            int res = pthread_condattr_init(&attr);
            if (res)
            {
              return res;
            }
            pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
            res=pthread_cond_init(&cond,&attr);
            pthread_condattr_destroy(&attr);
            return res;
#else
            return pthread_cond_init(&cond,NULL);
#endif

    }
  }

    class condition_variable
    {
    private:
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
        pthread_mutex_t internal_mutex;
#endif
        pthread_cond_t cond;

    public:
    //private: // used by autoboost::thread::try_join_until

        inline bool do_wait_until(
            unique_lock<mutex>& lock,
            struct timespec const &timeout);

        bool do_wait_for(
            unique_lock<mutex>& lock,
            struct timespec const &timeout)
        {
#if ! defined AUTOBOOST_THREAD_USEFIXES_TIMESPEC
            return do_wait_until(lock, autoboost::detail::timespec_plus(timeout, autoboost::detail::timespec_now()));
#elif ! defined AUTOBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC
            //using namespace chrono;
            //nanoseconds ns = chrono::system_clock::now().time_since_epoch();

            struct timespec ts = autoboost::detail::timespec_now_realtime();
            //ts.tv_sec = static_cast<long>(chrono::duration_cast<chrono::seconds>(ns).count());
            //ts.tv_nsec = static_cast<long>((ns - chrono::duration_cast<chrono::seconds>(ns)).count());
            return do_wait_until(lock, autoboost::detail::timespec_plus(timeout, ts));
#else
            // old behavior was fine for monotonic
            return do_wait_until(lock, autoboost::detail::timespec_plus(timeout, autoboost::detail::timespec_now_realtime()));
#endif
        }

    public:
      AUTOBOOST_THREAD_NO_COPYABLE(condition_variable)
        condition_variable()
        {
            int res;
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
            res=pthread_mutex_init(&internal_mutex,NULL);
            if(res)
            {
                autoboost::throw_exception(thread_resource_error(res, "autoboost::condition_variable::condition_variable() constructor failed in pthread_mutex_init"));
            }
#endif
            res = detail::monotonic_pthread_cond_init(cond);
            if (res)
            {
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                AUTOBOOST_VERIFY(!pthread_mutex_destroy(&internal_mutex));
#endif
                autoboost::throw_exception(thread_resource_error(res, "autoboost::condition_variable::condition_variable() constructor failed in detail::monotonic_pthread_cond_init"));
            }
        }
        ~condition_variable()
        {
            int ret;
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
            do {
              ret = pthread_mutex_destroy(&internal_mutex);
            } while (ret == EINTR);
            AUTOBOOST_ASSERT(!ret);
#endif
            do {
              ret = pthread_cond_destroy(&cond);
            } while (ret == EINTR);
            AUTOBOOST_ASSERT(!ret);
        }

        void wait(unique_lock<mutex>& m);

        template<typename predicate_type>
        void wait(unique_lock<mutex>& m,predicate_type pred)
        {
            while(!pred()) wait(m);
        }

#if defined AUTOBOOST_THREAD_USES_DATETIME
        inline bool timed_wait(
            unique_lock<mutex>& m,
            autoboost::system_time const& abs_time)
        {
#if defined AUTOBOOST_THREAD_WAIT_BUG
            struct timespec const timeout=detail::to_timespec(abs_time + AUTOBOOST_THREAD_WAIT_BUG);
            return do_wait_until(m, timeout);
#else
            struct timespec const timeout=detail::to_timespec(abs_time);
            return do_wait_until(m, timeout);
#endif
        }
        bool timed_wait(
            unique_lock<mutex>& m,
            xtime const& abs_time)
        {
            return timed_wait(m,system_time(abs_time));
        }

        template<typename duration_type>
        bool timed_wait(
            unique_lock<mutex>& m,
            duration_type const& wait_duration)
        {
            if (wait_duration.is_pos_infinity())
            {
                wait(m); // or do_wait(m,detail::timeout::sentinel());
                return true;
            }
            if (wait_duration.is_special())
            {
                return true;
            }
            return timed_wait(m,get_system_time()+wait_duration);
        }

        template<typename predicate_type>
        bool timed_wait(
            unique_lock<mutex>& m,
            autoboost::system_time const& abs_time,predicate_type pred)
        {
            while (!pred())
            {
                if(!timed_wait(m, abs_time))
                    return pred();
            }
            return true;
        }

        template<typename predicate_type>
        bool timed_wait(
            unique_lock<mutex>& m,
            xtime const& abs_time,predicate_type pred)
        {
            return timed_wait(m,system_time(abs_time),pred);
        }

        template<typename duration_type,typename predicate_type>
        bool timed_wait(
            unique_lock<mutex>& m,
            duration_type const& wait_duration,predicate_type pred)
        {
            if (wait_duration.is_pos_infinity())
            {
                while (!pred())
                {
                  wait(m); // or do_wait(m,detail::timeout::sentinel());
                }
                return true;
            }
            if (wait_duration.is_special())
            {
                return pred();
            }
            return timed_wait(m,get_system_time()+wait_duration,pred);
        }
#endif

#ifndef AUTOBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC

#ifdef AUTOBOOST_THREAD_USES_CHRONO

        template <class Duration>
        cv_status
        wait_until(
                unique_lock<mutex>& lock,
                const chrono::time_point<chrono::system_clock, Duration>& t)
        {
          using namespace chrono;
          typedef time_point<system_clock, nanoseconds> nano_sys_tmpt;
          wait_until(lock,
                        nano_sys_tmpt(ceil<nanoseconds>(t.time_since_epoch())));
          return system_clock::now() < t ? cv_status::no_timeout :
                                             cv_status::timeout;
        }

        template <class Clock, class Duration>
        cv_status
        wait_until(
                unique_lock<mutex>& lock,
                const chrono::time_point<Clock, Duration>& t)
        {
          using namespace chrono;
          system_clock::time_point     s_now = system_clock::now();
          typename Clock::time_point  c_now = Clock::now();
          wait_until(lock, s_now + ceil<nanoseconds>(t - c_now));
          return Clock::now() < t ? cv_status::no_timeout : cv_status::timeout;
        }



        template <class Rep, class Period>
        cv_status
        wait_for(
                unique_lock<mutex>& lock,
                const chrono::duration<Rep, Period>& d)
        {
          using namespace chrono;
          system_clock::time_point s_now = system_clock::now();
          steady_clock::time_point c_now = steady_clock::now();
          wait_until(lock, s_now + ceil<nanoseconds>(d));
          return steady_clock::now() - c_now < d ? cv_status::no_timeout :
                                                   cv_status::timeout;

        }

        inline cv_status wait_until(
            unique_lock<mutex>& lk,
            chrono::time_point<chrono::system_clock, chrono::nanoseconds> tp)
        {
            using namespace chrono;
            nanoseconds d = tp.time_since_epoch();
            timespec ts = autoboost::detail::to_timespec(d);
            if (do_wait_until(lk, ts)) return cv_status::no_timeout;
            else return cv_status::timeout;
        }
#endif

#else // defined AUTOBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC
#ifdef AUTOBOOST_THREAD_USES_CHRONO

        template <class Duration>
        cv_status
        wait_until(
              unique_lock<mutex>& lock,
              const chrono::time_point<chrono::steady_clock, Duration>& t)
        {
            using namespace chrono;
            typedef time_point<steady_clock, nanoseconds> nano_sys_tmpt;
            wait_until(lock,
                        nano_sys_tmpt(ceil<nanoseconds>(t.time_since_epoch())));
            return steady_clock::now() < t ? cv_status::no_timeout :
                                             cv_status::timeout;
        }

        template <class Clock, class Duration>
        cv_status
        wait_until(
            unique_lock<mutex>& lock,
            const chrono::time_point<Clock, Duration>& t)
        {
            using namespace chrono;
            steady_clock::time_point     s_now = steady_clock::now();
            typename Clock::time_point  c_now = Clock::now();
            wait_until(lock, s_now + ceil<nanoseconds>(t - c_now));
            return Clock::now() < t ? cv_status::no_timeout : cv_status::timeout;
        }

        template <class Rep, class Period>
        cv_status
        wait_for(
            unique_lock<mutex>& lock,
            const chrono::duration<Rep, Period>& d)
        {
            using namespace chrono;
            steady_clock::time_point c_now = steady_clock::now();
            wait_until(lock, c_now + ceil<nanoseconds>(d));
            return steady_clock::now() - c_now < d ? cv_status::no_timeout :
                                                   cv_status::timeout;
        }

        inline cv_status wait_until(
            unique_lock<mutex>& lk,
            chrono::time_point<chrono::steady_clock, chrono::nanoseconds> tp)
        {
            using namespace chrono;
            nanoseconds d = tp.time_since_epoch();
            timespec ts = autoboost::detail::to_timespec(d);
            if (do_wait_until(lk, ts)) return cv_status::no_timeout;
            else return cv_status::timeout;
        }
#endif

#endif // defined AUTOBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC

#ifdef AUTOBOOST_THREAD_USES_CHRONO
        template <class Clock, class Duration, class Predicate>
        bool
        wait_until(
                unique_lock<mutex>& lock,
                const chrono::time_point<Clock, Duration>& t,
                Predicate pred)
        {
            while (!pred())
            {
                if (wait_until(lock, t) == cv_status::timeout)
                    return pred();
            }
            return true;
        }

        template <class Rep, class Period, class Predicate>
        bool
        wait_for(
                unique_lock<mutex>& lock,
                const chrono::duration<Rep, Period>& d,
                Predicate pred)
        {
          return wait_until(lock, chrono::steady_clock::now() + d, autoboost::move(pred));
        }
#endif

#define AUTOBOOST_THREAD_DEFINES_CONDITION_VARIABLE_NATIVE_HANDLE
        typedef pthread_cond_t* native_handle_type;
        native_handle_type native_handle()
        {
            return &cond;
        }

        void notify_one() AUTOBOOST_NOEXCEPT;
        void notify_all() AUTOBOOST_NOEXCEPT;


    };

    AUTOBOOST_THREAD_DECL void notify_all_at_thread_exit(condition_variable& cond, unique_lock<mutex> lk);

}


#include <autoboost/config/abi_suffix.hpp>

#endif
