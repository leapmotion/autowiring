//  (C) Copyright 2008-10 Anthony Williams
//  (C) Copyright 2011-2014 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef AUTOBOOST_THREAD_FUTURE_HPP
#define AUTOBOOST_THREAD_FUTURE_HPP

#include <autoboost/thread/detail/config.hpp>

// autoboost::thread::future requires exception handling
// due to autoboost::exception::exception_ptr dependency

#ifndef AUTOBOOST_NO_EXCEPTIONS

#include <autoboost/core/scoped_enum.hpp>
#include <stdexcept>
#include <iostream>
#include <autoboost/thread/exceptional_ptr.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/detail/invoker.hpp>
#include <autoboost/thread/detail/invoke.hpp>
#include <autoboost/thread/thread_time.hpp>
#include <autoboost/thread/mutex.hpp>
#include <autoboost/thread/condition_variable.hpp>
#include <autoboost/thread/lock_algorithms.hpp>
#include <autoboost/thread/lock_types.hpp>
#include <autoboost/exception_ptr.hpp>
#include <autoboost/shared_ptr.hpp>
#if defined AUTOBOOST_THREAD_FUTURE_USES_OPTIONAL
#include <autoboost/optional.hpp>
#else
#include <autoboost/thread/csbl/memory/unique_ptr.hpp>
//#include <autoboost/move/make_unique.hpp>
#endif
#include <autoboost/type_traits/is_fundamental.hpp>
#include <autoboost/thread/detail/is_convertible.hpp>
#include <autoboost/type_traits/decay.hpp>
#include <autoboost/type_traits/is_void.hpp>
#include <autoboost/type_traits/conditional.hpp>
#include <autoboost/config.hpp>
#include <autoboost/throw_exception.hpp>
#include <algorithm>
#include <autoboost/function.hpp>
#include <autoboost/bind.hpp>
#include <autoboost/core/ref.hpp>
#include <autoboost/scoped_array.hpp>
#include <autoboost/enable_shared_from_this.hpp>
#include <autoboost/core/enable_if.hpp>

#include <list>
#include <autoboost/next_prior.hpp>
#include <vector>

#include <autoboost/thread/future_error_code.hpp>
#ifdef AUTOBOOST_THREAD_USES_CHRONO
#include <autoboost/chrono/system_clocks.hpp>
#endif

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CTOR_ALLOCATORS
#include <autoboost/thread/detail/memory.hpp>
#include <autoboost/container/scoped_allocator.hpp>
#if ! defined  AUTOBOOST_NO_CXX11_ALLOCATOR
#include <memory>
#endif
#endif

#include <autoboost/utility/result_of.hpp>
#include <autoboost/thread/thread_only.hpp>

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
#include <autoboost/thread/csbl/tuple.hpp>
#include <autoboost/thread/csbl/vector.hpp>
#endif

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE
#define AUTOBOOST_THREAD_FUTURE future
#else
#define AUTOBOOST_THREAD_FUTURE unique_future
#endif

namespace autoboost
{

  //enum class launch
  AUTOBOOST_SCOPED_ENUM_DECLARE_BEGIN(launch)
  {
      none = 0,
      async = 1,
      deferred = 2,
#ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS
      executor = 4,
#endif
      any = async | deferred
  }
  AUTOBOOST_SCOPED_ENUM_DECLARE_END(launch)

  //enum class future_status
  AUTOBOOST_SCOPED_ENUM_DECLARE_BEGIN(future_status)
  {
      ready,
      timeout,
      deferred
  }
  AUTOBOOST_SCOPED_ENUM_DECLARE_END(future_status)

  class AUTOBOOST_SYMBOL_VISIBLE future_error
      : public std::logic_error
  {
      system::error_code ec_;
  public:
      future_error(system::error_code ec)
      : logic_error(ec.message()),
        ec_(ec)
      {
      }

      const system::error_code& code() const AUTOBOOST_NOEXCEPT
      {
        return ec_;
      }
  };

    class AUTOBOOST_SYMBOL_VISIBLE future_uninitialized:
        public future_error
    {
    public:
        future_uninitialized() :
          future_error(system::make_error_code(future_errc::no_state))
        {}
    };
    class AUTOBOOST_SYMBOL_VISIBLE broken_promise:
        public future_error
    {
    public:
        broken_promise():
          future_error(system::make_error_code(future_errc::broken_promise))
        {}
    };
    class AUTOBOOST_SYMBOL_VISIBLE future_already_retrieved:
        public future_error
    {
    public:
        future_already_retrieved():
          future_error(system::make_error_code(future_errc::future_already_retrieved))
        {}
    };
    class AUTOBOOST_SYMBOL_VISIBLE promise_already_satisfied:
        public future_error
    {
    public:
        promise_already_satisfied():
          future_error(system::make_error_code(future_errc::promise_already_satisfied))
        {}
    };

    class AUTOBOOST_SYMBOL_VISIBLE task_already_started:
        public future_error
    {
    public:
        task_already_started():
        future_error(system::make_error_code(future_errc::promise_already_satisfied))
        {}
    };

    class AUTOBOOST_SYMBOL_VISIBLE task_moved:
        public future_error
    {
    public:
        task_moved():
          future_error(system::make_error_code(future_errc::no_state))
        {}
    };

    class promise_moved:
        public future_error
    {
    public:
          promise_moved():
          future_error(system::make_error_code(future_errc::no_state))
        {}
    };

    namespace future_state
    {
        enum state { uninitialized, waiting, ready, moved, deferred };
    }

    namespace detail
    {
        struct relocker
        {
            autoboost::unique_lock<autoboost::mutex>& lock_;
            bool  unlocked_;

            relocker(autoboost::unique_lock<autoboost::mutex>& lk):
                lock_(lk)
            {
                lock_.unlock();
                unlocked_=true;
            }
            ~relocker()
            {
              if (unlocked_) {
                lock_.lock();
              }
            }
            void lock() {
              if (unlocked_) {
                lock_.lock();
                unlocked_=false;
              }
            }
        private:
            relocker& operator=(relocker const&);
        };

        struct shared_state_base : enable_shared_from_this<shared_state_base>
        {
            typedef std::list<autoboost::condition_variable_any*> waiter_list;
            // This type should be only included conditionally if interruptions are allowed, but is included to maintain the same layout.
            typedef shared_ptr<shared_state_base> continuation_ptr_type;

            autoboost::exception_ptr exception;
            bool done;
            bool is_deferred_;
            launch policy_;
            bool is_constructed;
            mutable autoboost::mutex mutex;
            autoboost::condition_variable waiters;
            waiter_list external_waiters;
            autoboost::function<void()> callback;
            // This declaration should be only included conditionally, but is included to maintain the same layout.
            continuation_ptr_type continuation_ptr;

            // This declaration should be only included conditionally, but is included to maintain the same layout.
            virtual void launch_continuation(autoboost::unique_lock<autoboost::mutex>&)
            {
            }

            shared_state_base():
                done(false),
                is_deferred_(false),
                policy_(launch::none),
                is_constructed(false),
                continuation_ptr()
            {}
            virtual ~shared_state_base()
            {}

            void set_deferred()
            {
              is_deferred_ = true;
              policy_ = launch::deferred;
            }
            void set_async()
            {
              is_deferred_ = false;
              policy_ = launch::async;
            }
#ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS
            void set_executor()
            {
              is_deferred_ = false;
              policy_ = launch::executor;
            }
#endif
            waiter_list::iterator register_external_waiter(autoboost::condition_variable_any& cv)
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                do_callback(lock);
                return external_waiters.insert(external_waiters.end(),&cv);
            }

            void remove_external_waiter(waiter_list::iterator it)
            {
                autoboost::lock_guard<autoboost::mutex> lock(this->mutex);
                external_waiters.erase(it);
            }

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
            void do_continuation(autoboost::unique_lock<autoboost::mutex>& lock)
            {
                if (continuation_ptr) {
                  continuation_ptr_type this_continuation_ptr = continuation_ptr;
                  continuation_ptr.reset();
                  this_continuation_ptr->launch_continuation(lock);
                }
            }
#else
            void do_continuation(autoboost::unique_lock<autoboost::mutex>&)
            {
            }
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
            void set_continuation_ptr(continuation_ptr_type continuation, autoboost::unique_lock<autoboost::mutex>& lock)
            {
              continuation_ptr= continuation;
              if (done) {
                do_continuation(lock);
              }
            }
#endif
            void mark_finished_internal(autoboost::unique_lock<autoboost::mutex>& lock)
            {
                done=true;
                waiters.notify_all();
                for(waiter_list::const_iterator it=external_waiters.begin(),
                        end=external_waiters.end();it!=end;++it)
                {
                    (*it)->notify_all();
                }
                do_continuation(lock);
            }
            void make_ready()
            {
              autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
              mark_finished_internal(lock);
            }

            void do_callback(autoboost::unique_lock<autoboost::mutex>& lock)
            {
                if(callback && !done)
                {
                    autoboost::function<void()> local_callback=callback;
                    relocker relock(lock);
                    local_callback();
                }
            }

            virtual bool run_if_is_deferred()
            {
              autoboost::unique_lock<autoboost::mutex> lk(this->mutex);
              if (is_deferred_)
              {
                is_deferred_=false;
                execute(lk);
                return true;
              }
              else
                return false;
            }
            virtual bool run_if_is_deferred_or_ready()
            {
              autoboost::unique_lock<autoboost::mutex> lk(this->mutex);
              if (is_deferred_)
              {
                is_deferred_=false;
                execute(lk);

                return true;
              }
              else
                return done;
            }
            void wait_internal(autoboost::unique_lock<autoboost::mutex> &lk, bool rethrow=true)
            {
              do_callback(lk);
              if (is_deferred_)
              {
                is_deferred_=false;
                execute(lk);
              }
              while(!done)
              {
                  waiters.wait(lk);
              }
              if(rethrow && exception)
              {
                  autoboost::rethrow_exception(exception);
              }
            }

            virtual void wait(bool rethrow=true)
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                wait_internal(lock, rethrow);
            }

#if defined AUTOBOOST_THREAD_USES_DATETIME
            bool timed_wait_until(autoboost::system_time const& target_time)
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                if (is_deferred_)
                    return false;

                do_callback(lock);
                while(!done)
                {
                    bool const success=waiters.timed_wait(lock,target_time);
                    if(!success && !done)
                    {
                        return false;
                    }
                }
                return true;
            }
#endif
#ifdef AUTOBOOST_THREAD_USES_CHRONO

            template <class Clock, class Duration>
            future_status
            wait_until(const chrono::time_point<Clock, Duration>& abs_time)
            {
              autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
              if (is_deferred_)
                  return future_status::deferred;
              do_callback(lock);
              while(!done)
              {
                  cv_status const st=waiters.wait_until(lock,abs_time);
                  if(st==cv_status::timeout && !done)
                  {
                    return future_status::timeout;
                  }
              }
              return future_status::ready;
            }
#endif
            void mark_exceptional_finish_internal(autoboost::exception_ptr const& e, autoboost::unique_lock<autoboost::mutex>& lock)
            {
                exception=e;
                mark_finished_internal(lock);
            }

            void mark_exceptional_finish()
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                mark_exceptional_finish_internal(autoboost::current_exception(), lock);
            }

#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
            void mark_interrupted_finish()
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                exception = autoboost::copy_exception(autoboost::thread_interrupted());
                mark_finished_internal(lock);
            }

            void set_interrupted_autoboostat_thread_exit()
            {
              unique_lock<autoboost::mutex> lk(this->mutex);
              if (has_value(lk))
              {
                  throw_exception(promise_already_satisfied());
              }
              exception = autoboost::copy_exception(autoboost::thread_interrupted());
              this->is_constructed = true;
              detail::make_ready_autoboostat_thread_exit(shared_from_this());
            }
#endif

            void set_exception_autoboostat_thread_exit(exception_ptr e)
            {
              unique_lock<autoboost::mutex> lk(this->mutex);
              if (has_value(lk))
              {
                  throw_exception(promise_already_satisfied());
              }
              exception=e;
              this->is_constructed = true;
              detail::make_ready_autoboostat_thread_exit(shared_from_this());
            }

            bool has_value() const
            {
                autoboost::lock_guard<autoboost::mutex> lock(this->mutex);
                return done && ! exception;
            }

            bool has_value(unique_lock<autoboost::mutex>& )  const
            {
                return done && ! exception;
            }

            bool has_exception()  const
            {
                autoboost::lock_guard<autoboost::mutex> lock(this->mutex);
                return done && exception;
            }

            bool has_exception(unique_lock<autoboost::mutex>&) const
            {
                return done && exception;
            }

            bool is_deferred(autoboost::lock_guard<autoboost::mutex>&)  const {
                return is_deferred_;
            }

            launch launch_policy(autoboost::unique_lock<autoboost::mutex>&) const
            {
                return policy_;
            }

            future_state::state get_state() const
            {
                autoboost::lock_guard<autoboost::mutex> guard(this->mutex);
                if(!done)
                {
                    return future_state::waiting;
                }
                else
                {
                    return future_state::ready;
                }
            }

            exception_ptr get_exception_ptr()
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                return get_exception_ptr(lock);
            }
            exception_ptr get_exception_ptr(autoboost::unique_lock<autoboost::mutex>& lock)
            {
                wait_internal(lock, false);
                return exception;
            }

            template<typename F,typename U>
            void set_wait_callback(F f,U* u)
            {
                autoboost::lock_guard<autoboost::mutex> lock(this->mutex);
                callback=autoboost::bind(f,autoboost::ref(*u));
            }

            virtual void execute(autoboost::unique_lock<autoboost::mutex>&) {}

        private:
            shared_state_base(shared_state_base const&);
            shared_state_base& operator=(shared_state_base const&);
        };

        // Used to create stand-alone futures
        template<typename T>
        struct shared_state:
            detail::shared_state_base
        {
#if defined AUTOBOOST_THREAD_FUTURE_USES_OPTIONAL
              typedef autoboost::optional<T> storage_type;
#else
              typedef autoboost::csbl::unique_ptr<T> storage_type;
#endif
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
            typedef T const& source_reference_type;
            typedef AUTOBOOST_THREAD_RV_REF(T) rvalue_source_type;
            typedef T move_dest_type;
#elif defined AUTOBOOST_THREAD_USES_MOVE
            typedef typename conditional<autoboost::is_fundamental<T>::value,T,T const&>::type source_reference_type;
            typedef AUTOBOOST_THREAD_RV_REF(T) rvalue_source_type;
            typedef T move_dest_type;
#else
            typedef T& source_reference_type;
            typedef typename conditional<autoboost::thread_detail::is_convertible<T&,AUTOBOOST_THREAD_RV_REF(T) >::value, AUTOBOOST_THREAD_RV_REF(T),T const&>::type rvalue_source_type;
            typedef typename conditional<autoboost::thread_detail::is_convertible<T&,AUTOBOOST_THREAD_RV_REF(T) >::value, AUTOBOOST_THREAD_RV_REF(T),T>::type move_dest_type;
#endif

            typedef const T& shared_future_get_result_type;

            storage_type result;

            shared_state():
                result()
            {}

            ~shared_state()
            {}

            void mark_finished_with_result_internal(source_reference_type result_, autoboost::unique_lock<autoboost::mutex>& lock)
            {
#if defined AUTOBOOST_THREAD_FUTURE_USES_OPTIONAL
                result = result_;
#else
                result.reset(new T(result_));
#endif
                this->mark_finished_internal(lock);
            }

            void mark_finished_with_result_internal(rvalue_source_type result_, autoboost::unique_lock<autoboost::mutex>& lock)
            {
#if ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#if defined AUTOBOOST_THREAD_FUTURE_USES_OPTIONAL
                result = autoboost::move(result_);
#else
                result.reset(new T(autoboost::move(result_)));
#endif
#else
#if defined AUTOBOOST_THREAD_FUTURE_USES_OPTIONAL
                result = autoboost::move(result_);
#else
                result.reset(new T(static_cast<rvalue_source_type>(result_)));
#endif
#endif
                this->mark_finished_internal(lock);
            }

            void mark_finished_with_result(source_reference_type result_)
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                this->mark_finished_with_result_internal(result_, lock);
            }

            void mark_finished_with_result(rvalue_source_type result_)
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);

#if ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
                mark_finished_with_result_internal(autoboost::move(result_), lock);
#else
                mark_finished_with_result_internal(static_cast<rvalue_source_type>(result_), lock);
#endif
            }

            storage_type& get_storage(autoboost::unique_lock<autoboost::mutex>& lock)
            {
                wait_internal(lock);
                return result;
            }
            virtual move_dest_type get()
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                return autoboost::move(*get_storage(lock));
            }

            virtual shared_future_get_result_type get_sh()
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                return *get_storage(lock);
            }

            void set_value_autoboostat_thread_exit(source_reference_type result_)
            {
              unique_lock<autoboost::mutex> lk(this->mutex);
              if (this->has_value(lk))
              {
                  throw_exception(promise_already_satisfied());
              }
#if defined AUTOBOOST_THREAD_FUTURE_USES_OPTIONAL
              result = result_;
#else
              result.reset(new T(result_));
#endif

              this->is_constructed = true;
              detail::make_ready_autoboostat_thread_exit(shared_from_this());
            }
            void set_value_autoboostat_thread_exit(rvalue_source_type result_)
            {
              unique_lock<autoboost::mutex> lk(this->mutex);
              if (this->has_value(lk))
                  throw_exception(promise_already_satisfied());

#if ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#if defined AUTOBOOST_THREAD_FUTURE_USES_OPTIONAL
                result = autoboost::move(result_);
#else
                result.reset(new T(autoboost::move(result_)));
#endif
#else
#if defined AUTOBOOST_THREAD_FUTURE_USES_OPTIONAL
                result = autoboost::move(result_);
#else
                result.reset(new T(static_cast<rvalue_source_type>(result_)));
#endif
#endif
              this->is_constructed = true;
              detail::make_ready_autoboostat_thread_exit(shared_from_this());
            }

        private:
            shared_state(shared_state const&);
            shared_state& operator=(shared_state const&);
        };

        template<typename T>
        struct shared_state<T&>:
            detail::shared_state_base
        {
            typedef T* storage_type;
            typedef T& source_reference_type;
            typedef T& move_dest_type;
            typedef T& shared_future_get_result_type;

            T* result;

            shared_state():
                result(0)
            {}

            ~shared_state()
            {
            }

            void mark_finished_with_result_internal(source_reference_type result_, autoboost::unique_lock<autoboost::mutex>& lock)
            {
                result= &result_;
                mark_finished_internal(lock);
            }

            void mark_finished_with_result(source_reference_type result_)
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                mark_finished_with_result_internal(result_, lock);
            }

            virtual T& get()
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                wait_internal(lock);
                return *result;
            }

            virtual T& get_sh()
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                wait_internal(lock);
                return *result;
            }

            void set_value_autoboostat_thread_exit(T& result_)
            {
              unique_lock<autoboost::mutex> lk(this->mutex);
              if (this->has_value(lk))
                  throw_exception(promise_already_satisfied());
              result= &result_;
              this->is_constructed = true;
              detail::make_ready_autoboostat_thread_exit(shared_from_this());
            }

        private:
            shared_state(shared_state const&);
            shared_state& operator=(shared_state const&);
        };

        template<>
        struct shared_state<void>:
            detail::shared_state_base
        {
            typedef void shared_future_get_result_type;
            typedef void move_dest_type;

            shared_state()
            {}

            void mark_finished_with_result_internal(autoboost::unique_lock<autoboost::mutex>& lock)
            {
                mark_finished_internal(lock);
            }

            void mark_finished_with_result()
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                mark_finished_with_result_internal(lock);
            }

            virtual void get()
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                this->wait_internal(lock);
            }

            virtual void get_sh()
            {
                autoboost::unique_lock<autoboost::mutex> lock(this->mutex);
                this->wait_internal(lock);
            }

            void set_value_autoboostat_thread_exit()
            {
              unique_lock<autoboost::mutex> lk(this->mutex);
              if (this->has_value(lk))
              {
                  throw_exception(promise_already_satisfied());
              }
              this->is_constructed = true;
              detail::make_ready_autoboostat_thread_exit(shared_from_this());
            }
        private:
            shared_state(shared_state const&);
            shared_state& operator=(shared_state const&);
        };

        /////////////////////////
        /// future_async_shared_state_base
        /////////////////////////
        template<typename Rp>
        struct future_async_shared_state_base: shared_state<Rp>
        {
          typedef shared_state<Rp> base_type;
        protected:
          autoboost::thread thr_;
          void join()
          {
              if (thr_.joinable()) thr_.join();
          }
        public:
          future_async_shared_state_base()
          {
            this->set_async();
          }

          ~future_async_shared_state_base()
          {
            join();
          }

          virtual void wait(bool rethrow)
          {
              join();
              this->base_type::wait(rethrow);
          }
        };

        /////////////////////////
        /// future_async_shared_state
        /////////////////////////
        template<typename Rp, typename Fp>
        struct future_async_shared_state: future_async_shared_state_base<Rp>
        {
          explicit future_async_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f)
          {
            this->thr_ = thread(&future_async_shared_state::run, this, autoboost::forward<Fp>(f));
          }

          static void run(future_async_shared_state* that, AUTOBOOST_THREAD_FWD_REF(Fp) f)
          {
            try
            {
              that->mark_finished_with_result(f());
            }
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
            catch(thread_interrupted& )
            {
              that->mark_interrupted_finish();
            }
#endif
            catch(...)
            {
              that->mark_exceptional_finish();
            }
          }
        };

        template<typename Fp>
        struct future_async_shared_state<void, Fp>: public future_async_shared_state_base<void>
        {
          explicit future_async_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f)
          {
            this->thr_ = thread(&future_async_shared_state::run, this, autoboost::move(f));
          }

          static void run(future_async_shared_state* that, AUTOBOOST_THREAD_FWD_REF(Fp) f)
          {
            try
            {
              f();
              that->mark_finished_with_result();
            }
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
            catch(thread_interrupted& )
            {
              that->mark_interrupted_finish();
            }
#endif
            catch(...)
            {
              that->mark_exceptional_finish();
            }
          }
        };

        template<typename Rp, typename Fp>
        struct future_async_shared_state<Rp&, Fp>: future_async_shared_state_base<Rp&>
        {
          explicit future_async_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f)
          {
            this->thr_ = thread(&future_async_shared_state::run, this, autoboost::move(f));
          }

          static void run(future_async_shared_state* that, AUTOBOOST_THREAD_FWD_REF(Fp) f)
          {
            try
            {
              that->mark_finished_with_result(f());
            }
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
            catch(thread_interrupted& )
            {
              that->mark_interrupted_finish();
            }
#endif
            catch(...)
            {
              that->mark_exceptional_finish();
            }
          }
        };

        //////////////////////////
        /// future_deferred_shared_state
        //////////////////////////
        template<typename Rp, typename Fp>
        struct future_deferred_shared_state: shared_state<Rp>
        {
          typedef shared_state<Rp> base_type;
          Fp func_;

        public:
          explicit future_deferred_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f)
          : func_(autoboost::move(f))
          {
            this->set_deferred();
          }

          virtual void execute(autoboost::unique_lock<autoboost::mutex>& lck) {
            try
            {
              Fp local_fuct=autoboost::move(func_);
              relocker relock(lck);
              Rp res = local_fuct();
              relock.lock();
              this->mark_finished_with_result_internal(autoboost::move(res), lck);
            }
            catch (...)
            {
              this->mark_exceptional_finish_internal(current_exception(), lck);
            }
          }
        };
        template<typename Rp, typename Fp>
        struct future_deferred_shared_state<Rp&,Fp>: shared_state<Rp&>
        {
          typedef shared_state<Rp&> base_type;
          Fp func_;

        public:
          explicit future_deferred_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f)
          : func_(autoboost::move(f))
          {
            this->set_deferred();
          }

          virtual void execute(autoboost::unique_lock<autoboost::mutex>& lck) {
            try
            {
              this->mark_finished_with_result_internal(func_(), lck);
            }
            catch (...)
            {
              this->mark_exceptional_finish_internal(current_exception(), lck);
            }
          }
        };

        template<typename Fp>
        struct future_deferred_shared_state<void,Fp>: shared_state<void>
        {
          typedef shared_state<void> base_type;
          Fp func_;

        public:
          explicit future_deferred_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f)
          : func_(autoboost::move(f))
          {
            this->set_deferred();
          }

          virtual void execute(autoboost::unique_lock<autoboost::mutex>& lck) {
            try
            {
              Fp local_fuct=autoboost::move(func_);
              relocker relock(lck);
              local_fuct();
              relock.lock();
              this->mark_finished_with_result_internal(lck);
            }
            catch (...)
            {
              this->mark_exceptional_finish_internal(current_exception(), lck);
            }
          }
        };

        class future_waiter
        {
            struct registered_waiter;
            typedef std::vector<int>::size_type count_type;

            struct registered_waiter
            {
                autoboost::shared_ptr<detail::shared_state_base> future_;
                detail::shared_state_base::waiter_list::iterator wait_iterator;
                count_type index;

                registered_waiter(autoboost::shared_ptr<detail::shared_state_base> const& a_future,
                                  detail::shared_state_base::waiter_list::iterator wait_iterator_,
                                  count_type index_):
                    future_(a_future),wait_iterator(wait_iterator_),index(index_)
                {}
            };

            struct all_futures_lock
            {
#ifdef _MANAGED
                   typedef std::ptrdiff_t count_type_portable;
#else
                   typedef count_type count_type_portable;
#endif
                   count_type_portable count;
                   autoboost::scoped_array<autoboost::unique_lock<autoboost::mutex> > locks;

                all_futures_lock(std::vector<registered_waiter>& futures):
                    count(futures.size()),locks(new autoboost::unique_lock<autoboost::mutex>[count])
                {
                    for(count_type_portable i=0;i<count;++i)
                    {
                        locks[i]=AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::unique_lock<autoboost::mutex>(futures[i].future_->mutex));
                    }
                }

                void lock()
                {
                    autoboost::lock(locks.get(),locks.get()+count);
                }

                void unlock()
                {
                    for(count_type_portable i=0;i<count;++i)
                    {
                        locks[i].unlock();
                    }
                }
            };

            autoboost::condition_variable_any cv;
            std::vector<registered_waiter> futures;
            count_type future_count;

        public:
            future_waiter():
                future_count(0)
            {}

            template<typename F>
            void add(F& f)
            {
                if(f.future_)
                {
                  registered_waiter waiter(f.future_,f.future_->register_external_waiter(cv),future_count);
                  try {
                      futures.push_back(waiter);
                  } catch(...) {
                    f.future_->remove_external_waiter(waiter.wait_iterator);
                    throw;
                  }
                }
                ++future_count;
            }

#ifndef AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
            template<typename F1, typename... Fs>
            void add(F1& f1, Fs&... fs)
            {
              add(f1); add(fs...);
            }
#endif

            count_type wait()
            {
                all_futures_lock lk(futures);
                for(;;)
                {
                    for(count_type i=0;i<futures.size();++i)
                    {
                        if(futures[i].future_->done)
                        {
                            return futures[i].index;
                        }
                    }
                    cv.wait(lk);
                }
            }

            ~future_waiter()
            {
                for(count_type i=0;i<futures.size();++i)
                {
                    futures[i].future_->remove_external_waiter(futures[i].wait_iterator);
                }
            }
        };

    }

    template <typename R>
    class AUTOBOOST_THREAD_FUTURE;

    template <typename R>
    class shared_future;

    template<typename T>
    struct is_future_type
    {
        AUTOBOOST_STATIC_CONSTANT(bool, value=false);
        typedef void type;
    };

    template<typename T>
    struct is_future_type<AUTOBOOST_THREAD_FUTURE<T> >
    {
        AUTOBOOST_STATIC_CONSTANT(bool, value=true);
        typedef T type;
    };

    template<typename T>
    struct is_future_type<shared_future<T> >
    {
        AUTOBOOST_STATIC_CONSTANT(bool, value=true);
        typedef T type;
    };

    template<typename Iterator>
    typename autoboost::disable_if<is_future_type<Iterator>,void>::type wait_for_all(Iterator begin,Iterator end)
    {
        for(Iterator current=begin;current!=end;++current)
        {
            current->wait();
        }
    }

#ifdef AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
    template<typename F1,typename F2>
    typename autoboost::enable_if<is_future_type<F1>,void>::type wait_for_all(F1& f1,F2& f2)
    {
        f1.wait();
        f2.wait();
    }

    template<typename F1,typename F2,typename F3>
    void wait_for_all(F1& f1,F2& f2,F3& f3)
    {
        f1.wait();
        f2.wait();
        f3.wait();
    }

    template<typename F1,typename F2,typename F3,typename F4>
    void wait_for_all(F1& f1,F2& f2,F3& f3,F4& f4)
    {
        f1.wait();
        f2.wait();
        f3.wait();
        f4.wait();
    }

    template<typename F1,typename F2,typename F3,typename F4,typename F5>
    void wait_for_all(F1& f1,F2& f2,F3& f3,F4& f4,F5& f5)
    {
        f1.wait();
        f2.wait();
        f3.wait();
        f4.wait();
        f5.wait();
    }
#else
    template<typename F1, typename... Fs>
    void wait_for_all(F1& f1, Fs&... fs)
    {
        bool dummy[] = { (f1.wait(), true), (fs.wait(), true)... };

        // prevent unused parameter warning
        (void) dummy;
    }
#endif // !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

    template<typename Iterator>
    typename autoboost::disable_if<is_future_type<Iterator>,Iterator>::type wait_for_any(Iterator begin,Iterator end)
    {
        if(begin==end)
            return end;

        detail::future_waiter waiter;
        for(Iterator current=begin;current!=end;++current)
        {
            waiter.add(*current);
        }
        return autoboost::next(begin,waiter.wait());
    }

#ifdef AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES
    template<typename F1,typename F2>
    typename autoboost::enable_if<is_future_type<F1>,unsigned>::type wait_for_any(F1& f1,F2& f2)
    {
        detail::future_waiter waiter;
        waiter.add(f1);
        waiter.add(f2);
        return waiter.wait();
    }

    template<typename F1,typename F2,typename F3>
    unsigned wait_for_any(F1& f1,F2& f2,F3& f3)
    {
        detail::future_waiter waiter;
        waiter.add(f1);
        waiter.add(f2);
        waiter.add(f3);
        return waiter.wait();
    }

    template<typename F1,typename F2,typename F3,typename F4>
    unsigned wait_for_any(F1& f1,F2& f2,F3& f3,F4& f4)
    {
        detail::future_waiter waiter;
        waiter.add(f1);
        waiter.add(f2);
        waiter.add(f3);
        waiter.add(f4);
        return waiter.wait();
    }

    template<typename F1,typename F2,typename F3,typename F4,typename F5>
    unsigned wait_for_any(F1& f1,F2& f2,F3& f3,F4& f4,F5& f5)
    {
        detail::future_waiter waiter;
        waiter.add(f1);
        waiter.add(f2);
        waiter.add(f3);
        waiter.add(f4);
        waiter.add(f5);
        return waiter.wait();
    }
#else
    template<typename F1, typename... Fs>
    typename autoboost::enable_if<is_future_type<F1>, unsigned>::type wait_for_any(F1& f1, Fs&... fs)
    {
      detail::future_waiter waiter;
      waiter.add(f1, fs...);
      return waiter.wait();
    }
#endif // !defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)

    template <typename R>
    class promise;

    template <typename R>
    class packaged_task;

    namespace detail
    {
      /// Common implementation for all the futures independently of the return type
      class base_future
      {
      };
      /// Common implementation for future and shared_future.
      template <typename R>
      class basic_future : public base_future
      {
      protected:
      public:

        typedef autoboost::shared_ptr<detail::shared_state<R> > future_ptr;
        typedef typename detail::shared_state<R>::move_dest_type move_dest_type;

        static //AUTOBOOST_CONSTEXPR
        future_ptr make_exceptional_future_ptr(exceptional_ptr const& ex) {
          promise<R> p;
          p.set_exception(ex.ptr_);
          return p.get_future().future_;
        }

        void set_exceptional_if_invalid() {
          if (valid()) return;
          promise<R> p;
          p.set_exception(future_uninitialized());
          future_ = p.get_future().future_;
        }


        future_ptr future_;

        basic_future(future_ptr a_future):
          future_(a_future)
        {
        }
        // Copy construction from a shared_future
        explicit basic_future(const shared_future<R>&) AUTOBOOST_NOEXCEPT;

      public:
        typedef future_state::state state;

        AUTOBOOST_THREAD_MOVABLE_ONLY(basic_future)
        basic_future(): future_() {}


        //AUTOBOOST_CONSTEXPR
        basic_future(exceptional_ptr const& ex)
          : future_(make_exceptional_future_ptr(ex))
        {
        }

        ~basic_future() {}

        basic_future(AUTOBOOST_THREAD_RV_REF(basic_future) other) AUTOBOOST_NOEXCEPT:
        future_(AUTOBOOST_THREAD_RV(other).future_)
        {
            AUTOBOOST_THREAD_RV(other).future_.reset();
        }
        basic_future& operator=(AUTOBOOST_THREAD_RV_REF(basic_future) other) AUTOBOOST_NOEXCEPT
        {
            future_=AUTOBOOST_THREAD_RV(other).future_;
            AUTOBOOST_THREAD_RV(other).future_.reset();
            return *this;
        }
        void swap(basic_future& that) AUTOBOOST_NOEXCEPT
        {
          future_.swap(that.future_);
        }
        // functions to check state, and wait for ready
        state get_state() const
        {
            if(!future_)
            {
                return future_state::uninitialized;
            }
            return future_->get_state();
        }

        bool is_ready() const
        {
            return get_state()==future_state::ready;
        }

        bool has_exception() const
        {
            return future_ && future_->has_exception();
        }

        bool has_value() const
        {
            return future_ && future_->has_value();
        }

        launch launch_policy(autoboost::unique_lock<autoboost::mutex>& lk) const
        {
            if ( future_ ) return future_->launch_policy(lk);
            else return launch(launch::none);
        }

        exception_ptr get_exception_ptr()
        {
            return future_
                ? future_->get_exception_ptr()
                : exception_ptr();
        }

        bool valid() const AUTOBOOST_NOEXCEPT
        {
            return future_ != 0;
        }

        void wait() const
        {
            if(!future_)
            {
                autoboost::throw_exception(future_uninitialized());
            }
            future_->wait(false);
        }

#if defined AUTOBOOST_THREAD_USES_DATETIME
        template<typename Duration>
        bool timed_wait(Duration const& rel_time) const
        {
            return timed_wait_until(autoboost::get_system_time()+rel_time);
        }

        bool timed_wait_until(autoboost::system_time const& abs_time) const
        {
            if(!future_)
            {
                autoboost::throw_exception(future_uninitialized());
            }
            return future_->timed_wait_until(abs_time);
        }
#endif
#ifdef AUTOBOOST_THREAD_USES_CHRONO
        template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const
        {
          return wait_until(chrono::steady_clock::now() + rel_time);

        }
        template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const
        {
          if(!future_)
          {
              autoboost::throw_exception(future_uninitialized());
          }
          return future_->wait_until(abs_time);
        }
#endif

      };

    } // detail
    AUTOBOOST_THREAD_DCL_MOVABLE_BEG(R) detail::basic_future<R> AUTOBOOST_THREAD_DCL_MOVABLE_END

    namespace detail
    {
#if (!defined _MSC_VER || _MSC_VER >= 1400) // _MSC_VER == 1400 on MSVC 2005
        template <class Rp, class Fp>
        AUTOBOOST_THREAD_FUTURE<Rp>
        make_future_async_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f);

        template <class Rp, class Fp>
        AUTOBOOST_THREAD_FUTURE<Rp>
        make_future_deferred_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f);
#endif // #if (!defined _MSC_VER || _MSC_VER >= 1400)
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
        template<typename F, typename Rp, typename Fp>
        struct future_deferred_continuation_shared_state;
        template<typename F, typename Rp, typename Fp>
        struct future_async_continuation_shared_state;

        template <class F, class Rp, class Fp>
        AUTOBOOST_THREAD_FUTURE<Rp>
        make_future_async_continuation_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c);

        template <class F, class Rp, class Fp>
        AUTOBOOST_THREAD_FUTURE<Rp>
        make_future_deferred_continuation_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c);
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_UNWRAP
        template<typename F, typename Rp>
        struct future_unwrap_shared_state;
        template <class F, class Rp>
        inline AUTOBOOST_THREAD_FUTURE<Rp>
        make_future_unwrap_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f);
#endif
    }

    template <typename R>
    class AUTOBOOST_THREAD_FUTURE : public detail::basic_future<R>
    {
    private:
        typedef detail::basic_future<R> base_type;
        typedef typename base_type::future_ptr future_ptr;

        friend class shared_future<R>;
        friend class promise<R>;
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
        template <typename, typename, typename>
        friend struct detail::future_async_continuation_shared_state;
        template <typename, typename, typename>
        friend struct detail::future_deferred_continuation_shared_state;

        template <class F, class Rp, class Fp>
        friend AUTOBOOST_THREAD_FUTURE<Rp>
        detail::make_future_async_continuation_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c);

        template <class F, class Rp, class Fp>
        friend AUTOBOOST_THREAD_FUTURE<Rp>
        detail::make_future_deferred_continuation_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c);
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_UNWRAP
        template<typename F, typename Rp>
        friend struct detail::future_unwrap_shared_state;
        template <class F, class Rp>
        friend AUTOBOOST_THREAD_FUTURE<Rp>
        detail::make_future_unwrap_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f);
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
        template <class> friend class packaged_task; // todo check if this works in windows
#else
        friend class packaged_task<R>;
#endif
        friend class detail::future_waiter;

        template <class Rp, class Fp>
        friend AUTOBOOST_THREAD_FUTURE<Rp>
        detail::make_future_async_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f);

        template <class Rp, class Fp>
        friend AUTOBOOST_THREAD_FUTURE<Rp>
        detail::make_future_deferred_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f);


        typedef typename base_type::move_dest_type move_dest_type;
    public: // when_all

        AUTOBOOST_THREAD_FUTURE(future_ptr a_future):
          base_type(a_future)
        {
        }

    public:
        AUTOBOOST_THREAD_MOVABLE_ONLY(AUTOBOOST_THREAD_FUTURE)
        typedef future_state::state state;
        typedef R value_type; // EXTENSION

        AUTOBOOST_CONSTEXPR AUTOBOOST_THREAD_FUTURE() {}
        //AUTOBOOST_CONSTEXPR
        AUTOBOOST_THREAD_FUTURE(exceptional_ptr const& ex):
            base_type(ex) {}

        ~AUTOBOOST_THREAD_FUTURE() {}

        AUTOBOOST_THREAD_FUTURE(AUTOBOOST_THREAD_RV_REF(AUTOBOOST_THREAD_FUTURE) other) AUTOBOOST_NOEXCEPT:
        base_type(autoboost::move(static_cast<base_type&>(AUTOBOOST_THREAD_RV(other))))
        {
        }
        inline AUTOBOOST_THREAD_FUTURE(AUTOBOOST_THREAD_RV_REF(AUTOBOOST_THREAD_FUTURE<AUTOBOOST_THREAD_FUTURE<R> >) other); // EXTENSION

        AUTOBOOST_THREAD_FUTURE& operator=(AUTOBOOST_THREAD_RV_REF(AUTOBOOST_THREAD_FUTURE) other) AUTOBOOST_NOEXCEPT
        {
            this->base_type::operator=(autoboost::move(static_cast<base_type&>(AUTOBOOST_THREAD_RV(other))));
            return *this;
        }

        shared_future<R> share()
        {
          return shared_future<R>(::autoboost::move(*this));
        }

        void swap(AUTOBOOST_THREAD_FUTURE& other)
        {
            static_cast<base_type*>(this)->swap(other);
        }

        // todo this function must be private and friendship provided to the internal users.
        void set_async()
        {
          this->future_->set_async();
        }
        // todo this function must be private and friendship provided to the internal users.
        void set_deferred()
        {
          this->future_->set_deferred();
        }
        bool run_if_is_deferred() {
          return this->future_->run_if_is_deferred();
        }
        bool run_if_is_deferred_or_ready() {
          return this->future_->run_if_is_deferred_or_ready();
        }
        // retrieving the value
        move_dest_type get()
        {
            if(!this->future_)
            {
                autoboost::throw_exception(future_uninitialized());
            }
            future_ptr fut_=this->future_;
#ifdef AUTOBOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
            this->future_.reset();
#endif
            return fut_->get();
        }

        template <typename R2>
        typename autoboost::disable_if< is_void<R2>, move_dest_type>::type
        get_or(AUTOBOOST_THREAD_RV_REF(R2) v)
        {
            if(!this->future_)
            {
                autoboost::throw_exception(future_uninitialized());
            }
            this->future_->wait(false);
            future_ptr fut_=this->future_;
#ifdef AUTOBOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
            this->future_.reset();
#endif
            if (fut_->has_value()) {
              return fut_->get();
            }
            else {
              return autoboost::move(v);
            }
        }

        template <typename R2>
        typename autoboost::disable_if< is_void<R2>, move_dest_type>::type
        get_or(R2 const& v)  // EXTENSION
        {
            if(!this->future_)
            {
                autoboost::throw_exception(future_uninitialized());
            }
            this->future_->wait(false);
            future_ptr fut_=this->future_;
#ifdef AUTOBOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
            this->future_.reset();
#endif
            if (fut_->has_value()) {
              return fut_->get();
            }
            else {
              return v;
            }
        }

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
        template<typename F>
        inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE)>::type>
        then(AUTOBOOST_THREAD_FWD_REF(F) func);  // EXTENSION
        template<typename F>
        inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE)>::type>
        then(launch policy, AUTOBOOST_THREAD_FWD_REF(F) func);  // EXTENSION
  #ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS
        template<typename Ex, typename F>
        inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE)>::type>
        then(Ex& ex, AUTOBOOST_THREAD_FWD_REF(F) func);  // EXTENSION
  #endif

        template <typename R2>
        inline typename autoboost::disable_if< is_void<R2>, AUTOBOOST_THREAD_FUTURE<R> >::type
        fallback_to(AUTOBOOST_THREAD_RV_REF(R2) v);  // EXTENSION
        template <typename R2>
        inline typename autoboost::disable_if< is_void<R2>, AUTOBOOST_THREAD_FUTURE<R> >::type
        fallback_to(R2 const& v);  // EXTENSION

#endif

    };

    AUTOBOOST_THREAD_DCL_MOVABLE_BEG(T) AUTOBOOST_THREAD_FUTURE<T> AUTOBOOST_THREAD_DCL_MOVABLE_END

        template <typename R2>
        class AUTOBOOST_THREAD_FUTURE<AUTOBOOST_THREAD_FUTURE<R2> > : public detail::basic_future<AUTOBOOST_THREAD_FUTURE<R2> >
        {
          typedef AUTOBOOST_THREAD_FUTURE<R2> R;

        private:
            typedef detail::basic_future<R> base_type;
            typedef typename base_type::future_ptr future_ptr;

            friend class shared_future<R>;
            friend class promise<R>;
    #if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
            template <typename, typename, typename>
            friend struct detail::future_async_continuation_shared_state;
            template <typename, typename, typename>
            friend struct detail::future_deferred_continuation_shared_state;

            template <class F, class Rp, class Fp>
            friend AUTOBOOST_THREAD_FUTURE<Rp>
            detail::make_future_async_continuation_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c);

            template <class F, class Rp, class Fp>
            friend AUTOBOOST_THREAD_FUTURE<Rp>
            detail::make_future_deferred_continuation_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c);
    #endif
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_UNWRAP
            template<typename F, typename Rp>
            friend struct detail::future_unwrap_shared_state;
        template <class F, class Rp>
        friend AUTOBOOST_THREAD_FUTURE<Rp>
        detail::make_future_unwrap_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f);
#endif
    #if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
            template <class> friend class packaged_task; // todo check if this works in windows
    #else
            friend class packaged_task<R>;
    #endif
            friend class detail::future_waiter;

            template <class Rp, class Fp>
            friend AUTOBOOST_THREAD_FUTURE<Rp>
            detail::make_future_async_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f);

            template <class Rp, class Fp>
            friend AUTOBOOST_THREAD_FUTURE<Rp>
            detail::make_future_deferred_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f);

            typedef typename base_type::move_dest_type move_dest_type;

            AUTOBOOST_THREAD_FUTURE(future_ptr a_future):
              base_type(a_future)
            {
            }

        public:
            AUTOBOOST_THREAD_MOVABLE_ONLY(AUTOBOOST_THREAD_FUTURE)
            typedef future_state::state state;
            typedef R value_type; // EXTENSION

            AUTOBOOST_CONSTEXPR AUTOBOOST_THREAD_FUTURE() {}
            //AUTOBOOST_CONSTEXPR
            AUTOBOOST_THREAD_FUTURE(exceptional_ptr const& ex):
                base_type(ex) {}

            ~AUTOBOOST_THREAD_FUTURE() {}

            AUTOBOOST_THREAD_FUTURE(AUTOBOOST_THREAD_RV_REF(AUTOBOOST_THREAD_FUTURE) other) AUTOBOOST_NOEXCEPT:
            base_type(autoboost::move(static_cast<base_type&>(AUTOBOOST_THREAD_RV(other))))
            {
            }

            AUTOBOOST_THREAD_FUTURE& operator=(AUTOBOOST_THREAD_RV_REF(AUTOBOOST_THREAD_FUTURE) other) AUTOBOOST_NOEXCEPT
            {
                this->base_type::operator=(autoboost::move(static_cast<base_type&>(AUTOBOOST_THREAD_RV(other))));
                return *this;
            }

            shared_future<R> share()
            {
              return shared_future<R>(::autoboost::move(*this));
            }

            void swap(AUTOBOOST_THREAD_FUTURE& other)
            {
                static_cast<base_type*>(this)->swap(other);
            }

            // todo this function must be private and friendship provided to the internal users.
            void set_async()
            {
              this->future_->set_async();
            }
            // todo this function must be private and friendship provided to the internal users.
            void set_deferred()
            {
              this->future_->set_deferred();
            }
            bool run_if_is_deferred() {
              return this->future_->run_if_is_deferred();
            }
            bool run_if_is_deferred_or_ready() {
              return this->future_->run_if_is_deferred_or_ready();
            }
            // retrieving the value
            move_dest_type get()
            {
                if(!this->future_)
                {
                    autoboost::throw_exception(future_uninitialized());
                }
                future_ptr fut_=this->future_;
    #ifdef AUTOBOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
                this->future_.reset();
    #endif
                return fut_->get();
            }
            move_dest_type get_or(AUTOBOOST_THREAD_RV_REF(R) v) // EXTENSION
            {
                if(!this->future_)
                {
                    autoboost::throw_exception(future_uninitialized());
                }
                this->future_->wait(false);
                future_ptr fut_=this->future_;
    #ifdef AUTOBOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
                this->future_.reset();
    #endif
                if (fut_->has_value()) return fut_->get();
                else return autoboost::move(v);
            }

            move_dest_type get_or(R const& v) // EXTENSION
            {
                if(!this->future_)
                {
                    autoboost::throw_exception(future_uninitialized());
                }
                this->future_->wait(false);
                future_ptr fut_=this->future_;
    #ifdef AUTOBOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
                this->future_.reset();
    #endif
                if (fut_->has_value()) return fut_->get();
                else return v;
            }


    #if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
            template<typename F>
            inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE)>::type>
            then(AUTOBOOST_THREAD_FWD_REF(F) func); // EXTENSION
            template<typename F>
            inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE)>::type>
            then(launch policy, AUTOBOOST_THREAD_FWD_REF(F) func); // EXTENSION
      #ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS
            template<typename Ex, typename F>
            inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE)>::type>
            then(Ex &ex, AUTOBOOST_THREAD_FWD_REF(F) func); // EXTENSION
      #endif
    #endif

    #if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_UNWRAP
            inline
            AUTOBOOST_THREAD_FUTURE<R2>
            unwrap(); // EXTENSION
    #endif

  };

    template <typename R>
    class shared_future : public detail::basic_future<R>
    {
        typedef detail::basic_future<R> base_type;
        typedef typename base_type::future_ptr future_ptr;

        friend class detail::future_waiter;
        friend class promise<R>;

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
        template <typename, typename, typename>
        friend struct detail::future_async_continuation_shared_state;
        template <typename, typename, typename>
        friend struct detail::future_deferred_continuation_shared_state;

        template <class F, class Rp, class Fp>
        friend AUTOBOOST_THREAD_FUTURE<Rp>
        detail::make_future_async_continuation_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c);

        template <class F, class Rp, class Fp>
        friend AUTOBOOST_THREAD_FUTURE<Rp>
        detail::make_future_deferred_continuation_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c);
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
        template <class> friend class packaged_task;// todo check if this works in windows
#else
        friend class packaged_task<R>;
#endif
        shared_future(future_ptr a_future):
          base_type(a_future)
        {}

    public:
        AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(shared_future)
        typedef R value_type; // EXTENSION

        shared_future(shared_future const& other):
        base_type(other)
        {}

        typedef future_state::state state;

        AUTOBOOST_CONSTEXPR shared_future()
        {}
        //AUTOBOOST_CONSTEXPR
        shared_future(exceptional_ptr const& ex):
            base_type(ex) {}
        ~shared_future()
        {}

        shared_future& operator=(AUTOBOOST_THREAD_COPY_ASSIGN_REF(shared_future) other)
        {
            shared_future(other).swap(*this);
            return *this;
        }

        shared_future(AUTOBOOST_THREAD_RV_REF(shared_future) other) AUTOBOOST_NOEXCEPT :
        base_type(autoboost::move(static_cast<base_type&>(AUTOBOOST_THREAD_RV(other))))
        {
            AUTOBOOST_THREAD_RV(other).future_.reset();
        }
        shared_future(AUTOBOOST_THREAD_RV_REF( AUTOBOOST_THREAD_FUTURE<R> ) other) AUTOBOOST_NOEXCEPT :
        base_type(autoboost::move(static_cast<base_type&>(AUTOBOOST_THREAD_RV(other))))
        {
        }

        shared_future& operator=(AUTOBOOST_THREAD_RV_REF(shared_future) other) AUTOBOOST_NOEXCEPT
        {
            base_type::operator=(autoboost::move(static_cast<base_type&>(AUTOBOOST_THREAD_RV(other))));
            return *this;
        }
        shared_future& operator=(AUTOBOOST_THREAD_RV_REF( AUTOBOOST_THREAD_FUTURE<R> ) other) AUTOBOOST_NOEXCEPT
        {
            base_type::operator=(autoboost::move(static_cast<base_type&>(AUTOBOOST_THREAD_RV(other))));
            return *this;
        }

        void swap(shared_future& other) AUTOBOOST_NOEXCEPT
        {
            static_cast<base_type*>(this)->swap(other);
        }
        bool run_if_is_deferred() {
          return this->future_->run_if_is_deferred();
        }
        bool run_if_is_deferred_or_ready() {
          return this->future_->run_if_is_deferred_or_ready();
        }
        // retrieving the value
        typename detail::shared_state<R>::shared_future_get_result_type get()
        {
            if(!this->future_)
            {
                autoboost::throw_exception(future_uninitialized());
            }
            return this->future_->get_sh();
        }

        template <typename R2>
        typename autoboost::disable_if< is_void<R2>, typename detail::shared_state<R>::shared_future_get_result_type>::type
        get_or(AUTOBOOST_THREAD_RV_REF(R2) v) // EXTENSION
        {
            if(!this->future_)
            {
                autoboost::throw_exception(future_uninitialized());
            }
            future_ptr fut_=this->future_;
            fut_->wait();
            if (fut_->has_value()) return fut_->get_sh();
            else return autoboost::move(v);
        }

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
        template<typename F>
        inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(shared_future)>::type>
        then(AUTOBOOST_THREAD_FWD_REF(F) func) const; // EXTENSION
        template<typename F>
        inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(shared_future)>::type>
        then(launch policy, AUTOBOOST_THREAD_FWD_REF(F) func) const; // EXTENSION
  #ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS
        template<typename Ex, typename F>
        inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(shared_future)>::type>
        then(Ex& ex, AUTOBOOST_THREAD_FWD_REF(F) func) const; // EXTENSION
  #endif
#endif

    };

    AUTOBOOST_THREAD_DCL_MOVABLE_BEG(T) shared_future<T> AUTOBOOST_THREAD_DCL_MOVABLE_END

    namespace detail
    {
      /// Copy construction from a shared_future
      template <typename R>
      inline basic_future<R>::basic_future(const shared_future<R>& other) AUTOBOOST_NOEXCEPT
      : future_(other.future_)
      {
      }
    }

    template <typename R>
    class promise
    {
        typedef autoboost::shared_ptr<detail::shared_state<R> > future_ptr;

        typedef typename detail::shared_state<R>::source_reference_type source_reference_type;
        typedef typename detail::shared_state<R>::rvalue_source_type rvalue_source_type;
        typedef typename detail::shared_state<R>::move_dest_type move_dest_type;
        typedef typename detail::shared_state<R>::shared_future_get_result_type shared_future_get_result_type;

        future_ptr future_;
        bool future_obtained;

        void lazy_init()
        {
#if defined AUTOBOOST_THREAD_PROVIDES_PROMISE_LAZY
#include <autoboost/detail/atomic_undef_macros.hpp>
          if(!atomic_load(&future_))
            {
                future_ptr blank;
                atomic_compare_exchange(&future_,&blank,future_ptr(new detail::shared_state<R>));
            }
#include <autoboost/detail/atomic_redef_macros.hpp>
#endif
        }

    public:
        AUTOBOOST_THREAD_MOVABLE_ONLY(promise)
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CTOR_ALLOCATORS
        template <class Allocator>
        promise(autoboost::allocator_arg_t, Allocator a)
        {
          typedef typename Allocator::template rebind<detail::shared_state<R> >::other A2;
          A2 a2(a);
          typedef thread_detail::allocator_destructor<A2> D;

          future_ = future_ptr(::new(a2.allocate(1)) detail::shared_state<R>(), D(a2, 1) );
          future_obtained = false;
        }
#endif
        promise():
#if defined AUTOBOOST_THREAD_PROVIDES_PROMISE_LAZY
            future_(),
#else
            future_(new detail::shared_state<R>()),
#endif
            future_obtained(false)
        {}

        ~promise()
        {
            if(future_)
            {
                autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);

                if(!future_->done && !future_->is_constructed)
                {
                    future_->mark_exceptional_finish_internal(autoboost::copy_exception(broken_promise()), lock);
                }
            }
        }

        // Assignment
        promise(AUTOBOOST_THREAD_RV_REF(promise) rhs) AUTOBOOST_NOEXCEPT :
            future_(AUTOBOOST_THREAD_RV(rhs).future_),future_obtained(AUTOBOOST_THREAD_RV(rhs).future_obtained)
        {
            AUTOBOOST_THREAD_RV(rhs).future_.reset();
            AUTOBOOST_THREAD_RV(rhs).future_obtained=false;
        }
        promise & operator=(AUTOBOOST_THREAD_RV_REF(promise) rhs) AUTOBOOST_NOEXCEPT
        {
            future_=AUTOBOOST_THREAD_RV(rhs).future_;
            future_obtained=AUTOBOOST_THREAD_RV(rhs).future_obtained;
            AUTOBOOST_THREAD_RV(rhs).future_.reset();
            AUTOBOOST_THREAD_RV(rhs).future_obtained=false;
            return *this;
        }

        void swap(promise& other)
        {
            future_.swap(other.future_);
            std::swap(future_obtained,other.future_obtained);
        }

        // Result retrieval
        AUTOBOOST_THREAD_FUTURE<R> get_future()
        {
            lazy_init();
            if (future_.get()==0)
            {
                autoboost::throw_exception(promise_moved());
            }
            if (future_obtained)
            {
                autoboost::throw_exception(future_already_retrieved());
            }
            future_obtained=true;
            return AUTOBOOST_THREAD_FUTURE<R>(future_);
        }

#if defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
        template <class TR>
        typename autoboost::enable_if_c<is_copy_constructible<TR>::value && is_same<R, TR>::value, void>::type set_value(TR const &  r)
        {
            lazy_init();
            autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);
            if(future_->done)
            {
                autoboost::throw_exception(promise_already_satisfied());
            }
            future_->mark_finished_with_result_internal(r, lock);
        }
#else
        void set_value(source_reference_type r)
        {
            lazy_init();
            autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);
            if(future_->done)
            {
                autoboost::throw_exception(promise_already_satisfied());
            }
            future_->mark_finished_with_result_internal(r, lock);
        }
#endif

        void set_value(rvalue_source_type r)
        {
            lazy_init();
            autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);
            if(future_->done)
            {
                autoboost::throw_exception(promise_already_satisfied());
            }
#if ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
            future_->mark_finished_with_result_internal(autoboost::move(r), lock);
#else
            future_->mark_finished_with_result_internal(static_cast<rvalue_source_type>(r), lock);
#endif
        }
        void set_exception(autoboost::exception_ptr p)
        {
            lazy_init();
            autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);
            if(future_->done)
            {
                autoboost::throw_exception(promise_already_satisfied());
            }
            future_->mark_exceptional_finish_internal(p, lock);
        }
        template <typename E>
        void set_exception(E ex)
        {
          set_exception(autoboost::copy_exception(ex));
        }
        // setting the result with deferred notification
#if defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
        template <class TR>
        typename autoboost::enable_if_c<is_copy_constructible<TR>::value && is_same<R, TR>::value, void>::type set_value_autoboostat_thread_exit(TR const& r)
        {
          if (future_.get()==0)
          {
              autoboost::throw_exception(promise_moved());
          }
          future_->set_value_autoboostat_thread_exit(r);
        }
#else
        void set_value_autoboostat_thread_exit(source_reference_type r)
        {
          if (future_.get()==0)
          {
              autoboost::throw_exception(promise_moved());
          }
          future_->set_value_autoboostat_thread_exit(r);
        }
#endif
        void set_value_autoboostat_thread_exit(AUTOBOOST_THREAD_RV_REF(R) r)
        {
          if (future_.get()==0)
          {
              autoboost::throw_exception(promise_moved());
          }
          future_->set_value_autoboostat_thread_exit(autoboost::move(r));
        }
        void set_exception_autoboostat_thread_exit(exception_ptr e)
        {
          if (future_.get()==0)
          {
              autoboost::throw_exception(promise_moved());
          }
          future_->set_exception_autoboostat_thread_exit(e);
        }
        template <typename E>
        void set_exception_autoboostat_thread_exit(E ex)
        {
          set_exception_autoboostat_thread_exit(autoboost::copy_exception(ex));
        }

        template<typename F>
        void set_wait_callback(F f)
        {
            lazy_init();
            future_->set_wait_callback(f,this);
        }

    };

    template <typename R>
    class promise<R&>
    {
        typedef autoboost::shared_ptr<detail::shared_state<R&> > future_ptr;

        future_ptr future_;
        bool future_obtained;

        void lazy_init()
        {
#if defined AUTOBOOST_THREAD_PROVIDES_PROMISE_LAZY
#include <autoboost/detail/atomic_undef_macros.hpp>
            if(!atomic_load(&future_))
            {
                future_ptr blank;
                atomic_compare_exchange(&future_,&blank,future_ptr(new detail::shared_state<R&>));
            }
#include <autoboost/detail/atomic_redef_macros.hpp>
#endif
        }

    public:
        AUTOBOOST_THREAD_MOVABLE_ONLY(promise)
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CTOR_ALLOCATORS
        template <class Allocator>
        promise(autoboost::allocator_arg_t, Allocator a)
        {
          typedef typename Allocator::template rebind<detail::shared_state<R&> >::other A2;
          A2 a2(a);
          typedef thread_detail::allocator_destructor<A2> D;

          future_ = future_ptr(::new(a2.allocate(1)) detail::shared_state<R&>(), D(a2, 1) );
          future_obtained = false;
        }
#endif
        promise():
#if defined AUTOBOOST_THREAD_PROVIDES_PROMISE_LAZY
            future_(),
#else
            future_(new detail::shared_state<R&>()),
#endif
            future_obtained(false)
        {}

        ~promise()
        {
            if(future_)
            {
                autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);

                if(!future_->done && !future_->is_constructed)
                {
                    future_->mark_exceptional_finish_internal(autoboost::copy_exception(broken_promise()), lock);
                }
            }
        }

        // Assignment
        promise(AUTOBOOST_THREAD_RV_REF(promise) rhs) AUTOBOOST_NOEXCEPT :
            future_(AUTOBOOST_THREAD_RV(rhs).future_),future_obtained(AUTOBOOST_THREAD_RV(rhs).future_obtained)
        {
            AUTOBOOST_THREAD_RV(rhs).future_.reset();
            AUTOBOOST_THREAD_RV(rhs).future_obtained=false;
        }
        promise & operator=(AUTOBOOST_THREAD_RV_REF(promise) rhs) AUTOBOOST_NOEXCEPT
        {
            future_=AUTOBOOST_THREAD_RV(rhs).future_;
            future_obtained=AUTOBOOST_THREAD_RV(rhs).future_obtained;
            AUTOBOOST_THREAD_RV(rhs).future_.reset();
            AUTOBOOST_THREAD_RV(rhs).future_obtained=false;
            return *this;
        }

        void swap(promise& other)
        {
            future_.swap(other.future_);
            std::swap(future_obtained,other.future_obtained);
        }

        // Result retrieval
        AUTOBOOST_THREAD_FUTURE<R&> get_future()
        {
            lazy_init();
            if (future_.get()==0)
            {
                autoboost::throw_exception(promise_moved());
            }
            if (future_obtained)
            {
                autoboost::throw_exception(future_already_retrieved());
            }
            future_obtained=true;
            return AUTOBOOST_THREAD_FUTURE<R&>(future_);
        }

        void set_value(R& r)
        {
            lazy_init();
            autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);
            if(future_->done)
            {
                autoboost::throw_exception(promise_already_satisfied());
            }
            future_->mark_finished_with_result_internal(r, lock);
        }

        void set_exception(autoboost::exception_ptr p)
        {
            lazy_init();
            autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);
            if(future_->done)
            {
                autoboost::throw_exception(promise_already_satisfied());
            }
            future_->mark_exceptional_finish_internal(p, lock);
        }
        template <typename E>
        void set_exception(E ex)
        {
          set_exception(autoboost::copy_exception(ex));
        }

        // setting the result with deferred notification
        void set_value_autoboostat_thread_exit(R& r)
        {
          if (future_.get()==0)
          {
              autoboost::throw_exception(promise_moved());
          }
          future_->set_value_autoboostat_thread_exit(r);
        }

        void set_exception_autoboostat_thread_exit(exception_ptr e)
        {
          if (future_.get()==0)
          {
              autoboost::throw_exception(promise_moved());
          }
          future_->set_exception_autoboostat_thread_exit(e);
        }
        template <typename E>
        void set_exception_autoboostat_thread_exit(E ex)
        {
          set_exception_autoboostat_thread_exit(autoboost::copy_exception(ex));
        }

        template<typename F>
        void set_wait_callback(F f)
        {
            lazy_init();
            future_->set_wait_callback(f,this);
        }
    };

    template <>
    class promise<void>
    {
        typedef autoboost::shared_ptr<detail::shared_state<void> > future_ptr;

        future_ptr future_;
        bool future_obtained;

        void lazy_init()
        {
#if defined AUTOBOOST_THREAD_PROVIDES_PROMISE_LAZY
            if(!atomic_load(&future_))
            {
                future_ptr blank;
                atomic_compare_exchange(&future_,&blank,future_ptr(new detail::shared_state<void>));
            }
#endif
        }
    public:
        AUTOBOOST_THREAD_MOVABLE_ONLY(promise)

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CTOR_ALLOCATORS
        template <class Allocator>
        promise(autoboost::allocator_arg_t, Allocator a)
        {
          typedef typename Allocator::template rebind<detail::shared_state<void> >::other A2;
          A2 a2(a);
          typedef thread_detail::allocator_destructor<A2> D;

          future_ = future_ptr(::new(a2.allocate(1)) detail::shared_state<void>(), D(a2, 1) );
          future_obtained = false;
        }
#endif
        promise():
#if defined AUTOBOOST_THREAD_PROVIDES_PROMISE_LAZY
            future_(),
#else
            future_(new detail::shared_state<void>),
#endif
            future_obtained(false)
        {}

        ~promise()
        {
            if(future_)
            {
                autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);

                if(!future_->done && !future_->is_constructed)
                {
                    future_->mark_exceptional_finish_internal(autoboost::copy_exception(broken_promise()), lock);
                }
            }
        }

        // Assignment
        promise(AUTOBOOST_THREAD_RV_REF(promise) rhs) AUTOBOOST_NOEXCEPT :
            future_(AUTOBOOST_THREAD_RV(rhs).future_),future_obtained(AUTOBOOST_THREAD_RV(rhs).future_obtained)
        {
          // we need to release the future as shared_ptr doesn't implements move semantics
            AUTOBOOST_THREAD_RV(rhs).future_.reset();
            AUTOBOOST_THREAD_RV(rhs).future_obtained=false;
        }

        promise & operator=(AUTOBOOST_THREAD_RV_REF(promise) rhs) AUTOBOOST_NOEXCEPT
        {
            future_=AUTOBOOST_THREAD_RV(rhs).future_;
            future_obtained=AUTOBOOST_THREAD_RV(rhs).future_obtained;
            AUTOBOOST_THREAD_RV(rhs).future_.reset();
            AUTOBOOST_THREAD_RV(rhs).future_obtained=false;
            return *this;
        }

        void swap(promise& other)
        {
            future_.swap(other.future_);
            std::swap(future_obtained,other.future_obtained);
        }

        // Result retrieval
        AUTOBOOST_THREAD_FUTURE<void> get_future()
        {
            lazy_init();

            if (future_.get()==0)
            {
                autoboost::throw_exception(promise_moved());
            }
            if(future_obtained)
            {
                autoboost::throw_exception(future_already_retrieved());
            }
            future_obtained=true;
            //return AUTOBOOST_THREAD_MAKE_RV_REF(AUTOBOOST_THREAD_FUTURE<void>(future_));
            return AUTOBOOST_THREAD_FUTURE<void>(future_);
        }

        void set_value()
        {
            lazy_init();
            autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);
            if(future_->done)
            {
                autoboost::throw_exception(promise_already_satisfied());
            }
            future_->mark_finished_with_result_internal(lock);
        }

        void set_exception(autoboost::exception_ptr p)
        {
            lazy_init();
            autoboost::unique_lock<autoboost::mutex> lock(future_->mutex);
            if(future_->done)
            {
                autoboost::throw_exception(promise_already_satisfied());
            }
            future_->mark_exceptional_finish_internal(p,lock);
        }
        template <typename E>
        void set_exception(E ex)
        {
          set_exception(autoboost::copy_exception(ex));
        }

        // setting the result with deferred notification
        void set_value_autoboostat_thread_exit()
        {
          if (future_.get()==0)
          {
              autoboost::throw_exception(promise_moved());
          }
          future_->set_value_autoboostat_thread_exit();
        }

        void set_exception_autoboostat_thread_exit(exception_ptr e)
        {
          if (future_.get()==0)
          {
              autoboost::throw_exception(promise_moved());
          }
          future_->set_exception_autoboostat_thread_exit(e);
        }
        template <typename E>
        void set_exception_autoboostat_thread_exit(E ex)
        {
          set_exception_autoboostat_thread_exit(autoboost::copy_exception(ex));
        }

        template<typename F>
        void set_wait_callback(F f)
        {
            lazy_init();
            future_->set_wait_callback(f,this);
        }

    };
}
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CTOR_ALLOCATORS
namespace autoboost { namespace container {
    template <class R, class Alloc>
    struct uses_allocator< ::autoboost::promise<R> , Alloc> : true_type
    {
    };
}}
#if ! defined  AUTOBOOST_NO_CXX11_ALLOCATOR
namespace std {
    template <class R, class Alloc>
    struct uses_allocator< ::autoboost::promise<R> , Alloc> : true_type
    {
    };
}
#endif
#endif

namespace autoboost
{

    AUTOBOOST_THREAD_DCL_MOVABLE_BEG(T) promise<T> AUTOBOOST_THREAD_DCL_MOVABLE_END

    namespace detail
    {
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
      template<typename R>
      struct task_base_shared_state;
#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
      template<typename R, typename ...ArgTypes>
      struct task_base_shared_state<R(ArgTypes...)>:
#else
      template<typename R>
      struct task_base_shared_state<R()>:
#endif
#else
      template<typename R>
      struct task_base_shared_state:
#endif
            detail::shared_state<R>
        {
            bool started;

            task_base_shared_state():
                started(false)
            {}

            void reset()
            {
              started=false;
            }
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            virtual void do_run(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)=0;
            void run(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
#else
            virtual void do_run()=0;
            void run()
#endif
            {
                {
                    autoboost::lock_guard<autoboost::mutex> lk(this->mutex);
                    if(started)
                    {
                        autoboost::throw_exception(task_already_started());
                    }
                    started=true;
                }
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
                do_run(autoboost::move(args)...);
#else
                do_run();
#endif
            }

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            virtual void do_apply(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)=0;
            void apply(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
#else
            virtual void do_apply()=0;
            void apply()
#endif
            {
                {
                    autoboost::lock_guard<autoboost::mutex> lk(this->mutex);
                    if(started)
                    {
                        autoboost::throw_exception(task_already_started());
                    }
                    started=true;
                }
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
                do_apply(autoboost::move(args)...);
#else
                do_apply();
#endif
            }

            void owner_destroyed()
            {
                autoboost::unique_lock<autoboost::mutex> lk(this->mutex);
                if(!started)
                {
                    started=true;
                    this->mark_exceptional_finish_internal(autoboost::copy_exception(autoboost::broken_promise()), lk);
                }
            }
        };

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
        template<typename F, typename R>
        struct task_shared_state;
#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
        template<typename F, typename R, typename ...ArgTypes>
        struct task_shared_state<F, R(ArgTypes...)>:
          task_base_shared_state<R(ArgTypes...)>
#else
        template<typename F, typename R>
        struct task_shared_state<F, R()>:
          task_base_shared_state<R()>
#endif
#else
        template<typename F, typename R>
        struct task_shared_state:
            task_base_shared_state<R>
#endif
        {
        private:
          task_shared_state(task_shared_state&);
        public:
            F f;
            task_shared_state(F const& f_):
                f(f_)
            {}
            task_shared_state(AUTOBOOST_THREAD_RV_REF(F) f_):
              f(autoboost::move(f_))
            {}

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            void do_apply(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
            {
                try
                {
                    this->set_value_autoboostat_thread_exit(f(autoboost::move(args)...));
                }
#else
            void do_apply()
            {
                try
                {
                    this->set_value_autoboostat_thread_exit(f());
                }
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                catch(thread_interrupted& )
                {
                    this->set_interrupted_autoboostat_thread_exit();
                }
#endif
                catch(...)
                {
                    this->set_exception_autoboostat_thread_exit(current_exception());
                }
            }

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            void do_run(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
            {
                try
                {
                    this->mark_finished_with_result(f(autoboost::move(args)...));
                }
#else
            void do_run()
            {
                try
                {
#if ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
                  R res((f()));
                  this->mark_finished_with_result(autoboost::move(res));
#else
                  this->mark_finished_with_result(f());
#endif
                  }
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                catch(thread_interrupted& )
                {
                    this->mark_interrupted_finish();
                }
#endif
                catch(...)
                {
                    this->mark_exceptional_finish();
                }
            }
        };

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
        template<typename F, typename R, typename ...ArgTypes>
        struct task_shared_state<F, R&(ArgTypes...)>:
          task_base_shared_state<R&(ArgTypes...)>
#else
        template<typename F, typename R>
        struct task_shared_state<F, R&()>:
          task_base_shared_state<R&()>
#endif
#else
        template<typename F, typename R>
        struct task_shared_state<F,R&>:
            task_base_shared_state<R&>
#endif
        {
        private:
          task_shared_state(task_shared_state&);
        public:
            F f;
            task_shared_state(F const& f_):
                f(f_)
            {}
            task_shared_state(AUTOBOOST_THREAD_RV_REF(F) f_):
                f(autoboost::move(f_))
            {}

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            void do_apply(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
            {
                try
                {
                    this->set_value_autoboostat_thread_exit(f(autoboost::move(args)...));
                }
#else
            void do_apply()
            {
                try
                {
                    this->set_value_autoboostat_thread_exit(f());
                }
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                catch(thread_interrupted& )
                {
                    this->set_interrupted_autoboostat_thread_exit();
                }
#endif
                catch(...)
                {
                    this->set_exception_autoboostat_thread_exit(current_exception());
                }
            }

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            void do_run(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
            {
                try
                {
                    this->mark_finished_with_result(f(autoboost::move(args)...));
                }
#else
            void do_run()
            {
                try
                {
                  R& res((f()));
                  this->mark_finished_with_result(res);
                }
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                catch(thread_interrupted& )
                {
                    this->mark_interrupted_finish();
                }
#endif
                catch(...)
                {
                    this->mark_exceptional_finish();
                }
            }
        };

#if defined(AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR)

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
        template<typename R, typename ...ArgTypes>
        struct task_shared_state<R (*)(ArgTypes...), R(ArgTypes...)>:
          task_base_shared_state<R(ArgTypes...)>
#else
        template<typename R>
        struct task_shared_state<R (*)(), R()>:
          task_base_shared_state<R()>
#endif
#else
        template<typename R>
        struct task_shared_state<R (*)(), R> :
           task_base_shared_state<R>
#endif
            {
            private:
              task_shared_state(task_shared_state&);
            public:
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
                R (*f)(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... );
                task_shared_state(R (*f_)(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... )):
                    f(f_)
                {}
#else
                R (*f)();
                task_shared_state(R (*f_)()):
                    f(f_)
                {}
#endif

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
                void do_apply(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
                {
                    try
                    {
                        this->set_value_autoboostat_thread_exit(f(autoboost::move(args)...));
                    }
#else
                void do_apply()
                {
                    try
                    {
                        R r((f()));
                        this->set_value_autoboostat_thread_exit(autoboost::move(r));
                    }
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                    catch(thread_interrupted& )
                    {
                        this->set_interrupted_autoboostat_thread_exit();
                    }
#endif
                    catch(...)
                    {
                        this->set_exception_autoboostat_thread_exit(current_exception());
                    }
                }


#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
                void do_run(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
                {
                    try
                    {
                        this->mark_finished_with_result(f(autoboost::move(args)...));
                    }
#else
                void do_run()
                {
                    try
                    {
                        R res((f()));
                        this->mark_finished_with_result(autoboost::move(res));
                    }
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                    catch(thread_interrupted& )
                    {
                        this->mark_interrupted_finish();
                    }
#endif
                    catch(...)
                    {
                        this->mark_exceptional_finish();
                    }
                }
            };
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
        template<typename R, typename ...ArgTypes>
        struct task_shared_state<R& (*)(ArgTypes...), R&(ArgTypes...)>:
          task_base_shared_state<R&(ArgTypes...)>
#else
        template<typename R>
        struct task_shared_state<R& (*)(), R&()>:
          task_base_shared_state<R&()>
#endif
#else
        template<typename R>
        struct task_shared_state<R& (*)(), R&> :
           task_base_shared_state<R&>
#endif
            {
            private:
              task_shared_state(task_shared_state&);
            public:
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
                R& (*f)(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... );
                task_shared_state(R& (*f_)(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... )):
                    f(f_)
                {}
#else
                R& (*f)();
                task_shared_state(R& (*f_)()):
                    f(f_)
                {}
#endif

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
                void do_apply(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
                {
                    try
                    {
                        this->set_value_autoboostat_thread_exit(f(autoboost::move(args)...));
                    }
#else
                void do_apply()
                {
                    try
                    {
                      this->set_value_autoboostat_thread_exit(f());
                    }
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                    catch(thread_interrupted& )
                    {
                        this->set_interrupted_autoboostat_thread_exit();
                    }
#endif
                    catch(...)
                    {
                        this->set_exception_autoboostat_thread_exit(current_exception());
                    }
                }


#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
                void do_run(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
                {
                    try
                    {
                        this->mark_finished_with_result(f(autoboost::move(args)...));
                    }
#else
                void do_run()
                {
                    try
                    {
                        this->mark_finished_with_result(f());
                    }
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                    catch(thread_interrupted& )
                    {
                        this->mark_interrupted_finish();
                    }
#endif
                    catch(...)
                    {
                        this->mark_exceptional_finish();
                    }
                }
            };
#endif
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
        template<typename F, typename ...ArgTypes>
        struct task_shared_state<F, void(ArgTypes...)>:
          task_base_shared_state<void(ArgTypes...)>
#else
        template<typename F>
        struct task_shared_state<F, void()>:
          task_base_shared_state<void()>
#endif
#else
        template<typename F>
        struct task_shared_state<F,void>:
          task_base_shared_state<void>
#endif
        {
        private:
          task_shared_state(task_shared_state&);
        public:
            F f;
            task_shared_state(F const& f_):
                f(f_)
            {}
            task_shared_state(AUTOBOOST_THREAD_RV_REF(F) f_):
                f(autoboost::move(f_))
            {}

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            void do_apply(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
            {
              try
              {
                f(autoboost::move(args)...);
#else
            void do_apply()
            {
                try
                {
                    f();
#endif
                  this->set_value_autoboostat_thread_exit();
                }
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                catch(thread_interrupted& )
                {
                    this->set_interrupted_autoboostat_thread_exit();
                }
#endif
                catch(...)
                {
                    this->set_exception_autoboostat_thread_exit(current_exception());
                }
            }

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            void do_run(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
            {
                try
                {
                    f(autoboost::move(args)...);
#else
            void do_run()
            {
                try
                {
                    f();
#endif
                    this->mark_finished_with_result();
                }
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                catch(thread_interrupted& )
                {
                    this->mark_interrupted_finish();
                }
#endif
                catch(...)
                {
                    this->mark_exceptional_finish();
                }
            }
        };

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
        template<typename ...ArgTypes>
        struct task_shared_state<void (*)(ArgTypes...), void(ArgTypes...)>:
        task_base_shared_state<void(ArgTypes...)>
#else
        template<>
        struct task_shared_state<void (*)(), void()>:
        task_base_shared_state<void()>
#endif
#else
        template<>
        struct task_shared_state<void (*)(),void>:
          task_base_shared_state<void>
#endif
        {
        private:
          task_shared_state(task_shared_state&);
        public:
            void (*f)();
            task_shared_state(void (*f_)()):
                f(f_)
            {}

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            void do_apply(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
            {
                try
                {
                    f(autoboost::move(args)...);
#else
            void do_apply()
            {
                try
                {
                    f();
#endif
                    this->set_value_autoboostat_thread_exit();
                }
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                catch(thread_interrupted& )
                {
                    this->set_interrupted_autoboostat_thread_exit();
                }
#endif
                catch(...)
                {
                    this->set_exception_autoboostat_thread_exit(current_exception());
                }
            }

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            void do_run(AUTOBOOST_THREAD_RV_REF(ArgTypes) ... args)
            {
                try
                {
                    f(autoboost::move(args)...);
#else
            void do_run()
            {
                try
                {
                  f();
#endif
                  this->mark_finished_with_result();
                }
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
                catch(thread_interrupted& )
                {
                    this->mark_interrupted_finish();
                }
#endif
                catch(...)
                {
                    this->mark_exceptional_finish();
                }
            }
        };
    }

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
  #if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
    template<typename R, typename ...ArgTypes>
    class packaged_task<R(ArgTypes...)>
    {
      typedef autoboost::shared_ptr<detail::task_base_shared_state<R(ArgTypes...)> > task_ptr;
      autoboost::shared_ptr<detail::task_base_shared_state<R(ArgTypes...)> > task;
  #else
    template<typename R>
    class packaged_task<R()>
    {
      typedef autoboost::shared_ptr<detail::task_base_shared_state<R()> > task_ptr;
      autoboost::shared_ptr<detail::task_base_shared_state<R()> > task;
  #endif
#else
    template<typename R>
    class packaged_task
    {
      typedef autoboost::shared_ptr<detail::task_base_shared_state<R> > task_ptr;
      autoboost::shared_ptr<detail::task_base_shared_state<R> > task;
#endif
        bool future_obtained;
        struct dummy;

    public:
        typedef R result_type;
        AUTOBOOST_THREAD_MOVABLE_ONLY(packaged_task)

        packaged_task():
            future_obtained(false)
        {}

        // construction and destruction
#if defined(AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR)

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
  #if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
        explicit packaged_task(R(*f)(), AUTOBOOST_THREAD_FWD_REF(ArgTypes)... args)
        {
            typedef R(*FR)(AUTOBOOST_THREAD_FWD_REF(ArgTypes)...);
            typedef detail::task_shared_state<FR,R(ArgTypes...)> task_shared_state_type;
            task= task_ptr(new task_shared_state_type(f, autoboost::move(args)...));
            future_obtained=false;
        }
  #else
        explicit packaged_task(R(*f)())
        {
            typedef R(*FR)();
            typedef detail::task_shared_state<FR,R()> task_shared_state_type;
            task= task_ptr(new task_shared_state_type(f));
            future_obtained=false;
        }
  #endif
#else
        explicit packaged_task(R(*f)())
        {
              typedef R(*FR)();
            typedef detail::task_shared_state<FR,R> task_shared_state_type;
            task= task_ptr(new task_shared_state_type(f));
            future_obtained=false;
        }
#endif
#endif
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
        template <class F>
        explicit packaged_task(AUTOBOOST_THREAD_FWD_REF(F) f
            , typename autoboost::disable_if<is_same<typename decay<F>::type, packaged_task>, dummy* >::type=0
            )
        {
          typedef typename decay<F>::type FR;
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
  #if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            typedef detail::task_shared_state<FR,R(ArgTypes...)> task_shared_state_type;
  #else
            typedef detail::task_shared_state<FR,R()> task_shared_state_type;
  #endif
#else
            typedef detail::task_shared_state<FR,R> task_shared_state_type;
#endif
            task = task_ptr(new task_shared_state_type(autoboost::forward<F>(f)));
            future_obtained = false;

        }

#else
        template <class F>
        explicit packaged_task(F const& f
            , typename autoboost::disable_if<is_same<typename decay<F>::type, packaged_task>, dummy* >::type=0
            )
        {
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
  #if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            typedef detail::task_shared_state<F,R(ArgTypes...)> task_shared_state_type;
  #else
            typedef detail::task_shared_state<F,R()> task_shared_state_type;
  #endif
#else
            typedef detail::task_shared_state<F,R> task_shared_state_type;
#endif
            task = task_ptr(new task_shared_state_type(f));
            future_obtained=false;
        }
        template <class F>
        explicit packaged_task(AUTOBOOST_THREAD_RV_REF(F) f)
        {
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
            typedef detail::task_shared_state<F,R(ArgTypes...)> task_shared_state_type;
            task = task_ptr(new task_shared_state_type(autoboost::move(f)));
#else
            typedef detail::task_shared_state<F,R()> task_shared_state_type;
            task = task_ptr(new task_shared_state_type(autoboost::move(f)));
#endif
#else
            typedef detail::task_shared_state<F,R> task_shared_state_type;
            task = task_ptr(new task_shared_state_type(autoboost::move(f)));
#endif
            future_obtained=false;

        }
#endif

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CTOR_ALLOCATORS
#if defined(AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR)
        template <class Allocator>
        packaged_task(autoboost::allocator_arg_t, Allocator a, R(*f)())
        {
          typedef R(*FR)();
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
  #if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          typedef detail::task_shared_state<FR,R(ArgTypes...)> task_shared_state_type;
  #else
          typedef detail::task_shared_state<FR,R()> task_shared_state_type;
  #endif
#else
          typedef detail::task_shared_state<FR,R> task_shared_state_type;
#endif
          typedef typename Allocator::template rebind<task_shared_state_type>::other A2;
          A2 a2(a);
          typedef thread_detail::allocator_destructor<A2> D;

          task = task_ptr(::new(a2.allocate(1)) task_shared_state_type(f), D(a2, 1) );
          future_obtained = false;
        }
#endif // AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR

#if ! defined AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
        template <class F, class Allocator>
        packaged_task(autoboost::allocator_arg_t, Allocator a, AUTOBOOST_THREAD_FWD_REF(F) f)
        {
          typedef typename decay<F>::type FR;

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
  #if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          typedef detail::task_shared_state<FR,R(ArgTypes...)> task_shared_state_type;
  #else
          typedef detail::task_shared_state<FR,R()> task_shared_state_type;
  #endif
#else
          typedef detail::task_shared_state<FR,R> task_shared_state_type;
#endif
          typedef typename Allocator::template rebind<task_shared_state_type>::other A2;
          A2 a2(a);
          typedef thread_detail::allocator_destructor<A2> D;

          task = task_ptr(::new(a2.allocate(1)) task_shared_state_type(autoboost::forward<F>(f)), D(a2, 1) );
          future_obtained = false;
        }
#else // ! defined AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
        template <class F, class Allocator>
        packaged_task(autoboost::allocator_arg_t, Allocator a, const F& f)
        {
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
  #if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          typedef detail::task_shared_state<F,R(ArgTypes...)> task_shared_state_type;
  #else
          typedef detail::task_shared_state<F,R()> task_shared_state_type;
  #endif
#else
          typedef detail::task_shared_state<F,R> task_shared_state_type;
#endif
          typedef typename Allocator::template rebind<task_shared_state_type>::other A2;
          A2 a2(a);
          typedef thread_detail::allocator_destructor<A2> D;

          task = task_ptr(::new(a2.allocate(1)) task_shared_state_type(f), D(a2, 1) );
          future_obtained = false;
        }
        template <class F, class Allocator>
        packaged_task(autoboost::allocator_arg_t, Allocator a, AUTOBOOST_THREAD_RV_REF(F) f)
        {
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
  #if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          typedef detail::task_shared_state<F,R(ArgTypes...)> task_shared_state_type;
  #else
          typedef detail::task_shared_state<F,R()> task_shared_state_type;
  #endif
#else
          typedef detail::task_shared_state<F,R> task_shared_state_type;
#endif
          typedef typename Allocator::template rebind<task_shared_state_type>::other A2;
          A2 a2(a);
          typedef thread_detail::allocator_destructor<A2> D;

#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          task = task_ptr(::new(a2.allocate(1)) task_shared_state_type(autoboost::move(f)), D(a2, 1) );
#else
          task = task_ptr(::new(a2.allocate(1)) task_shared_state_type(autoboost::move(f)), D(a2, 1) );
#endif
          future_obtained = false;
        }

#endif //AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#endif // AUTOBOOST_THREAD_PROVIDES_FUTURE_CTOR_ALLOCATORS

        ~packaged_task() {
            if(task) {
                task->owner_destroyed();
            }
        }

        // assignment
        packaged_task(AUTOBOOST_THREAD_RV_REF(packaged_task) other) AUTOBOOST_NOEXCEPT
        : future_obtained(AUTOBOOST_THREAD_RV(other).future_obtained) {
            task.swap(AUTOBOOST_THREAD_RV(other).task);
            AUTOBOOST_THREAD_RV(other).future_obtained=false;
        }
        packaged_task& operator=(AUTOBOOST_THREAD_RV_REF(packaged_task) other) AUTOBOOST_NOEXCEPT {

#if ! defined  AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
            packaged_task temp(autoboost::move(other));
#else
            packaged_task temp(static_cast<AUTOBOOST_THREAD_RV_REF(packaged_task)>(other));
#endif
            swap(temp);
            return *this;
        }

        void reset() {
            if (!valid())
                throw future_error(system::make_error_code(future_errc::no_state));
            task->reset();
            future_obtained=false;
        }

        void swap(packaged_task& other) AUTOBOOST_NOEXCEPT {
            task.swap(other.task);
            std::swap(future_obtained,other.future_obtained);
        }
        bool valid() const AUTOBOOST_NOEXCEPT {
          return task.get()!=0;
        }

        // result retrieval
        AUTOBOOST_THREAD_FUTURE<R> get_future() {
            if(!task) {
                autoboost::throw_exception(task_moved());
            } else if(!future_obtained) {
                future_obtained=true;
                return AUTOBOOST_THREAD_FUTURE<R>(task);
            } else {
                autoboost::throw_exception(future_already_retrieved());
            }
        }

        // execution
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
        void operator()(AUTOBOOST_THREAD_RV_REF(ArgTypes)... args) {
            if(!task) {
                autoboost::throw_exception(task_moved());
            }
            task->run(autoboost::move(args)...);
        }
        void make_ready_autoboostat_thread_exit(ArgTypes... args) {
          if(!task) {
              autoboost::throw_exception(task_moved());
          }
          if (task->has_value()) {
                autoboost::throw_exception(promise_already_satisfied());
          }
          task->apply(autoboost::move(args)...);
        }
#else
        void operator()() {
            if(!task) {
                autoboost::throw_exception(task_moved());
            }
            task->run();
        }
        void make_ready_autoboostat_thread_exit() {
          if(!task) {
              autoboost::throw_exception(task_moved());
          }
          if (task->has_value()) autoboost::throw_exception(promise_already_satisfied());
          task->apply();
        }
#endif
        template<typename F>
        void set_wait_callback(F f) {
            task->set_wait_callback(f,this);
        }
    };
}
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CTOR_ALLOCATORS
namespace autoboost { namespace container {
    template <class R, class Alloc>
    struct uses_allocator< ::autoboost::packaged_task<R> , Alloc> : true_type
    {};
}}
#if ! defined  AUTOBOOST_NO_CXX11_ALLOCATOR
namespace std {
    template <class R, class Alloc>
    struct uses_allocator< ::autoboost::packaged_task<R> , Alloc> : true_type
    {};
}
#endif
#endif

namespace autoboost
{
  AUTOBOOST_THREAD_DCL_MOVABLE_BEG(T) packaged_task<T> AUTOBOOST_THREAD_DCL_MOVABLE_END

namespace detail
{
  ////////////////////////////////
  // make_future_deferred_shared_state
  ////////////////////////////////
  template <class Rp, class Fp>
  AUTOBOOST_THREAD_FUTURE<Rp>
  make_future_deferred_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f) {
    shared_ptr<future_deferred_shared_state<Rp, Fp> >
        h(new future_deferred_shared_state<Rp, Fp>(autoboost::forward<Fp>(f)));
    return AUTOBOOST_THREAD_FUTURE<Rp>(h);
  }

  ////////////////////////////////
  // make_future_async_shared_state
  ////////////////////////////////
  template <class Rp, class Fp>
  AUTOBOOST_THREAD_FUTURE<Rp>
  make_future_async_shared_state(AUTOBOOST_THREAD_FWD_REF(Fp) f) {
    shared_ptr<future_async_shared_state<Rp, Fp> >
        h(new future_async_shared_state<Rp, Fp>(autoboost::forward<Fp>(f)));
    return AUTOBOOST_THREAD_FUTURE<Rp>(h);
  }
}

    ////////////////////////////////
    // template <class F, class... ArgTypes>
    // future<R> async(launch policy, F&&, ArgTypes&&...);
    ////////////////////////////////

#if defined AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR

#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
  template <class R, class... ArgTypes>
  AUTOBOOST_THREAD_FUTURE<R>
  async(launch policy, R(*f)(AUTOBOOST_THREAD_FWD_REF(ArgTypes)...), AUTOBOOST_THREAD_FWD_REF(ArgTypes)... args) {
    typedef R(*F)(AUTOBOOST_THREAD_FWD_REF(ArgTypes)...);
    typedef detail::invoker<typename decay<F>::type, typename decay<ArgTypes>::type...> BF;
    typedef typename BF::result_type Rp;

    if (underlying_cast<int>(policy) & int(launch::async)) {
      return AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::detail::make_future_async_shared_state<Rp>(
              BF(
                  f
                  , thread_detail::decay_copy(autoboost::forward<ArgTypes>(args))...
              )
          ));
    } else if (underlying_cast<int>(policy) & int(launch::deferred)) {
      return AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::detail::make_future_deferred_shared_state<Rp>(
              BF(
                  f
                  , thread_detail::decay_copy(autoboost::forward<ArgTypes>(args))...
              )
          ));
    } else {
      std::terminate();
      AUTOBOOST_THREAD_FUTURE<R> ret;
      return ::autoboost::move(ret);
    }
  }

#else // defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)

  template <class R>
  AUTOBOOST_THREAD_FUTURE<R>
  async(launch policy, R(*f)()) {
  #if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
    typedef packaged_task<R()> packaged_task_type;
  #else
    typedef packaged_task<R> packaged_task_type;
  #endif

    if (underlying_cast<int>(policy) & int(launch::async)) {
      packaged_task_type pt( f );
      AUTOBOOST_THREAD_FUTURE<R> ret = AUTOBOOST_THREAD_MAKE_RV_REF(pt.get_future());
      ret.set_async();
      autoboost::thread( autoboost::move(pt) ).detach();
      return ::autoboost::move(ret);
    } else if (underlying_cast<int>(policy) & int(launch::deferred)) {
      std::terminate();
      AUTOBOOST_THREAD_FUTURE<R> ret;
      return ::autoboost::move(ret);
    } else {
      std::terminate();
      AUTOBOOST_THREAD_FUTURE<R> ret;
      return ::autoboost::move(ret);
    }
  }
#endif
#endif // defined(AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR)

#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)

  template <class F, class ...ArgTypes>
  AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<typename decay<F>::type(
      typename decay<ArgTypes>::type...
  )>::type>
  async(launch policy, AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(ArgTypes)... args) {
    typedef typename autoboost::result_of<typename decay<F>::type(
        typename decay<ArgTypes>::type...
    )>::type R;
    typedef detail::invoker<typename decay<F>::type, typename decay<ArgTypes>::type...> BF;
    typedef typename BF::result_type Rp;

    if (underlying_cast<int>(policy) & int(launch::async)) {
      return AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::detail::make_future_async_shared_state<Rp>(
              BF(
                  thread_detail::decay_copy(autoboost::forward<F>(f))
                , thread_detail::decay_copy(autoboost::forward<ArgTypes>(args))...
              )
          ));
    } else if (underlying_cast<int>(policy) & int(launch::deferred)) {
      return AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::detail::make_future_deferred_shared_state<Rp>(
              BF(
                  thread_detail::decay_copy(autoboost::forward<F>(f))
                , thread_detail::decay_copy(autoboost::forward<ArgTypes>(args))...
              )
          ));
    } else {
      std::terminate();
      AUTOBOOST_THREAD_FUTURE<R> ret;
      return ::autoboost::move(ret);
    }
  }

#else // defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)

  template <class F>
  AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<typename decay<F>::type()>::type>
  async(launch policy, AUTOBOOST_THREAD_FWD_REF(F) f) {
    typedef typename autoboost::result_of<typename decay<F>::type()>::type R;
#if defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
    typedef packaged_task<R()> packaged_task_type;
#else // defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
    typedef packaged_task<R> packaged_task_type;
#endif // defined AUTOBOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK

    if (underlying_cast<int>(policy) & int(launch::async)) {
      packaged_task_type pt( autoboost::forward<F>(f) );
      AUTOBOOST_THREAD_FUTURE<R> ret = pt.get_future();
      ret.set_async();
      autoboost::thread( autoboost::move(pt) ).detach();
      return ::autoboost::move(ret);
    } else if (underlying_cast<int>(policy) & int(launch::deferred)) {
      std::terminate();
      AUTOBOOST_THREAD_FUTURE<R> ret;
      return ::autoboost::move(ret);
      //          return autoboost::detail::make_future_deferred_shared_state<Rp>(
      //              BF(
      //                  thread_detail::decay_copy(autoboost::forward<F>(f))
      //              )
      //          );
    } else {
      std::terminate();
      AUTOBOOST_THREAD_FUTURE<R> ret;
      return ::autoboost::move(ret);
    }
  }
#endif // defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)

#ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS
namespace detail {
    /////////////////////////
    /// shared_state_nullary_task
    /////////////////////////
    template<typename Rp, typename Fp>
    struct shared_state_nullary_task
    {
      shared_state<Rp>* that;
      Fp f_;
    public:

      shared_state_nullary_task(shared_state<Rp>* st, AUTOBOOST_THREAD_FWD_REF(Fp) f)
      : that(st), f_(autoboost::move(f))
      {};
#if ! defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(shared_state_nullary_task)
      shared_state_nullary_task(shared_state_nullary_task const& x) //AUTOBOOST_NOEXCEPT
      : that(x.that), f_(x.f_)
      {}
      shared_state_nullary_task& operator=(AUTOBOOST_THREAD_COPY_ASSIGN_REF(shared_state_nullary_task) x) //AUTOBOOST_NOEXCEPT
      {
        if (this != &x) {
          that=x.that;
          f_=x.f_;
        }
        return *this;
      }
      // move
      shared_state_nullary_task(AUTOBOOST_THREAD_RV_REF(shared_state_nullary_task) x) //AUTOBOOST_NOEXCEPT
      : that(x.that), f_(autoboost::move(x.f_))
      {
        x.that=0;
      }
      shared_state_nullary_task& operator=(AUTOBOOST_THREAD_RV_REF(shared_state_nullary_task) x) //AUTOBOOST_NOEXCEPT
      {
        if (this != &x) {
          that=x.that;
          f_=autoboost::move(x.f_);
          x.that=0;
        }
        return *this;
      }
#endif
      void operator()() {
        try {
          that->mark_finished_with_result(f_());
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
        } catch(thread_interrupted& ) {
          that->mark_interrupted_finish();
#endif // defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
        } catch(...) {
          that->mark_exceptional_finish();
        }
      }
    };

    template<typename Fp>
    struct shared_state_nullary_task<void, Fp>
    {
      shared_state<void>* that;
      Fp f_;
    public:
      shared_state_nullary_task(shared_state<void>* st, AUTOBOOST_THREAD_FWD_REF(Fp) f)
      : that(st), f_(autoboost::move(f))
      {};
#if ! defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(shared_state_nullary_task)
      shared_state_nullary_task(shared_state_nullary_task const& x) //AUTOBOOST_NOEXCEPT
      : that(x.that), f_(x.f_)
      {}
      shared_state_nullary_task& operator=(AUTOBOOST_THREAD_COPY_ASSIGN_REF(shared_state_nullary_task) x) //AUTOBOOST_NOEXCEPT
      {
        if (this != &x) {
          that=x.that;
          f_=x.f_;
        }
        return *this;
      }
      // move
      shared_state_nullary_task(AUTOBOOST_THREAD_RV_REF(shared_state_nullary_task) x) AUTOBOOST_NOEXCEPT
      : that(x.that), f_(autoboost::move(x.f_))
      {
        x.that=0;
      }
      shared_state_nullary_task& operator=(AUTOBOOST_THREAD_RV_REF(shared_state_nullary_task) x) AUTOBOOST_NOEXCEPT {
        if (this != &x) {
          that=x.that;
          f_=autoboost::move(x.f_);
          x.that=0;
        }
        return *this;
      }
#endif
      void operator()() {
        try {
          f_();
          that->mark_finished_with_result();
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
        } catch(thread_interrupted& ) {
          that->mark_interrupted_finish();
#endif // defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
        } catch(...) {
          that->mark_exceptional_finish();
        }
      }
    };

    template<typename Rp, typename Fp>
    struct shared_state_nullary_task<Rp&, Fp>
    {
      shared_state<Rp&>* that;
      Fp f_;
    public:
      shared_state_nullary_task(shared_state<Rp&>* st, AUTOBOOST_THREAD_FWD_REF(Fp) f)
        : that(st), f_(autoboost::move(f))
      {}
#if ! defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
      AUTOBOOST_THREAD_COPYABLE_AND_MOVABLE(shared_state_nullary_task)
      shared_state_nullary_task(shared_state_nullary_task const& x) AUTOBOOST_NOEXCEPT
      : that(x.that), f_(x.f_) {}

      shared_state_nullary_task& operator=(AUTOBOOST_THREAD_COPY_ASSIGN_REF(shared_state_nullary_task) x) AUTOBOOST_NOEXCEPT {
        if (this != &x){
          that=x.that;
          f_=x.f_;
        }
        return *this;
      }
      // move
      shared_state_nullary_task(AUTOBOOST_THREAD_RV_REF(shared_state_nullary_task) x) AUTOBOOST_NOEXCEPT
      : that(x.that), f_(autoboost::move(x.f_))
      {
        x.that=0;
      }
      shared_state_nullary_task& operator=(AUTOBOOST_THREAD_RV_REF(shared_state_nullary_task) x) AUTOBOOST_NOEXCEPT {
        if (this != &x) {
          that=x.that;
          f_=autoboost::move(x.f_);
          x.that=0;
        }
        return *this;
      }
#endif
      void operator()() {
        try {
          that->mark_finished_with_result(f_());
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
        } catch(thread_interrupted& ) {
          that->mark_interrupted_finish();
#endif // defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
        } catch(...) {
          that->mark_exceptional_finish();
        }
      }
    };

    /////////////////////////
    /// future_executor_shared_state_base
    /////////////////////////
    template<typename Rp, typename Executor>
    struct future_executor_shared_state: shared_state<Rp>
    {
      typedef shared_state<Rp> base_type;
    protected:
    public:
      template<typename Fp>
      future_executor_shared_state(Executor& ex, AUTOBOOST_THREAD_FWD_REF(Fp) f) {
        this->set_executor();
        shared_state_nullary_task<Rp,Fp> t(this, autoboost::forward<Fp>(f));
        ex.submit(autoboost::move(t));
      }

      ~future_executor_shared_state() {
        this->wait(false);
      }
    };

    ////////////////////////////////
    // make_future_executor_shared_state
    ////////////////////////////////
    template <class Rp, class Fp, class Executor>
    AUTOBOOST_THREAD_FUTURE<Rp>
    make_future_executor_shared_state(Executor& ex, AUTOBOOST_THREAD_FWD_REF(Fp) f) {
      shared_ptr<future_executor_shared_state<Rp, Executor> >
          h(new future_executor_shared_state<Rp, Executor>(ex, autoboost::forward<Fp>(f)));
      return AUTOBOOST_THREAD_FUTURE<Rp>(h);
    }

} // detail

    ////////////////////////////////
    // template <class Executor, class F, class... ArgTypes>
    // future<R> async(Executor& ex, F&&, ArgTypes&&...);
    ////////////////////////////////

//#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#if defined(AUTOBOOST_THREAD_PROVIDES_INVOKE) && ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES) && ! defined(AUTOBOOST_NO_CXX11_HDR_TUPLE)

#if defined AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR

  template <class Executor, class R, class... ArgTypes>
  AUTOBOOST_THREAD_FUTURE<R>
  async(Executor& ex, R(*f)(AUTOBOOST_THREAD_FWD_REF(ArgTypes)...), AUTOBOOST_THREAD_FWD_REF(ArgTypes)... args) {
    typedef R(*F)(AUTOBOOST_THREAD_FWD_REF(ArgTypes)...);
    typedef detail::invoker<typename decay<F>::type, typename decay<ArgTypes>::type...> BF;
    typedef typename BF::result_type Rp;

    return AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::detail::make_future_executor_shared_state<Rp>(ex,
        BF(
            f
            , thread_detail::decay_copy(autoboost::forward<ArgTypes>(args))...
        )
    ));
  }
#endif // defined AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR

  template <class Executor, class F, class ...ArgTypes>
  AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<typename decay<F>::type(
      typename decay<ArgTypes>::type...
  )>::type>
  async(Executor& ex, AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(ArgTypes)... args) {
    typedef detail::invoker<typename decay<F>::type, typename decay<ArgTypes>::type...> BF;
    typedef typename BF::result_type Rp;

    return AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::detail::make_future_executor_shared_state<Rp>(ex,
        BF(
            thread_detail::decay_copy(autoboost::forward<F>(f))
            , thread_detail::decay_copy(autoboost::forward<ArgTypes>(args))...
        )
    ));
  }

#else // ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#if defined AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR

  template <class Executor, class R>
  AUTOBOOST_THREAD_FUTURE<R>
  async(Executor& ex, R(*f)()) {
    typedef R(*F)();
    typedef detail::invoker<F> BF;
    typedef typename BF::result_type Rp;

    return AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::detail::make_future_executor_shared_state<Rp>(ex,
        BF(
            f
        )
    ));
  }

  template <class Executor, class R, class A1>
  AUTOBOOST_THREAD_FUTURE<R>
  async(Executor& ex, R(*f)(AUTOBOOST_THREAD_FWD_REF(A1)), AUTOBOOST_THREAD_FWD_REF(A1) a1) {
    typedef R(*F)(AUTOBOOST_THREAD_FWD_REF(A1));
    typedef detail::invoker<F, typename decay<A1>::type> BF;
    typedef typename BF::result_type Rp;

    return AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::detail::make_future_executor_shared_state<Rp>(ex,
        BF(
            f
            , thread_detail::decay_copy(autoboost::forward<A1>(a1))
        )
    ));
  }
#endif // defined AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR

  template <class Executor, class F>
  AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<typename decay<F>::type()>::type>
  async(Executor& ex, AUTOBOOST_THREAD_FWD_REF(F) f)  {
    typedef detail::invoker<typename decay<F>::type> BF;
    typedef typename BF::result_type Rp;

    return autoboost::detail::make_future_executor_shared_state<Rp>(ex,
        BF(
            thread_detail::decay_copy(autoboost::forward<F>(f))
        )
    );
  }

  template <class Executor, class F, class A1>
  AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<typename decay<F>::type(
      typename decay<A1>::type
  )>::type>
  async(Executor& ex, AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(A1) a1) {
    typedef detail::invoker<typename decay<F>::type, typename decay<A1>::type> BF;
    typedef typename BF::result_type Rp;

    return AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::detail::make_future_executor_shared_state<Rp>(ex,
        BF(
            thread_detail::decay_copy(autoboost::forward<F>(f))
          , thread_detail::decay_copy(autoboost::forward<A1>(a1))
        )
    ));
  }

  template <class Executor, class F, class A1, class A2>
  AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<typename decay<F>::type(
      typename decay<A1>::type, typename decay<A2>::type
  )>::type>
  async(Executor& ex, AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(A1) a1, AUTOBOOST_THREAD_FWD_REF(A2) a2) {
    typedef detail::invoker<typename decay<F>::type, typename decay<A1>::type, typename decay<A2>::type> BF;
    typedef typename BF::result_type Rp;

    return AUTOBOOST_THREAD_MAKE_RV_REF(autoboost::detail::make_future_executor_shared_state<Rp>(ex,
        BF(
            thread_detail::decay_copy(autoboost::forward<F>(f))
          , thread_detail::decay_copy(autoboost::forward<A1>(a1))
          , thread_detail::decay_copy(autoboost::forward<A2>(a2))
        )
    ));
  }

#endif //! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
#endif

  ////////////////////////////////
  // template <class F, class... ArgTypes>
  // future<R> async(F&&, ArgTypes&&...);
  ////////////////////////////////

#if defined AUTOBOOST_THREAD_RVALUE_REFERENCES_DONT_MATCH_FUNTION_PTR
  #if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
  template <class R, class... ArgTypes>
  AUTOBOOST_THREAD_FUTURE<R>
  async(R(*f)(AUTOBOOST_THREAD_FWD_REF(ArgTypes)...), AUTOBOOST_THREAD_FWD_REF(ArgTypes)... args) {
    return AUTOBOOST_THREAD_MAKE_RV_REF(async(launch(launch::any), f, autoboost::forward<ArgTypes>(args)...));
  }
  #else
  template <class R>
  AUTOBOOST_THREAD_FUTURE<R>
  async(R(*f)()) {
    return AUTOBOOST_THREAD_MAKE_RV_REF(async(launch(launch::any), f));
  }
  #endif
#endif

#if defined(AUTOBOOST_THREAD_PROVIDES_VARIADIC_THREAD)
  template <class F, class ...ArgTypes>
  AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<typename decay<F>::type(
      typename decay<ArgTypes>::type...
  )>::type>
  async(AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(ArgTypes)... args) {
      return AUTOBOOST_THREAD_MAKE_RV_REF(async(launch(launch::any), autoboost::forward<F>(f), autoboost::forward<ArgTypes>(args)...));
  }
#else
  template <class F>
  AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F()>::type>
  async(AUTOBOOST_THREAD_FWD_REF(F) f) {
      return AUTOBOOST_THREAD_MAKE_RV_REF(async(launch(launch::any), autoboost::forward<F>(f)));
  }
#endif

  ////////////////////////////////
  // make_future deprecated
  ////////////////////////////////
  template <typename T>
  AUTOBOOST_THREAD_FUTURE<typename decay<T>::type> make_future(AUTOBOOST_THREAD_FWD_REF(T) value) {
    typedef typename decay<T>::type future_value_type;
    promise<future_value_type> p;
    p.set_value(autoboost::forward<future_value_type>(value));
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future());
  }

#if defined AUTOBOOST_THREAD_USES_MOVE
  inline AUTOBOOST_THREAD_FUTURE<void> make_future() {
    promise<void> p;
    p.set_value();
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future());
  }
#endif

  ////////////////////////////////
  // make_ready_future
  ////////////////////////////////
  template <typename T>
  AUTOBOOST_THREAD_FUTURE<typename decay<T>::type> make_ready_future(AUTOBOOST_THREAD_FWD_REF(T) value) {
    typedef typename decay<T>::type future_value_type;
    promise<future_value_type> p;
    p.set_value(autoboost::forward<future_value_type>(value));
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future());
  }

  template <typename T, typename T1>
  AUTOBOOST_THREAD_FUTURE<T> make_ready_no_decay_future(T1 value) {
    typedef T future_value_type;
    promise<future_value_type> p;
    p.set_value(value);
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future());
  }

#if defined AUTOBOOST_THREAD_USES_MOVE
  inline AUTOBOOST_THREAD_FUTURE<void> make_ready_future() {
    promise<void> p;
    p.set_value();
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future());
  }
#endif

  template <typename T>
  AUTOBOOST_THREAD_FUTURE<T> make_ready_future(exception_ptr ex)  {
    promise<T> p;
    p.set_exception(ex);
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future());
  }

  template <typename T>
  AUTOBOOST_THREAD_FUTURE<T> make_exceptional_future(exception_ptr ex) {
    promise<T> p;
    p.set_exception(ex);
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future());
  }

  template <typename T, typename E>
  AUTOBOOST_THREAD_FUTURE<T> make_exceptional_future(E ex) {
    promise<T> p;
    p.set_exception(autoboost::copy_exception(ex));
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future());
  }

  template <typename T>
  AUTOBOOST_THREAD_FUTURE<T> make_exceptional_future() {
    promise<T> p;
    p.set_exception(autoboost::current_exception());
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future());
  }

  template <typename T>
  AUTOBOOST_THREAD_FUTURE<T> make_exceptional_future_if_invalid(AUTOBOOST_THREAD_FWD_REF(AUTOBOOST_THREAD_FUTURE<T>) fut) {
    fut.set_exceptional_if_invalid();
    return autoboost::move(fut);
  }
  template <typename T>
  shared_future<T> make_exceptional_future_if_invalid(shared_future<T> fut) {
    fut.set_exceptional_if_invalid();
    return fut;
  }

#if 0
  template<typename CLOSURE>
  make_future(CLOSURE closure) -> AUTOBOOST_THREAD_FUTURE<decltype(closure())> {
      typedef decltype(closure()) T;
      promise<T> p;
      try {
        p.set_value(closure());
      } catch(...) {
        p.set_exception(std::current_exception());
      }
      return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future());
  }
#endif

  ////////////////////////////////
  // make_shared_future deprecated
  ////////////////////////////////
  template <typename T>
  shared_future<typename decay<T>::type> make_shared_future(AUTOBOOST_THREAD_FWD_REF(T) value) {
    typedef typename decay<T>::type future_type;
    promise<future_type> p;
    p.set_value(autoboost::forward<T>(value));
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future().share());
  }

  inline shared_future<void> make_shared_future()  {
    promise<void> p;
    return AUTOBOOST_THREAD_MAKE_RV_REF(p.get_future().share());
  }

  ////////////////////////////////
  // detail::future_async_continuation_shared_state
  ////////////////////////////////
#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
namespace detail
{

  /////////////////////////
  /// future_async_continuation_shared_state
  /////////////////////////

  template<typename F, typename Rp, typename Fp>
  struct future_async_continuation_shared_state: future_async_shared_state_base<Rp>
  {
    F parent;
    Fp continuation;

  public:
    future_async_continuation_shared_state(AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : parent(autoboost::move(f)),
      continuation(autoboost::move(c)) {
    }

    void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
      this->thr_ = thread(&future_async_continuation_shared_state::run, this);
    }

    static void run(future_async_continuation_shared_state* that) {
      try {
        that->mark_finished_with_result(that->continuation(autoboost::move(that->parent)));
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }

    ~future_async_continuation_shared_state() {
      this->join();
    }
  };

  template<typename F, typename Fp>
  struct future_async_continuation_shared_state<F, void, Fp>: public future_async_shared_state_base<void>
  {
    F parent;
    Fp continuation;

  public:
    future_async_continuation_shared_state(AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : parent(autoboost::move(f)),
      continuation(autoboost::move(c)) {
    }

    void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
      this->thr_ = thread(&future_async_continuation_shared_state::run, this);
    }

    static void run(future_async_continuation_shared_state* that) {
      try {
        that->continuation(autoboost::move(that->parent));
        that->mark_finished_with_result();
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }

    ~future_async_continuation_shared_state() {
      this->join();
    }
  };

  /////////////////////////
  /// future_executor_continuation_shared_state
  /////////////////////////
#ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS

  template <typename Ex>
  struct run_it {
    Ex* that;

    run_it(Ex* that) : that (that) {}
    void operator()() { that->run(that); }
  };

  template<typename Ex, typename F, typename Rp, typename Fp>
  struct future_executor_continuation_shared_state: shared_state<Rp>
  {
    Ex* ex;
    F parent;
    Fp continuation;

  public:
    future_executor_continuation_shared_state(Ex& ex, AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : ex(&ex), parent(autoboost::move(f)),
      continuation(autoboost::move(c)) {
      this->set_executor();
    }

    void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
      run_it<future_executor_continuation_shared_state> fct(this);
      ex->submit(fct);
    }

    static void run(future_executor_continuation_shared_state* that) {
      try {
        that->mark_finished_with_result(that->continuation(autoboost::move(that->parent)));
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }

    ~future_executor_continuation_shared_state() {
      this->wait(false);
    }
  };

  template<typename Ex, typename F, typename Fp>
  struct future_executor_continuation_shared_state<Ex, F, void, Fp>: public shared_state<void>
  {
    Ex* ex;
    F parent;
    Fp continuation;

  public:
    future_executor_continuation_shared_state(Ex& ex, AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : ex(&ex), parent(autoboost::move(f)),
      continuation(autoboost::move(c)) {
      this->set_executor();
    }

    void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
      run_it<future_executor_continuation_shared_state> fct(this);
      ex->submit(fct);
    }

    static void run(future_executor_continuation_shared_state* that) {
      try {
        that->continuation(autoboost::move(that->parent));
        that->mark_finished_with_result();
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }

    ~future_executor_continuation_shared_state() {
      this->wait(false);
    }
  };
#endif

  /////////////////////////
  /// shared_future_async_continuation_shared_state
  /////////////////////////

  template<typename F, typename Rp, typename Fp>
  struct shared_future_async_continuation_shared_state: future_async_shared_state_base<Rp>
  {
    F parent;
    Fp continuation;

  public:
    shared_future_async_continuation_shared_state(F f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : parent(f),
      continuation(autoboost::move(c)) {
    }

    void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
      this->thr_ = thread(&shared_future_async_continuation_shared_state::run, this);
    }

    static void run(shared_future_async_continuation_shared_state* that) {
      try {
        that->mark_finished_with_result(that->continuation(that->parent));
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }

    ~shared_future_async_continuation_shared_state() {
      this->join();
    }
  };

  template<typename F, typename Fp>
  struct shared_future_async_continuation_shared_state<F, void, Fp>: public future_async_shared_state_base<void>
  {
    F parent;
    Fp continuation;

  public:
    shared_future_async_continuation_shared_state(F f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : parent(f),
      continuation(autoboost::move(c)) {
    }

    void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
      this->thr_ = thread(&shared_future_async_continuation_shared_state::run, this);
    }

    static void run(shared_future_async_continuation_shared_state* that) {
      try {
        that->continuation(that->parent);
        that->mark_finished_with_result();
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }

    ~shared_future_async_continuation_shared_state() {
      this->join();
    }
  };

  /////////////////////////
  /// shared_future_executor_continuation_shared_state
  /////////////////////////
#ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS

  template<typename Ex, typename F, typename Rp, typename Fp>
  struct shared_future_executor_continuation_shared_state: shared_state<Rp>
  {
    Ex* ex;
    F parent;
    Fp continuation;

  public:
    shared_future_executor_continuation_shared_state(Ex& ex, F f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : ex(&ex), parent(f),
      continuation(autoboost::move(c)) {
      this->set_executor();
    }

    void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
      run_it<shared_future_executor_continuation_shared_state> fct(this);
      ex->submit(fct);
    }

    static void run(shared_future_executor_continuation_shared_state* that) {
      try {
        that->mark_finished_with_result(that->continuation(that->parent));
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }

    ~shared_future_executor_continuation_shared_state() {
      this->wait(false);
    }
  };

  template<typename Ex, typename F, typename Fp>
  struct shared_future_executor_continuation_shared_state<Ex, F, void, Fp>: public shared_state<void>
  {
    Ex* ex;
    F parent;
    Fp continuation;

  public:
    shared_future_executor_continuation_shared_state(Ex& ex, F f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : ex(&ex), parent(f),
      continuation(autoboost::move(c)) {
    }

    void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
      run_it<shared_future_executor_continuation_shared_state> fct(this);
      ex->submit(fct);
    }

    static void run(shared_future_executor_continuation_shared_state* that) {
      try {
        that->continuation(that->parent);
        that->mark_finished_with_result();
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }

    ~shared_future_executor_continuation_shared_state() {
      this->wait(false);
    }
  };
#endif
  //////////////////////////
  /// future_deferred_continuation_shared_state
  //////////////////////////
  template<typename F, typename Rp, typename Fp>
  struct future_deferred_continuation_shared_state: shared_state<Rp>
  {
    F parent;
    Fp continuation;

  public:
    future_deferred_continuation_shared_state(AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : parent(autoboost::move(f)),
      continuation(autoboost::move(c)) {
      this->set_deferred();
    }

    virtual void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
    }

    virtual void execute(autoboost::unique_lock<autoboost::mutex>& lck) {
      try {
        Fp local_fuct=autoboost::move(continuation);
        F ftmp = autoboost::move(parent);
        relocker relock(lck);
        Rp res = local_fuct(autoboost::move(ftmp));
        relock.lock();
        this->mark_finished_with_result_internal(autoboost::move(res), lck);
      } catch (...) {
        this->mark_exceptional_finish_internal(current_exception(), lck);
      }
    }
  };

  template<typename F, typename Fp>
  struct future_deferred_continuation_shared_state<F,void,Fp>: shared_state<void>
  {
    F parent;
    Fp continuation;

  public:
    future_deferred_continuation_shared_state(AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : parent(autoboost::move(f)),
      continuation(autoboost::move(c)) {
      this->set_deferred();
    }

    virtual void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
    }

    virtual void execute(autoboost::unique_lock<autoboost::mutex>& lck) {
      try {
        Fp local_fuct=autoboost::move(continuation);
        F ftmp = autoboost::move(parent);
        relocker relock(lck);
        local_fuct(autoboost::move(ftmp));
        relock.lock();
        this->mark_finished_with_result_internal(lck);
      } catch (...) {
        this->mark_exceptional_finish_internal(current_exception(), lck);
      }
    }
  };

  //////////////////////////
  /// shared_future_deferred_continuation_shared_state
  //////////////////////////
  template<typename F, typename Rp, typename Fp>
  struct shared_future_deferred_continuation_shared_state: shared_state<Rp>
  {
    F parent;
    Fp continuation;

  public:
    shared_future_deferred_continuation_shared_state(F f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : parent(f),
      continuation(autoboost::move(c)) {
      this->set_deferred();
    }

    virtual void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
    }

    virtual void execute(autoboost::unique_lock<autoboost::mutex>& lck) {
      try {
        Fp local_fuct=autoboost::move(continuation);
        F ftmp = parent;
        relocker relock(lck);
        Rp res = local_fuct(ftmp);
        relock.lock();
        this->mark_finished_with_result_internal(autoboost::move(res), lck);
      } catch (...) {
        this->mark_exceptional_finish_internal(current_exception(), lck);
      }
    }
  };

  template<typename F, typename Fp>
  struct shared_future_deferred_continuation_shared_state<F,void,Fp>: shared_state<void>
  {
    F parent;
    Fp continuation;

  public:
    shared_future_deferred_continuation_shared_state(F f, AUTOBOOST_THREAD_FWD_REF(Fp) c)
    : parent(f),
      continuation(autoboost::move(c)) {
      this->set_deferred();
    }

    virtual void launch_continuation(autoboost::unique_lock<autoboost::mutex>& ) {
    }

    virtual void execute(autoboost::unique_lock<autoboost::mutex>& lck) {
      try {
        Fp local_fuct=autoboost::move(continuation);
        F ftmp = parent;
        relocker relock(lck);
        local_fuct(ftmp);
        relock.lock();
        this->mark_finished_with_result_internal(lck);
      } catch (...) {
        this->mark_exceptional_finish_internal(current_exception(), lck);
      }
    }
  };

  ////////////////////////////////
  // make_future_deferred_continuation_shared_state
  ////////////////////////////////
  template<typename F, typename Rp, typename Fp>
  AUTOBOOST_THREAD_FUTURE<Rp>
  make_future_deferred_continuation_shared_state(
      autoboost::unique_lock<autoboost::mutex> &lock,
      AUTOBOOST_THREAD_RV_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fp) c) {
    shared_ptr<future_deferred_continuation_shared_state<F, Rp, Fp> >
        h(new future_deferred_continuation_shared_state<F, Rp, Fp>(autoboost::move(f), autoboost::forward<Fp>(c)));
    h->parent.future_->set_continuation_ptr(h, lock);
    return AUTOBOOST_THREAD_FUTURE<Rp>(h);
  }

  ////////////////////////////////
  // make_future_async_continuation_shared_state
  ////////////////////////////////
  template<typename F, typename Rp, typename Fp>
  AUTOBOOST_THREAD_FUTURE<Rp>
  make_future_async_continuation_shared_state(
      autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f,
      AUTOBOOST_THREAD_FWD_REF(Fp) c) {
    shared_ptr<future_async_continuation_shared_state<F,Rp, Fp> >
        h(new future_async_continuation_shared_state<F,Rp, Fp>(autoboost::move(f), autoboost::forward<Fp>(c)));
    h->parent.future_->set_continuation_ptr(h, lock);

    return AUTOBOOST_THREAD_FUTURE<Rp>(h);
  }

  ////////////////////////////////
  // make_future_executor_continuation_shared_state
  ////////////////////////////////
#ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS

  template<typename Ex, typename F, typename Rp, typename Fp>
  AUTOBOOST_THREAD_FUTURE<Rp>
  make_future_executor_continuation_shared_state(Ex& ex,
      autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f,
      AUTOBOOST_THREAD_FWD_REF(Fp) c) {
    shared_ptr<future_executor_continuation_shared_state<Ex,F,Rp, Fp> >
        h(new future_executor_continuation_shared_state<Ex, F,Rp, Fp>(ex, autoboost::move(f), autoboost::forward<Fp>(c)));
    h->parent.future_->set_continuation_ptr(h, lock);

    return AUTOBOOST_THREAD_FUTURE<Rp>(h);
  }
#endif

  ////////////////////////////////
  // make_shared_future_deferred_continuation_shared_state
  ////////////////////////////////
  template<typename F, typename Rp, typename Fp>
  AUTOBOOST_THREAD_FUTURE<Rp>
  make_shared_future_deferred_continuation_shared_state(
      autoboost::unique_lock<autoboost::mutex> &lock,
      F f, AUTOBOOST_THREAD_FWD_REF(Fp) c) {
    shared_ptr<shared_future_deferred_continuation_shared_state<F, Rp, Fp> >
        h(new shared_future_deferred_continuation_shared_state<F, Rp, Fp>(f, autoboost::forward<Fp>(c)));
    h->parent.future_->set_continuation_ptr(h, lock);
    return AUTOBOOST_THREAD_FUTURE<Rp>(h);
  }
  ////////////////////////////////
  // make_shared_future_async_continuation_shared_state
  ////////////////////////////////
  template<typename F, typename Rp, typename Fp>
  AUTOBOOST_THREAD_FUTURE<Rp>
  make_shared_future_async_continuation_shared_state(
      autoboost::unique_lock<autoboost::mutex> &lock, F f,
      AUTOBOOST_THREAD_FWD_REF(Fp) c) {
    shared_ptr<shared_future_async_continuation_shared_state<F,Rp, Fp> >
        h(new shared_future_async_continuation_shared_state<F,Rp, Fp>(f, autoboost::forward<Fp>(c)));
    h->parent.future_->set_continuation_ptr(h, lock);

    return AUTOBOOST_THREAD_FUTURE<Rp>(h);
  }
  ////////////////////////////////
  // make_shared_future_executor_continuation_shared_state
  ////////////////////////////////
#ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS
  template<typename Ex, typename F, typename Rp, typename Fp>
  AUTOBOOST_THREAD_FUTURE<Rp>
  make_shared_future_executor_continuation_shared_state(Ex& ex,
      autoboost::unique_lock<autoboost::mutex> &lock, F f,
      AUTOBOOST_THREAD_FWD_REF(Fp) c) {
    shared_ptr<shared_future_executor_continuation_shared_state<Ex, F, Rp, Fp> >
        h(new shared_future_executor_continuation_shared_state<Ex, F, Rp, Fp>(ex, f, autoboost::forward<Fp>(c)));
    h->parent.future_->set_continuation_ptr(h, lock);

    return AUTOBOOST_THREAD_FUTURE<Rp>(h);
  }
#endif
}

  ////////////////////////////////
  // template<typename F>
  // auto future<R>::then(F&& func) -> AUTOBOOST_THREAD_FUTURE<decltype(func(*this))>;
  ////////////////////////////////

  template <typename R>
  template <typename F>
  inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE<R>)>::type>
  AUTOBOOST_THREAD_FUTURE<R>::then(launch policy, AUTOBOOST_THREAD_FWD_REF(F) func) {
    typedef typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE<R>)>::type future_type;
    AUTOBOOST_THREAD_ASSERT_PRECONDITION(this->future_!=0, future_uninitialized());

    autoboost::unique_lock<autoboost::mutex> lock(this->future_->mutex);
    if (underlying_cast<int>(policy) & int(launch::async)) {
      return AUTOBOOST_THREAD_MAKE_RV_REF((autoboost::detail::make_future_async_continuation_shared_state<AUTOBOOST_THREAD_FUTURE<R>, future_type, F>(
                  lock, autoboost::move(*this), autoboost::forward<F>(func)
              )));
    } else if (underlying_cast<int>(policy) & int(launch::deferred)) {
      return AUTOBOOST_THREAD_MAKE_RV_REF((autoboost::detail::make_future_deferred_continuation_shared_state<AUTOBOOST_THREAD_FUTURE<R>, future_type, F>(
                  lock, autoboost::move(*this), autoboost::forward<F>(func)
              )));
    } else {
      return AUTOBOOST_THREAD_MAKE_RV_REF((autoboost::detail::make_future_async_continuation_shared_state<AUTOBOOST_THREAD_FUTURE<R>, future_type, F>(
                  lock, autoboost::move(*this), autoboost::forward<F>(func)
              )));
    }
  }
#ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS
  template <typename R>
  template <typename Ex, typename F>
  inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE<R>)>::type>
  AUTOBOOST_THREAD_FUTURE<R>::then(Ex& ex, AUTOBOOST_THREAD_FWD_REF(F) func) {
    typedef typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE<R>)>::type future_type;
    AUTOBOOST_THREAD_ASSERT_PRECONDITION(this->future_!=0, future_uninitialized());

    autoboost::unique_lock<autoboost::mutex> lock(this->future_->mutex);
    return AUTOBOOST_THREAD_MAKE_RV_REF((autoboost::detail::make_future_executor_continuation_shared_state<Ex, AUTOBOOST_THREAD_FUTURE<R>, future_type, F>(ex,
                  lock, autoboost::move(*this), autoboost::forward<F>(func)
              )));
  }
#endif
  template <typename R>
  template <typename F>
  inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE<R>)>::type>
  AUTOBOOST_THREAD_FUTURE<R>::then(AUTOBOOST_THREAD_FWD_REF(F) func)  {
    typedef typename autoboost::result_of<F(AUTOBOOST_THREAD_FUTURE<R>)>::type future_type;
    AUTOBOOST_THREAD_ASSERT_PRECONDITION(this->future_!=0, future_uninitialized());

    autoboost::unique_lock<autoboost::mutex> lock(this->future_->mutex);
    if (underlying_cast<int>(this->launch_policy(lock)) & int(launch::async)) {
      return autoboost::detail::make_future_async_continuation_shared_state<AUTOBOOST_THREAD_FUTURE<R>, future_type, F>(
          lock, autoboost::move(*this), autoboost::forward<F>(func)
      );
    } else if (underlying_cast<int>(this->launch_policy(lock)) & int(launch::deferred)) {
      this->future_->wait_internal(lock);
      return autoboost::detail::make_future_deferred_continuation_shared_state<AUTOBOOST_THREAD_FUTURE<R>, future_type, F>(
          lock, autoboost::move(*this), autoboost::forward<F>(func)
      );
    } else {
      return autoboost::detail::make_future_async_continuation_shared_state<AUTOBOOST_THREAD_FUTURE<R>, future_type, F>(
          lock, autoboost::move(*this), autoboost::forward<F>(func)
      );
    }
  }

  template <typename R>
  template <typename F>
  inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(shared_future<R>)>::type>
  shared_future<R>::then(launch policy, AUTOBOOST_THREAD_FWD_REF(F) func)  const
  {
    typedef typename autoboost::result_of<F(shared_future<R>)>::type future_type;
    AUTOBOOST_THREAD_ASSERT_PRECONDITION(this->future_!=0, future_uninitialized());

    autoboost::unique_lock<autoboost::mutex> lock(this->future_->mutex);

    if (underlying_cast<int>(policy) & int(launch::async)) {
      return AUTOBOOST_THREAD_MAKE_RV_REF((autoboost::detail::make_shared_future_async_continuation_shared_state<shared_future<R>, future_type, F>(
                  lock, *this, autoboost::forward<F>(func)
              )));
    } else if (underlying_cast<int>(policy) & int(launch::deferred)) {
      return AUTOBOOST_THREAD_MAKE_RV_REF((autoboost::detail::make_shared_future_deferred_continuation_shared_state<shared_future<R>, future_type, F>(
                  lock, *this, autoboost::forward<F>(func)
              )));
    } else {
      return AUTOBOOST_THREAD_MAKE_RV_REF((autoboost::detail::make_shared_future_async_continuation_shared_state<shared_future<R>, future_type, F>(
                  lock, *this, autoboost::forward<F>(func)
              )));
    }
  }
#ifdef AUTOBOOST_THREAD_PROVIDES_EXECUTORS
  template <typename R>
  template <typename Ex, typename F>
  inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(shared_future<R>)>::type>
  shared_future<R>::then(Ex& ex, AUTOBOOST_THREAD_FWD_REF(F) func)  const
  {
    typedef typename autoboost::result_of<F(shared_future<R>)>::type future_type;
    AUTOBOOST_THREAD_ASSERT_PRECONDITION(this->future_!=0, future_uninitialized());

    autoboost::unique_lock<autoboost::mutex> lock(this->future_->mutex);
    return AUTOBOOST_THREAD_MAKE_RV_REF((autoboost::detail::make_shared_future_executor_continuation_shared_state<Ex, shared_future<R>, future_type, F>(ex,
                  lock, *this, autoboost::forward<F>(func)
              )));
  }
#endif

  template <typename R>
  template <typename F>
  inline AUTOBOOST_THREAD_FUTURE<typename autoboost::result_of<F(shared_future<R>)>::type>
  shared_future<R>::then(AUTOBOOST_THREAD_FWD_REF(F) func)  const {
    typedef typename autoboost::result_of<F(shared_future<R>)>::type future_type;

    AUTOBOOST_THREAD_ASSERT_PRECONDITION(this->future_!=0, future_uninitialized());

    autoboost::unique_lock<autoboost::mutex> lock(this->future_->mutex);
    if (underlying_cast<int>(this->launch_policy(lock)) & int(launch::async)) {
      return autoboost::detail::make_shared_future_async_continuation_shared_state<shared_future<R>, future_type, F>(
          lock, *this, autoboost::forward<F>(func));
    } else if (underlying_cast<int>(this->launch_policy(lock)) & int(launch::deferred)) {
      this->future_->wait_internal(lock);
      return autoboost::detail::make_shared_future_deferred_continuation_shared_state<shared_future<R>, future_type, F>(
          lock, *this, autoboost::forward<F>(func));
    } else {
      return autoboost::detail::make_shared_future_async_continuation_shared_state<shared_future<R>, future_type, F>(
          lock, *this, autoboost::forward<F>(func));
    }
  }

namespace detail
{
  template <typename T>
  struct mfallbacker_to
  {
    T value_;
    typedef T result_type;
    mfallbacker_to(AUTOBOOST_THREAD_RV_REF(T) v)
    : value_(autoboost::move(v))
    {}

    T operator()(AUTOBOOST_THREAD_FUTURE<T> fut) {
      return fut.get_or(autoboost::move(value_));
    }
  };
  template <typename T>
  struct cfallbacker_to
  {
    T value_;
    typedef T result_type;
    cfallbacker_to(T const& v)
    : value_(v)
    {}

    T operator()(AUTOBOOST_THREAD_FUTURE<T> fut) {
      return fut.get_or(value_);

    }
  };
}
  ////////////////////////////////
  // future<R> future<R>::fallback_to(R&& v);
  ////////////////////////////////

  template <typename R>
  template <typename R2>
  inline typename autoboost::disable_if< is_void<R2>, AUTOBOOST_THREAD_FUTURE<R> >::type
  AUTOBOOST_THREAD_FUTURE<R>::fallback_to(AUTOBOOST_THREAD_RV_REF(R2) v) {
    return then(detail::mfallbacker_to<R>(autoboost::move(v)));
  }

  template <typename R>
  template <typename R2>
  inline typename autoboost::disable_if< is_void<R2>, AUTOBOOST_THREAD_FUTURE<R> >::type
  AUTOBOOST_THREAD_FUTURE<R>::fallback_to(R2 const& v) {
    return then(detail::cfallbacker_to<R>(v));
  }

#endif

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_UNWRAP
namespace detail
{
  /////////////////////////
  /// future_unwrap_shared_state
  /////////////////////////

  template<typename F, typename Rp>
  struct future_unwrap_shared_state: shared_state<Rp>
  {
    F parent;
  public:
    explicit future_unwrap_shared_state(AUTOBOOST_THREAD_RV_REF(F) f)
    : parent(autoboost::move(f)) {}

    typename F::value_type parent_value(autoboost::unique_lock<autoboost::mutex>& ) {
        typename F::value_type r = parent.get();
        r.set_exceptional_if_invalid();
        return autoboost::move(r);
    }

    virtual void wait(bool ) { // todo see if rethrow must be used
        autoboost::unique_lock<autoboost::mutex> lk(this->mutex);
        parent_value(lk).wait();
    }
    virtual Rp get() {
        autoboost::unique_lock<autoboost::mutex> lk(this->mutex);
        return parent_value(lk).get();
    }
  };

  template <class F, class Rp>
  AUTOBOOST_THREAD_FUTURE<Rp>
  make_future_unwrap_shared_state(autoboost::unique_lock<autoboost::mutex> &lock, AUTOBOOST_THREAD_RV_REF(F) f) {
    shared_ptr<future_unwrap_shared_state<F, Rp> >
        h(new future_unwrap_shared_state<F, Rp>(autoboost::move(f)));
    h->parent.future_->set_continuation_ptr(h, lock);
    return AUTOBOOST_THREAD_FUTURE<Rp>(h);
  }
}

  template <typename R>
  inline AUTOBOOST_THREAD_FUTURE<R>::AUTOBOOST_THREAD_FUTURE(AUTOBOOST_THREAD_RV_REF(AUTOBOOST_THREAD_FUTURE<AUTOBOOST_THREAD_FUTURE<R> >) other)
  : base_type(other.unwrap()) {}

  template <typename R2>
  AUTOBOOST_THREAD_FUTURE<R2>
  AUTOBOOST_THREAD_FUTURE<AUTOBOOST_THREAD_FUTURE<R2> >::unwrap()
  {
    AUTOBOOST_THREAD_ASSERT_PRECONDITION(this->future_!=0, future_uninitialized());
    autoboost::unique_lock<autoboost::mutex> lock(this->future_->mutex);
    return autoboost::detail::make_future_unwrap_shared_state<AUTOBOOST_THREAD_FUTURE<AUTOBOOST_THREAD_FUTURE<R2> >, R2>(lock, autoboost::move(*this));
  }
#endif

#if defined AUTOBOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
namespace detail
{
  struct input_iterator_tag {};
  struct vector_tag {};
  struct values_tag {};
  template <typename T>
  struct alias_t { typedef T type; };

  AUTOBOOST_CONSTEXPR_OR_CONST input_iterator_tag input_iterator_tag_value = {};
  AUTOBOOST_CONSTEXPR_OR_CONST vector_tag vector_tag_value = {};
  AUTOBOOST_CONSTEXPR_OR_CONST values_tag values_tag_value = {};
  ////////////////////////////////
  // detail::future_async_when_all_shared_state
  ////////////////////////////////
  template<typename F>
  struct future_when_all_vector_shared_state: future_async_shared_state_base<csbl::vector<F> >
  {
    typedef csbl::vector<F> vector_type;
    typedef typename F::value_type value_type;
    vector_type vec_;

    static void run(future_when_all_vector_shared_state* that) {
      try {
        autoboost::wait_for_all(that->vec_.begin(), that->vec_.end());
        that->mark_finished_with_result(autoboost::move(that->vec_));
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }
    bool run_deferred() {

      bool res = false;
      for (typename csbl::vector<F>::iterator it = vec_.begin(); it != vec_.end(); ++it) {
        if (! it->run_if_is_deferred())
        {
          res = true;
        }
      }
      return res;
    }
    void init() {
      if (! run_deferred())
      {
        future_when_all_vector_shared_state::run(this);
        return;
      }
      this->thr_ = thread(&future_when_all_vector_shared_state::run, this);
    }

  public:
    template< typename InputIterator>
    future_when_all_vector_shared_state(input_iterator_tag, InputIterator first, InputIterator last)
    : vec_(std::make_move_iterator(first), std::make_move_iterator(last))
    {
      init();
    }

    future_when_all_vector_shared_state(vector_tag, AUTOBOOST_THREAD_RV_REF(csbl::vector<F>) v)
    : vec_(autoboost::move(v))
    {
      init();
    }

#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template< typename T0, typename ...T>
    future_when_all_vector_shared_state(values_tag, AUTOBOOST_THREAD_FWD_REF(T0) f, AUTOBOOST_THREAD_FWD_REF(T) ... futures) {
      vec_.push_back(autoboost::forward<T0>(f));
      typename alias_t<char[]>::type{
          ( //first part of magic unpacker
          vec_.push_back(autoboost::forward<T>(futures)),'0'
          )..., '0'
      }; //second part of magic unpacker
      init();
    }
#endif
    ~future_when_all_vector_shared_state() {
      this->join();
    }
  };

  ////////////////////////////////
  // detail::future_async_when_any_shared_state
  ////////////////////////////////
  template<typename F>
  struct future_when_any_vector_shared_state: future_async_shared_state_base<csbl::vector<F> >
  {
    typedef csbl::vector<F> vector_type;
    typedef typename F::value_type value_type;
    vector_type vec_;

    static void run(future_when_any_vector_shared_state* that)
    {
      try {
        autoboost::wait_for_any(that->vec_.begin(), that->vec_.end());
        that->mark_finished_with_result(autoboost::move(that->vec_));
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }
    bool run_deferred() {

      for (typename csbl::vector<F>::iterator it = vec_.begin(); it != vec_.end(); ++it) {
        if (it->run_if_is_deferred_or_ready())
        {
          return true;
        }
      }
      return false;
    }
    void init() {
      if (run_deferred())
      {
        future_when_any_vector_shared_state::run(this);
        return;
      }

      this->thr_ = thread(&future_when_any_vector_shared_state::run, this);
    }

  public:
    template< typename InputIterator>
    future_when_any_vector_shared_state(input_iterator_tag, InputIterator first, InputIterator last)
    : vec_(std::make_move_iterator(first), std::make_move_iterator(last))
    {
      init();
    }

    future_when_any_vector_shared_state(vector_tag, AUTOBOOST_THREAD_RV_REF(csbl::vector<F>) v)
    : vec_(autoboost::move(v))
    {
      init();
    }

#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
    template< typename T0, typename ...T>
    future_when_any_vector_shared_state(values_tag,
        AUTOBOOST_THREAD_FWD_REF(T0) f, AUTOBOOST_THREAD_FWD_REF(T) ... futures
    ) {
      vec_.push_back(autoboost::forward<T0>(f));
      typename alias_t<char[]>::type{
          ( //first part of magic unpacker
          vec_.push_back(autoboost::forward<T>(futures))
          ,'0'
          )...,
          '0'
      }; //second part of magic unpacker
      init();
    }
#endif

    ~future_when_any_vector_shared_state()    {
      this->join();
    }
  };

#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
  struct wait_for_all_fctr {
    template <class ...T>
    void operator()(T&&... v) {
      autoboost::wait_for_all(autoboost::forward<T>(v)...);
    }
  };

  struct wait_for_any_fctr {
    template <class ...T>
    void operator()(T&&... v) {
      autoboost::wait_for_any(autoboost::forward<T>(v)...);
    }
  };


  template <class Tuple, std::size_t i=csbl::tuple_size<Tuple>::value>
  struct accumulate_run_if_is_deferred {
    bool operator ()(Tuple& t)
    {
      return (! csbl::get<i-1>(t).run_if_is_deferred()) || accumulate_run_if_is_deferred<Tuple,i-1>()(t);
    }
  };
  template <class Tuple>
  struct accumulate_run_if_is_deferred<Tuple, 0> {
    bool operator ()(Tuple& )
    {
      return false;
    }
  };


  template< typename Tuple, typename T0, typename ...T>
  struct future_when_all_tuple_shared_state: future_async_shared_state_base<Tuple>
  {
    Tuple tup_;
    typedef typename make_tuple_indices<1+sizeof...(T)>::type Index;

    static void run(future_when_all_tuple_shared_state* that) {
      try {
        // TODO make use of apply(that->tup_, autoboost::detail::wait_for_all_fctor());
        that->wait_for_all(Index());

        that->mark_finished_with_result(autoboost::move(that->tup_));
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }

    template <size_t ...Indices>
    void wait_for_all(tuple_indices<Indices...>) {
#if defined AUTOBOOST_THREAD_PROVIDES_INVOKE
      return invoke<void>(wait_for_all_fctr(), csbl::get<Indices>(tup_)...);
#else
      return wait_for_all_fctr()(csbl::get<Indices>(tup_)...);
#endif
    }

    bool run_deferred() {

      return accumulate_run_if_is_deferred<Tuple>()(tup_);
    }
    void init() {
      if (! run_deferred())
      {
        future_when_all_tuple_shared_state::run(this);
        return;
      }

      this->thr_ = thread(&future_when_all_tuple_shared_state::run, this);
    }
  public:
    template< typename F, typename ...Fs>
    future_when_all_tuple_shared_state(values_tag, AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fs) ... futures) :
      tup_(autoboost::csbl::make_tuple(autoboost::forward<F>(f), autoboost::forward<Fs>(futures)...))
    {
      init();
    }
    ~future_when_all_tuple_shared_state() {
      this->join();
    }

  };


  template <class Tuple, std::size_t i=csbl::tuple_size<Tuple>::value>
  struct apply_any_run_if_is_deferred_or_ready {
    bool operator ()(Tuple& t)
    {
      if (csbl::get<i-1>(t).run_if_is_deferred_or_ready()) return true;
      return apply_any_run_if_is_deferred_or_ready<Tuple,i-1>()(t);
    }
  };
  template <class Tuple>
  struct apply_any_run_if_is_deferred_or_ready<Tuple, 0> {
    bool operator ()(Tuple& )
    {
      return false;
    }
  };

  template< typename Tuple, typename T0, typename ...T >
  struct future_when_any_tuple_shared_state: future_async_shared_state_base<Tuple>
  {
    Tuple tup_;
    typedef typename make_tuple_indices<1+sizeof...(T)>::type Index;

    static void run(future_when_any_tuple_shared_state* that)
    {
      try {
        // TODO make use of apply(that->tup_, wait_for_any_fctr);
        that->wait_for_any(Index());

        that->mark_finished_with_result(autoboost::move(that->tup_));
#if defined AUTOBOOST_THREAD_PROVIDES_INTERRUPTIONS
      } catch(thread_interrupted& ) {
        that->mark_interrupted_finish();
#endif
      } catch(...) {
        that->mark_exceptional_finish();
      }
    }
    template <size_t ...Indices>
    void wait_for_any(tuple_indices<Indices...>) {
#if defined AUTOBOOST_THREAD_PROVIDES_INVOKE
      return invoke<void>(wait_for_any_fctr(), csbl::get<Indices>(tup_)...);
#else
      return wait_for_any_fctr()(csbl::get<Indices>(tup_)...);
#endif
    }
    bool run_deferred() {
      return apply_any_run_if_is_deferred_or_ready<Tuple>()(tup_);
    }
    void init() {
      if (run_deferred())
      {
        future_when_any_tuple_shared_state::run(this);
        return;
      }

      this->thr_ = thread(&future_when_any_tuple_shared_state::run, this);
    }

  public:
    template< typename F, typename ...Fs>
    future_when_any_tuple_shared_state(values_tag,
        AUTOBOOST_THREAD_FWD_REF(F) f, AUTOBOOST_THREAD_FWD_REF(Fs) ... futures
    ) :
      tup_(autoboost::csbl::make_tuple(autoboost::forward<F>(f), autoboost::forward<Fs>(futures)...))
    {
      init();
    }

    ~future_when_any_tuple_shared_state()    {
      this->join();
    }
  };
#endif

}

  template< typename InputIterator>
  typename autoboost::disable_if<is_future_type<InputIterator>,
    AUTOBOOST_THREAD_FUTURE<csbl::vector<typename InputIterator::value_type>  >
  >::type
  when_all(InputIterator first, InputIterator last) {
    typedef  typename InputIterator::value_type value_type;
    typedef  csbl::vector<value_type> container_type;
    typedef  detail::future_when_all_vector_shared_state<value_type> factory_type;

    if (first==last) return make_ready_future(container_type());
    shared_ptr<factory_type >
        h(new factory_type(detail::input_iterator_tag_value, first,last));
    return AUTOBOOST_THREAD_FUTURE<container_type>(h);
  }

  inline AUTOBOOST_THREAD_FUTURE<csbl::tuple<> > when_all() {
    return make_ready_future(csbl::tuple<>());
  }

#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
  template< typename T0, typename ...T>
  AUTOBOOST_THREAD_FUTURE<csbl::tuple<typename decay<T0>::type, typename decay<T>::type...> >
  when_all(AUTOBOOST_THREAD_FWD_REF(T0) f, AUTOBOOST_THREAD_FWD_REF(T) ... futures) {
    typedef csbl::tuple<typename decay<T0>::type, typename decay<T>::type...> container_type;
    typedef detail::future_when_all_tuple_shared_state<container_type, typename decay<T0>::type, typename decay<T>::type...> factory_type;

    shared_ptr<factory_type>
        h(new factory_type(detail::values_tag_value, autoboost::forward<T0>(f), autoboost::forward<T>(futures)...));
    return AUTOBOOST_THREAD_FUTURE<container_type>(h);
  }
#endif

  template< typename InputIterator>
  typename autoboost::disable_if<is_future_type<InputIterator>,
    AUTOBOOST_THREAD_FUTURE<csbl::vector<typename InputIterator::value_type>  >
  >::type
  when_any(InputIterator first, InputIterator last) {
    typedef  typename InputIterator::value_type value_type;
    typedef  csbl::vector<value_type> container_type;
    typedef  detail::future_when_any_vector_shared_state<value_type> factory_type;

    if (first==last) return make_ready_future(container_type());
    shared_ptr<factory_type >
        h(new factory_type(detail::input_iterator_tag_value, first,last));
    return AUTOBOOST_THREAD_FUTURE<container_type>(h);
  }

  inline AUTOBOOST_THREAD_FUTURE<csbl::tuple<> > when_any() {
    return make_ready_future(csbl::tuple<>());
  }

#if ! defined(AUTOBOOST_NO_CXX11_VARIADIC_TEMPLATES)
  template< typename T0, typename ...T>
  AUTOBOOST_THREAD_FUTURE<csbl::tuple<typename decay<T0>::type, typename decay<T>::type...> >
  when_any(AUTOBOOST_THREAD_FWD_REF(T0) f, AUTOBOOST_THREAD_FWD_REF(T) ... futures) {
    typedef csbl::tuple<typename decay<T0>::type, typename decay<T>::type...> container_type;
    typedef detail::future_when_any_tuple_shared_state<container_type, typename decay<T0>::type, typename decay<T>::type...> factory_type;

    shared_ptr<factory_type>
        h(new factory_type(detail::values_tag_value, autoboost::forward<T0>(f), autoboost::forward<T>(futures)...));
    return AUTOBOOST_THREAD_FUTURE<container_type>(h);
  }
#endif
#endif // AUTOBOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY
}

#endif // AUTOBOOST_NO_EXCEPTION
#endif // header
