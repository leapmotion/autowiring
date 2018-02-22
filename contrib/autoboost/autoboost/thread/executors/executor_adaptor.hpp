// Copyright (C) 2013,2014 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/09 Vicente J. Botet Escriba
//    Adapt to autoboost from CCIA C++11 implementation

#ifndef AUTOBOOST_THREAD_EXECUTORS_EXECUTOR_ADAPTOR_HPP
#define AUTOBOOST_THREAD_EXECUTORS_EXECUTOR_ADAPTOR_HPP

#include <autoboost/thread/detail/config.hpp>

#include <autoboost/thread/executors/executor.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{
namespace executors
{
  /**
   * Polymorphic adaptor of a model of Executor to an executor.
   */
  template <typename Executor>
  class executor_adaptor : public executor
  {
    Executor ex;
  public:
    /// type-erasure to store the works to do
    typedef  executor::work work;

    /// executor is not copyable.
    AUTOBOOST_THREAD_NO_COPYABLE(executor_adaptor)

    /**
     * executor_adaptor constructor
     */
#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template <typename ...Args>
    executor_adaptor(AUTOBOOST_THREAD_RV_REF(Args) ... args) : ex(autoboost::forward<Args>(args)...) {}
#else
    /**
     * executor_adaptor constructor
     */
    executor_adaptor() : ex() {}

    template <typename A1>
    executor_adaptor(
        AUTOBOOST_THREAD_FWD_REF(A1) a1
        ) :
      ex(
          autoboost::forward<A1>(a1)
          ) {}
    template <typename A1, typename A2>
    executor_adaptor(
        AUTOBOOST_THREAD_FWD_REF(A1) a1,
        AUTOBOOST_THREAD_FWD_REF(A2) a2
        ) :
      ex(
          autoboost::forward<A1>(a1),
          autoboost::forward<A2>(a2)
          ) {}
    template <typename A1, typename A2, typename A3>
    executor_adaptor(
        AUTOBOOST_THREAD_FWD_REF(A1) a1,
        AUTOBOOST_THREAD_FWD_REF(A2) a2,
        AUTOBOOST_THREAD_FWD_REF(A3) a3
        ) :
      ex(
          autoboost::forward<A1>(a1),
          autoboost::forward<A2>(a2),
          autoboost::forward<A3>(a3)
          ) {}
#endif
    Executor& underlying_executor() { return ex; }

    /**
     * \b Effects: close the \c executor for submissions.
     * The worker threads will work until there is no more closures to run.
     */
    void close() { ex.close(); }

    /**
     * \b Returns: whether the pool is closed for submissions.
     */
    bool closed() { return ex.closed(); }

    /**
     * \b Effects: The specified closure will be scheduled for execution at some point in the future.
     * If invoked closure throws an exception the executor will call std::terminate, as is the case with threads.
     *
     * \b Synchronization: completion of closure on a particular thread happens before destruction of thread's thread local variables.
     *
     * \b Throws: \c sync_queue_is_closed if the thread pool is closed.
     * Whatever exception that can be throw while storing the closure.
     */
    void submit(AUTOBOOST_THREAD_RV_REF(work) closure)  {
      return ex.submit(autoboost::move(closure));
    }

#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
    template <typename Closure>
    void submit(Closure & closure)
    {
      submit(work(closure));
    }
#endif
    void submit(void (*closure)())
    {
      submit(work(closure));
    }

    template <typename Closure>
    void submit(AUTOBOOST_THREAD_FWD_REF(Closure) closure)
    {
      //submit(work(autoboost::forward<Closure>(closure)));
      work w((autoboost::forward<Closure>(closure)));
      submit(autoboost::move(w));
    }

    /**
     * Effects: try to execute one task.
     * Returns: whether a task has been executed.
     * Throws: whatever the current task constructor throws or the task() throws.
     */
    bool try_executing_one() { return ex.try_executing_one(); }

  };
}
using executors::executor_adaptor;
}

#include <autoboost/config/abi_suffix.hpp>

#endif
