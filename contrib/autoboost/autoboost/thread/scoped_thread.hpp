// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2009-2012 Anthony Williams
// (C) Copyright 2012 Vicente J. Botet Escriba

// Based on the Anthony's idea of scoped_thread in CCiA

#ifndef AUTOBOOST_THREAD_SCOPED_THREAD_HPP
#define AUTOBOOST_THREAD_SCOPED_THREAD_HPP

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/delete.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/thread_functors.hpp>
#include <autoboost/thread/thread_only.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{

  /**
   * RAI @c thread wrapper adding a specific destroyer allowing to master what can be done at destruction time.
   *
   * CallableThread: A callable void(thread&) .
   * The default is a join_if_joinable.
   *
   * thread std/autoboost::thread destructor terminates the program if the thread is not joinable.
   * Having a wrapper that can join the thread before destroying it seems a natural need.
   *
   * Example:
   *
   *     autoboost::strict_scoped_thread<> t((autoboost::thread(F)));
   *
   */
  template <class CallableThread = join_if_joinable, class Thread=::autoboost::thread>
  class strict_scoped_thread
  {
    Thread t_;
    struct dummy;
  public:

    AUTOBOOST_THREAD_NO_COPYABLE( strict_scoped_thread) /// non copyable

    /*
     *
     */
#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template <class F, class ...Args, typename = typename disable_if<is_same<typename decay<F>::type, Thread>, void* >::type>
    explicit strict_scoped_thread(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Args)... args) :
      t_(autoboost::forward<F>(f), autoboost::forward<Args>(args)...) {}
#else
    template <class F>
    explicit strict_scoped_thread(AUTOBOOST_THREAD_FWD_REF(F) f,
        typename disable_if<is_same<typename decay<F>::type, Thread>, void* >::type=0) :
      t_(autoboost::forward<F>(f)) {}
    template <class F, class A1>
    strict_scoped_thread(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(A1) a1) :
      t_(autoboost::forward<F>(f), autoboost::forward<A1>(a1)) {}
    template <class F, class A1, class A2>
    strict_scoped_thread(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(A1) a1, AUTOBOOST_THREAD_FWD_REF(A2) a2) :
      t_(autoboost::forward<F>(f), autoboost::forward<A1>(a1), autoboost::forward<A2>(a2)) {}
    template <class F, class A1, class A2, class A3>
    strict_scoped_thread(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(A1) a1, AUTOBOOST_THREAD_FWD_REF(A2) a2, AUTOBOOST_THREAD_FWD_REF(A3) a3) :
      t_(autoboost::forward<F>(f), autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3)) {}
#endif

    /**
     * Constructor from the thread to own.
     *
     * @param t: the thread to own.
     *
     * Effects: move the thread to own @c t.
     */
    explicit strict_scoped_thread(AUTOBOOST_THREAD_RV_REF(Thread) t) AUTOBOOST_NOEXCEPT :
    t_(autoboost::move(t))
    {
    }

    /**
     * Destructor
     * Effects: Call the CallableThread functor before destroying the owned thread.
     * Remark: The CallableThread should not throw when joining the thread as the scoped variable is on a scope outside the thread function.
     */
    ~strict_scoped_thread()
    {
      CallableThread on_destructor;

      on_destructor(t_);
    }

  };

  /**
   * RAI @c thread wrapper adding a specific destroyer allowing to master what can be done at destruction time.
   *
   * CallableThread: A callable void(thread&) .
   * The default is join_if_joinable.
   *
   * thread std::thread destructor terminates the program if the thread is not joinable.
   * Having a wrapper that can join the thread before destroying it seems a natural need.
   *
   * Remark: @c scoped_thread is not a @c thread as @c thread is not designed to be derived from as a polymorphic type.
   * Anyway @c scoped_thread can be used in most of the contexts a @c thread could be used as it has the
   * same non-deprecated interface with the exception of the construction.
   *
   * Example:
   *
   *     autoboost::scoped_thread<> t((autoboost::thread(F)));
   *     t.interrupt();
   *
   */
  template <class CallableThread = join_if_joinable, class Thread=::autoboost::thread>
  class scoped_thread
  {
    Thread t_;
    struct dummy;
  public:

    typedef typename Thread::id id;
    typedef typename Thread::native_handle_type native_handle_type;

    AUTOBOOST_THREAD_MOVABLE_ONLY( scoped_thread) /// Movable only

    /**
     * Default Constructor.
     *
     * Effects: wraps a not-a-thread.
     */
    scoped_thread() AUTOBOOST_NOEXCEPT:
    t_()
    {
    }

    /**
     *
     */

#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template <class F, class ...Args, typename = typename disable_if<is_same<typename decay<F>::type, Thread>, void* >::type>
    explicit scoped_thread(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Args)... args) :
      t_(autoboost::forward<F>(f), autoboost::forward<Args>(args)...) {}
#else
    template <class F>
    explicit scoped_thread(AUTOBOOST_THREAD_FWD_REF(F) f,
        typename disable_if<is_same<typename decay<F>::type, Thread>, void* >::type=0) :
      t_(autoboost::forward<F>(f)) {}
    template <class F, class A1>
    scoped_thread(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(A1) a1) :
      t_(autoboost::forward<F>(f), autoboost::forward<A1>(a1)) {}
    template <class F, class A1, class A2>
    scoped_thread(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(A1) a1, AUTOBOOST_THREAD_FWD_REF(A2) a2) :
      t_(autoboost::forward<F>(f), autoboost::forward<A1>(a1), autoboost::forward<A2>(a2)) {}
    template <class F, class A1, class A2, class A3>
    scoped_thread(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(A1) a1, AUTOBOOST_THREAD_FWD_REF(A2) a2, AUTOBOOST_THREAD_FWD_REF(A3) a3) :
      t_(autoboost::forward<F>(f), autoboost::forward<A1>(a1), autoboost::forward<A2>(a2), autoboost::forward<A3>(a3)) {}

#endif
    /**
     * Constructor from the thread to own.
     *
     * @param t: the thread to own.
     *
     * Effects: move the thread to own @c t.
     */
    explicit scoped_thread(AUTOBOOST_THREAD_RV_REF(Thread) t) AUTOBOOST_NOEXCEPT :
    t_(autoboost::move(t))
    {
    }

//    explicit operator Thread()
//    {
//      return autoboost::move(t_);
//    }

    /**
     * Move constructor.
     */
    scoped_thread(AUTOBOOST_RV_REF(scoped_thread) x) AUTOBOOST_NOEXCEPT :
    t_(autoboost::move(AUTOBOOST_THREAD_RV(x).t_))
    {}

    /**
     * Destructor
     *
     * Effects: Call the CallableThread functor before destroying the owned thread.
     */
    ~scoped_thread()
    {
      CallableThread on_destructor;

      on_destructor(t_);
    }

    /**
     * Move assignment.
     */
    scoped_thread& operator=(AUTOBOOST_RV_REF(scoped_thread) x)
    {
      CallableThread on_destructor;

      on_destructor(t_);
      t_ = autoboost::move(AUTOBOOST_THREAD_RV(x).t_);
      return *this;
    }

    /**
     *
     */
    void swap(scoped_thread& x) AUTOBOOST_NOEXCEPT
    {
      t_.swap(x.t_);
    }

    // forwarded thread functions
    inline id get_id() const AUTOBOOST_NOEXCEPT
    {
      return t_.get_id();
    }

    void detach()
    {
      t_.detach();
    }

    void join()
    {
      t_.join();
    }

#ifdef AUTOBOOST_THREAD_USES_CHRONO
    template <class Rep, class Period>
    bool try_join_for(const chrono::duration<Rep, Period>& rel_time)
    {
      return t_.try_join_for(rel_time);
    }

    template <class Clock, class Duration>
    bool try_join_until(const chrono::time_point<Clock, Duration>& abs_time)
    {
      return t_.try_join_until(abs_time);
    }
#endif

    native_handle_type native_handle()AUTOBOOST_NOEXCEPT
    {
      return t_.native_handle();
    }

    bool joinable() const AUTOBOOST_NOEXCEPT
    {
      return t_.joinable();
    }

#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
    void interrupt()
    {
      t_.interrupt();
    }

    bool interruption_requested() const AUTOBOOST_NOEXCEPT
    {
      return t_.interruption_requested();
    }
#endif

    static unsigned hardware_concurrency() AUTOBOOST_NOEXCEPT
    {
      return Thread::hardware_concurrency();
    }

#ifdef AUTOBOOST_THREAD_PROVIDES_PHYSICAL_CONCURRENCY
    static unsigned physical_concurrency() AUTOBOOST_NOEXCEPT
    {
      return Thread::physical_concurrency();
    }
#endif
  };

  /**
   * Effects: swaps the contents of two scoped threads.
   */
  template <class Destroyer, class Thread >
  void swap(scoped_thread<Destroyer, Thread>& lhs, scoped_thread<Destroyer, Thread>& rhs)
AUTOBOOST_NOEXCEPT {
  return lhs.swap(rhs);
}

  typedef scoped_thread<> joining_thread;
}
#include <autoboost/config/abi_suffix.hpp>

#endif
