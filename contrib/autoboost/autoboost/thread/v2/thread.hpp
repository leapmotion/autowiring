// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2011 Vicente J. Botet Escriba

#ifndef AUTOBOOST_THREAD_V2_THREAD_HPP
#define AUTOBOOST_THREAD_V2_THREAD_HPP

#include <autoboost/thread/detail/config.hpp>
#ifdef AUTOBOOST_THREAD_USES_CHRONO
#include <autoboost/chrono/system_clocks.hpp>
#include <autoboost/chrono/ceil.hpp>
#endif
#include <autoboost/thread/condition_variable.hpp>
#include <autoboost/thread/lock_types.hpp>

namespace autoboost
{
  namespace this_thread
  {
    namespace no_interruption_point
    {
#ifdef AUTOBOOST_THREAD_USES_CHRONO

    template <class Clock, class Duration>
    void sleep_until(const chrono::time_point<Clock, Duration>& t)
    {
      using namespace chrono;
      mutex mut;
      condition_variable cv;
      unique_lock<mutex> lk(mut);
      while (Clock::now() < t)
        cv.wait_until(lk, t);
    }

#ifdef AUTOBOOST_THREAD_SLEEP_FOR_IS_STEADY

    template <class Rep, class Period>
    void sleep_for(const chrono::duration<Rep, Period>& d)
    {
      using namespace chrono;
      if (d > duration<Rep, Period>::zero())
      {
          duration<long double> Max = nanoseconds::max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION ();
          nanoseconds ns;
          if (d < Max)
          {
              ns = duration_cast<nanoseconds>(d);
              if (ns < d)
                  ++ns;
          }
          else
              ns = nanoseconds:: max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION ();
          sleep_for(ns);
      }
    }

    template <class Duration>
    inline AUTOBOOST_SYMBOL_VISIBLE
    void sleep_until(const chrono::time_point<chrono::steady_clock, Duration>& t)
    {
      using namespace chrono;
      sleep_for(t - steady_clock::now());
    }
#else
    template <class Rep, class Period>
    void sleep_for(const chrono::duration<Rep, Period>& d)
    {
      using namespace chrono;
      if (d > duration<Rep, Period>::zero())
      {
        steady_clock::time_point c_timeout = steady_clock::now() + ceil<nanoseconds>(d);
        sleep_until(c_timeout);
      }
    }

#endif

#endif
    }
#ifdef AUTOBOOST_THREAD_USES_CHRONO

    template <class Clock, class Duration>
    void sleep_until(const chrono::time_point<Clock, Duration>& t)
    {
      using namespace chrono;
      mutex mut;
      condition_variable cv;
      unique_lock<mutex> lk(mut);
      while (Clock::now() < t)
        cv.wait_until(lk, t);
    }

#if defined AUTOBOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC && defined AUTOBOOST_CHRONO_HAS_CLOCK_STEADY
    template <class Rep, class Period>
    void sleep_for(const chrono::duration<Rep, Period>& d)
    {
      using namespace chrono;
      if (d > duration<Rep, Period>::zero())
      {
        steady_clock::time_point c_timeout = steady_clock::now() + ceil<nanoseconds>(d);
        sleep_until(c_timeout);
      }
    }

#elif defined AUTOBOOST_THREAD_SLEEP_FOR_IS_STEADY

    template <class Rep, class Period>
    void sleep_for(const chrono::duration<Rep, Period>& d)
    {
      using namespace chrono;
      if (d > duration<Rep, Period>::zero())
      {
          duration<long double> Max = nanoseconds::max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION ();
          nanoseconds ns;
          if (d < Max)
          {
              ns = duration_cast<nanoseconds>(d);
              if (ns < d)
                  ++ns;
          }
          else
              ns = nanoseconds:: max AUTOBOOST_PREVENT_MACRO_SUBSTITUTION ();
          sleep_for(ns);
      }
    }

    template <class Duration>
    inline AUTOBOOST_SYMBOL_VISIBLE
    void sleep_until(const chrono::time_point<chrono::steady_clock, Duration>& t)
    {
      using namespace chrono;
      sleep_for(t - steady_clock::now());
    }
#else
    template <class Rep, class Period>
    void sleep_for(const chrono::duration<Rep, Period>& d)
    {
      using namespace chrono;
      if (d > duration<Rep, Period>::zero())
      {
        //system_clock::time_point c_timeout = time_point_cast<system_clock::duration>(system_clock::now() + ceil<nanoseconds>(d));
        system_clock::time_point c_timeout = system_clock::now() + ceil<system_clock::duration>(d);
        sleep_until(c_timeout);
      }
    }

#endif

#endif
  }
}


#endif
