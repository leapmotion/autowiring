// Copyright (C) 2015 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/11 Vicente J. Botet Escriba
//    first implementation of a simple serial scheduler.

#ifndef AUTOBOOST_THREAD_SERIAL_EXECUTOR_CONT_HPP
#define AUTOBOOST_THREAD_SERIAL_EXECUTOR_CONT_HPP

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/delete.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/concurrent_queues/sync_queue.hpp>
#include <autoboost/thread/executors/work.hpp>
#include <autoboost/thread/executors/generic_executor_ref.hpp>
#include <autoboost/thread/future.hpp>
#include <autoboost/thread/scoped_thread.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{
namespace executors
{
  class serial_executor_cont
  {
  public:
    /// type-erasure to store the works to do
    typedef  executors::work work;
  private:

    generic_executor_ref ex_;
    future<void> fut_; // protected by mtx_
    bool closed_; // protected by mtx_
    mutex mtx_;

    struct continuation {
      work task;
      template <class X>
      struct result {
        typedef void type;
      };
      continuation(AUTOBOOST_THREAD_RV_REF(work) tsk)
      : task(autoboost::move(tsk)) {}
      void operator()(future<void> f)
      {
        try {
          task();
        } catch (...)  {
          std::terminate();
        }
      }
    };

    bool closed(lock_guard<mutex>&) const
    {
      return closed_;
    }
  public:
    /**
     * \par Returns
     * The underlying executor wrapped on a generic executor reference.
     */
    generic_executor_ref& underlying_executor() AUTOBOOST_NOEXCEPT { return ex_; }

    /// serial_executor_cont is not copyable.
    AUTOBOOST_THREAD_NO_COPYABLE(serial_executor_cont)

    /**
     * \b Effects: creates a serial executor that runs closures in fifo order using one the associated executor.
     *
     * \b Throws: Whatever exception is thrown while initializing the needed resources.
     *
     * \b Notes:
     * * The lifetime of the associated executor must outlive the serial executor.
     * * The current implementation doesn't support submission from synchronous continuation, that is,
     *     - the executor must execute the continuation asynchronously or
     *     - the continuation can not submit to this serial executor.
     */
    template <class Executor>
    serial_executor_cont(Executor& ex)
    : ex_(ex), fut_(make_ready_future()), closed_(false)
    {
    }
    /**
     * \b Effects: Destroys the thread pool.
     *
     * \b Synchronization: The completion of all the closures happen before the completion of the \c serial_executor_cont destructor.
     */
    ~serial_executor_cont()
    {
      // signal to the worker thread that there will be no more submissions.
      close();
    }

    /**
     * \b Effects: close the \c serial_executor_cont for submissions.
     * The loop will work until there is no more closures to run.
     */
    void close()
    {
      lock_guard<mutex> lk(mtx_);
      closed_ = true;;
    }

    /**
     * \b Returns: whether the pool is closed for submissions.
     */
    bool closed()
    {
      lock_guard<mutex> lk(mtx_);
      return closed(lk);
    }

    /**
     * Effects: none.
     * Returns: always false.
     * Throws: No.
     * Remark: A serial executor can not execute one of its pending tasks as the tasks depends on the other tasks.
     */
    bool try_executing_one()
    {
      return false;
    }

    /**
     * \b Requires: \c Closure is a model of \c Callable(void()) and a model of \c CopyConstructible/MoveConstructible.
     *
     * \b Effects: The specified \c closure will be scheduled for execution after the last submitted closure finish.
     * If the invoked closure throws an exception the \c serial_executor_cont will call \c std::terminate, as is the case with threads.
     *
     * \b Throws: \c sync_queue_is_closed if the executor is closed.
     * Whatever exception that can be throw while storing the closure.
     *
     */

#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
    template <typename Closure>
    void submit(Closure & closure)
    {
      lock_guard<mutex> lk(mtx_);
      if (closed(lk))       AUTOBOOST_THROW_EXCEPTION( sync_queue_is_closed() );
      fut_ = fut_.then(ex_, continuation(work(closure)));
    }
#endif
    void submit(void (*closure)())
    {
      lock_guard<mutex> lk(mtx_);
      if (closed(lk))       AUTOBOOST_THROW_EXCEPTION( sync_queue_is_closed() );
      fut_ = fut_.then(ex_, continuation(work(closure)));
    }

    template <typename Closure>
    void submit(AUTOBOOST_THREAD_FWD_REF(Closure) closure)
    {
      lock_guard<mutex> lk(mtx_);
      if (closed(lk))       AUTOBOOST_THROW_EXCEPTION( sync_queue_is_closed() );
      fut_ = fut_.then(ex_, continuation(work(autoboost::forward<Closure>(closure))));
    }

  };
}
using executors::serial_executor_cont;
}

#include <autoboost/config/abi_suffix.hpp>

#endif
