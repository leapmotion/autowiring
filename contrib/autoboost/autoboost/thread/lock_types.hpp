// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2007 Anthony Williams
// (C) Copyright 2011-2012 Vicente J. Botet Escriba

#ifndef AUTOBOOST_THREAD_LOCK_TYPES_HPP
#define AUTOBOOST_THREAD_LOCK_TYPES_HPP

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/exceptions.hpp>
#include <autoboost/thread/lock_options.hpp>
#include <autoboost/thread/lockable_traits.hpp>
#if ! defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
#include <autoboost/thread/is_locked_by_this_thread.hpp>
#endif
#include <autoboost/thread/thread_time.hpp>

#include <autoboost/assert.hpp>
#ifdef AUTOBOOST_THREAD_USES_CHRONO
#include <autoboost/chrono/time_point.hpp>
#include <autoboost/chrono/duration.hpp>
#endif
#include <autoboost/detail/workaround.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{
  struct xtime;

  template <typename Mutex>
  class shared_lock;

  template <typename Mutex>
  class upgrade_lock;

  template <typename Mutex>
  class unique_lock;

  namespace detail
  {
    template <typename Mutex>
    class try_lock_wrapper;
  }

#ifdef AUTOBOOST_THREAD_NO_AUTO_DETECT_MUTEX_TYPES
  namespace sync
  {
    template<typename T>
    struct is_basic_lockable<unique_lock<T> >
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
    template<typename T>
    struct is_lockable<unique_lock<T> >
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };

    template<typename T>
    struct is_basic_lockable<shared_lock<T> >
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
    template<typename T>
    struct is_lockable<shared_lock<T> >
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };

    template<typename T>
    struct is_basic_lockable<upgrade_lock<T> >
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
    template<typename T>
    struct is_lockable<upgrade_lock<T> >
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };

    template<typename T>
    struct is_basic_lockable<detail::try_lock_wrapper<T> >
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
    template<typename T>
    struct is_lockable<detail::try_lock_wrapper<T> >
    {
      AUTOBOOST_STATIC_CONSTANT(bool, value = true);
    };
  }
#endif


  template <typename Mutex>
  class unique_lock
  {
  private:
    Mutex* m;
    bool is_locked;

  private:
    explicit unique_lock(upgrade_lock<Mutex>&);
    unique_lock& operator=(upgrade_lock<Mutex>& other);
  public:
    typedef Mutex mutex_type;
    AUTOBOOST_THREAD_MOVABLE_ONLY( unique_lock)

#if 0 // This should not be needed anymore. Use instead AUTOBOOST_THREAD_MAKE_RV_REF.
#if AUTOBOOST_WORKAROUND(__SUNPRO_CC, < 0x5100)
    unique_lock(const volatile unique_lock&);
#endif
#endif
    unique_lock()AUTOBOOST_NOEXCEPT :
    m(0),is_locked(false)
    {}

    explicit unique_lock(Mutex& m_) :
      m(&m_), is_locked(false)
    {
      lock();
    }
    unique_lock(Mutex& m_, adopt_lock_t) :
      m(&m_), is_locked(true)
    {
#if ! defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
      AUTOBOOST_ASSERT(is_locked_by_this_thread(m));
#endif
    }
    unique_lock(Mutex& m_, defer_lock_t)AUTOBOOST_NOEXCEPT:
    m(&m_),is_locked(false)
    {}
    unique_lock(Mutex& m_, try_to_lock_t) :
      m(&m_), is_locked(false)
    {
      try_lock();
    }
#if defined AUTOBOOST_THREAD_USES_DATETIME
    template<typename TimeDuration>
    unique_lock(Mutex& m_,TimeDuration const& target_time):
    m(&m_),is_locked(false)
    {
      timed_lock(target_time);
    }
    unique_lock(Mutex& m_,system_time const& target_time):
    m(&m_),is_locked(false)
    {
      timed_lock(target_time);
    }
#endif
#ifdef AUTOBOOST_THREAD_USES_CHRONO
    template <class Clock, class Duration>
    unique_lock(Mutex& mtx, const chrono::time_point<Clock, Duration>& t)
    : m(&mtx), is_locked(mtx.try_lock_until(t))
    {
    }
    template <class Rep, class Period>
    unique_lock(Mutex& mtx, const chrono::duration<Rep, Period>& d)
    : m(&mtx), is_locked(mtx.try_lock_for(d))
    {
    }
#endif

    unique_lock(AUTOBOOST_THREAD_RV_REF(unique_lock) other) AUTOBOOST_NOEXCEPT:
    m(AUTOBOOST_THREAD_RV(other).m),is_locked(AUTOBOOST_THREAD_RV(other).is_locked)
    {
      AUTOBOOST_THREAD_RV(other).is_locked=false;
      AUTOBOOST_THREAD_RV(other).m=0;
    }

    AUTOBOOST_THREAD_EXPLICIT_LOCK_CONVERSION unique_lock(AUTOBOOST_THREAD_RV_REF_BEG upgrade_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other);

#ifndef AUTOBOOST_THREAD_PROVIDES_EXPLICIT_LOCK_CONVERSION
    //std-2104 unique_lock move-assignment should not be noexcept
    unique_lock& operator=(AUTOBOOST_THREAD_RV_REF_BEG upgrade_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other) //AUTOBOOST_NOEXCEPT
    {
      unique_lock temp(::autoboost::move(other));
      swap(temp);
      return *this;
    }
#endif

    //std-2104 unique_lock move-assignment should not be noexcept
    unique_lock& operator=(AUTOBOOST_THREAD_RV_REF(unique_lock) other) //AUTOBOOST_NOEXCEPT
    {
      unique_lock temp(::autoboost::move(other));
      swap(temp);
      return *this;
    }
#if 0 // This should not be needed anymore. Use instead AUTOBOOST_THREAD_MAKE_RV_REF.
#if AUTOBOOST_WORKAROUND(__SUNPRO_CC, < 0x5100)
    unique_lock& operator=(unique_lock<Mutex> other)
    {
      swap(other);
      return *this;
    }
#endif // AUTOBOOST_WORKAROUND
#endif

    // Conversion from upgrade locking
    unique_lock(AUTOBOOST_THREAD_RV_REF_BEG upgrade_lock<mutex_type> AUTOBOOST_THREAD_RV_REF_END ul, try_to_lock_t)
    : m(0),is_locked(false)
    {
      if (AUTOBOOST_THREAD_RV(ul).owns_lock())
      {
        if (AUTOBOOST_THREAD_RV(ul).mutex()->try_unlock_upgrade_and_lock())
        {
          m = AUTOBOOST_THREAD_RV(ul).release();
          is_locked = true;
        }
      }
      else
      {
        m = AUTOBOOST_THREAD_RV(ul).release();
      }
    }

#ifdef AUTOBOOST_THREAD_USES_CHRONO
    template <class Clock, class Duration>
    unique_lock(AUTOBOOST_THREAD_RV_REF_BEG upgrade_lock<mutex_type> AUTOBOOST_THREAD_RV_REF_END ul,
        const chrono::time_point<Clock, Duration>& abs_time)
    : m(0),is_locked(false)
    {
      if (AUTOBOOST_THREAD_RV(ul).owns_lock())
      {
        if (AUTOBOOST_THREAD_RV(ul).mutex()->try_unlock_upgrade_and_lock_until(abs_time))
        {
          m = AUTOBOOST_THREAD_RV(ul).release();
          is_locked = true;
        }
      }
      else
      {
        m = AUTOBOOST_THREAD_RV(ul).release();
      }
    }

    template <class Rep, class Period>
    unique_lock(AUTOBOOST_THREAD_RV_REF_BEG upgrade_lock<mutex_type> AUTOBOOST_THREAD_RV_REF_END ul,
        const chrono::duration<Rep, Period>& rel_time)
    : m(0),is_locked(false)
    {
      if (AUTOBOOST_THREAD_RV(ul).owns_lock())
      {
        if (AUTOBOOST_THREAD_RV(ul).mutex()->try_unlock_upgrade_and_lock_for(rel_time))
        {
          m = AUTOBOOST_THREAD_RV(ul).release();
          is_locked = true;
        }
      }
      else
      {
        m = AUTOBOOST_THREAD_RV(ul).release();
      }
    }
#endif

#ifdef AUTOBOOST_THREAD_PROVIDES_SHARED_MUTEX_UPWARDS_CONVERSIONS
    // Conversion from shared locking
    unique_lock(AUTOBOOST_THREAD_RV_REF_BEG shared_lock<mutex_type> AUTOBOOST_THREAD_RV_REF_END sl, try_to_lock_t)
    : m(0),is_locked(false)
    {
      if (AUTOBOOST_THREAD_RV(sl).owns_lock())
      {
        if (AUTOBOOST_THREAD_RV(sl).mutex()->try_unlock_shared_and_lock())
        {
          m = AUTOBOOST_THREAD_RV(sl).release();
          is_locked = true;
        }
      }
      else
      {
        m = AUTOBOOST_THREAD_RV(sl).release();
      }
    }

#ifdef AUTOBOOST_THREAD_USES_CHRONO
    template <class Clock, class Duration>
    unique_lock(AUTOBOOST_THREAD_RV_REF_BEG shared_lock<mutex_type> AUTOBOOST_THREAD_RV_REF_END sl,
        const chrono::time_point<Clock, Duration>& abs_time)
    : m(0),is_locked(false)
    {
      if (AUTOBOOST_THREAD_RV(sl).owns_lock())
      {
        if (AUTOBOOST_THREAD_RV(sl).mutex()->try_unlock_shared_and_lock_until(abs_time))
        {
          m = AUTOBOOST_THREAD_RV(sl).release();
          is_locked = true;
        }
      }
      else
      {
        m = AUTOBOOST_THREAD_RV(sl).release();
      }
    }

    template <class Rep, class Period>
    unique_lock(AUTOBOOST_THREAD_RV_REF_BEG shared_lock<mutex_type> AUTOBOOST_THREAD_RV_REF_END sl,
        const chrono::duration<Rep, Period>& rel_time)
    : m(0),is_locked(false)
    {
      if (AUTOBOOST_THREAD_RV(sl).owns_lock())
      {
        if (AUTOBOOST_THREAD_RV(sl).mutex()->try_unlock_shared_and_lock_for(rel_time))
        {
          m = AUTOBOOST_THREAD_RV(sl).release();
          is_locked = true;
        }
      }
      else
      {
        m = AUTOBOOST_THREAD_RV(sl).release();
      }
    }
#endif // AUTOBOOST_THREAD_USES_CHRONO
#endif // AUTOBOOST_THREAD_PROVIDES_SHARED_MUTEX_UPWARDS_CONVERSIONS

    void swap(unique_lock& other)AUTOBOOST_NOEXCEPT
    {
      std::swap(m,other.m);
      std::swap(is_locked,other.is_locked);
    }

    ~unique_lock()
    {
      if (owns_lock())
      {
        m->unlock();
      }
    }
    void lock()
    {
      if (m == 0)
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost unique_lock has no mutex"));
      }
      if (owns_lock())
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost unique_lock owns already the mutex"));
      }
      m->lock();
      is_locked = true;
    }
    bool try_lock()
    {
      if (m == 0)
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost unique_lock has no mutex"));
      }
      if (owns_lock())
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost unique_lock owns already the mutex"));
      }
      is_locked = m->try_lock();
      return is_locked;
    }
#if defined AUTOBOOST_THREAD_USES_DATETIME
    template<typename TimeDuration>
    bool timed_lock(TimeDuration const& relative_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost unique_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost unique_lock owns already the mutex"));
      }
      is_locked=m->timed_lock(relative_time);
      return is_locked;
    }

    bool timed_lock(::autoboost::system_time const& absolute_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost unique_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost unique_lock owns already the mutex"));
      }
      is_locked=m->timed_lock(absolute_time);
      return is_locked;
    }
    bool timed_lock(::autoboost::xtime const& absolute_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost unique_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost unique_lock owns already the mutex"));
      }
      is_locked=m->timed_lock(absolute_time);
      return is_locked;
    }
#endif
#ifdef AUTOBOOST_THREAD_USES_CHRONO

    template <class Rep, class Period>
    bool try_lock_for(const chrono::duration<Rep, Period>& rel_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost unique_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost unique_lock owns already the mutex"));
      }
      is_locked=m->try_lock_for(rel_time);
      return is_locked;
    }
    template <class Clock, class Duration>
    bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost unique_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost unique_lock owns already the mutex"));
      }
      is_locked=m->try_lock_until(abs_time);
      return is_locked;
    }
#endif

    void unlock()
    {
      if (m == 0)
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost unique_lock has no mutex"));
      }
      if (!owns_lock())
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost unique_lock doesn't own the mutex"));
      }
      m->unlock();
      is_locked = false;
    }

#if defined(AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
    typedef void (unique_lock::*bool_type)();
    operator bool_type() const AUTOBOOST_NOEXCEPT
    {
      return is_locked?&unique_lock::lock:0;
    }
    bool operator!() const AUTOBOOST_NOEXCEPT
    {
      return !owns_lock();
    }
#else
    explicit operator bool() const AUTOBOOST_NOEXCEPT
    {
      return owns_lock();
    }
#endif
    bool owns_lock() const AUTOBOOST_NOEXCEPT
    {
      return is_locked;
    }

    Mutex* mutex() const AUTOBOOST_NOEXCEPT
    {
      return m;
    }

    Mutex* release()AUTOBOOST_NOEXCEPT
    {
      Mutex* const res=m;
      m=0;
      is_locked=false;
      return res;
    }

    friend class shared_lock<Mutex> ;
    friend class upgrade_lock<Mutex> ;
  };

  template<typename Mutex>
  void swap(unique_lock<Mutex>& lhs, unique_lock<Mutex>& rhs)
  AUTOBOOST_NOEXCEPT
  {
    lhs.swap(rhs);
  }

  AUTOBOOST_THREAD_DCL_MOVABLE_BEG(Mutex) unique_lock<Mutex> AUTOBOOST_THREAD_DCL_MOVABLE_END

  template<typename Mutex>
  class shared_lock
  {
  protected:
    Mutex* m;
    bool is_locked;

  public:
    typedef Mutex mutex_type;
    AUTOBOOST_THREAD_MOVABLE_ONLY(shared_lock)

    shared_lock() AUTOBOOST_NOEXCEPT:
    m(0),is_locked(false)
    {}

    explicit shared_lock(Mutex& m_):
    m(&m_),is_locked(false)
    {
      lock();
    }
    shared_lock(Mutex& m_,adopt_lock_t):
    m(&m_),is_locked(true)
    {
#if ! defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
      AUTOBOOST_ASSERT(is_locked_by_this_thread(m));
#endif
    }
    shared_lock(Mutex& m_,defer_lock_t) AUTOBOOST_NOEXCEPT:
    m(&m_),is_locked(false)
    {}
    shared_lock(Mutex& m_,try_to_lock_t):
    m(&m_),is_locked(false)
    {
      try_lock();
    }
#if defined AUTOBOOST_THREAD_USES_DATETIME
    shared_lock(Mutex& m_,system_time const& target_time):
    m(&m_),is_locked(false)
    {
      timed_lock(target_time);
    }
#endif
#ifdef AUTOBOOST_THREAD_USES_CHRONO
    template <class Clock, class Duration>
    shared_lock(Mutex& mtx, const chrono::time_point<Clock, Duration>& t)
    : m(&mtx), is_locked(mtx.try_lock_shared_until(t))
    {
    }
    template <class Rep, class Period>
    shared_lock(Mutex& mtx, const chrono::duration<Rep, Period>& d)
    : m(&mtx), is_locked(mtx.try_lock_shared_for(d))
    {
    }
#endif

    shared_lock(AUTOBOOST_THREAD_RV_REF_BEG shared_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other) AUTOBOOST_NOEXCEPT:
    m(AUTOBOOST_THREAD_RV(other).m),is_locked(AUTOBOOST_THREAD_RV(other).is_locked)
    {
      AUTOBOOST_THREAD_RV(other).is_locked=false;
      AUTOBOOST_THREAD_RV(other).m=0;
    }

    AUTOBOOST_THREAD_EXPLICIT_LOCK_CONVERSION shared_lock(AUTOBOOST_THREAD_RV_REF_BEG unique_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other):
    m(AUTOBOOST_THREAD_RV(other).m),is_locked(AUTOBOOST_THREAD_RV(other).is_locked)
    {
      if(is_locked)
      {
        m->unlock_and_lock_shared();
      }
      AUTOBOOST_THREAD_RV(other).is_locked=false;
      AUTOBOOST_THREAD_RV(other).m=0;
    }

    AUTOBOOST_THREAD_EXPLICIT_LOCK_CONVERSION shared_lock(AUTOBOOST_THREAD_RV_REF_BEG upgrade_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other):
    m(AUTOBOOST_THREAD_RV(other).m),is_locked(AUTOBOOST_THREAD_RV(other).is_locked)
    {
      if(is_locked)
      {
        m->unlock_upgrade_and_lock_shared();
      }
      AUTOBOOST_THREAD_RV(other).is_locked=false;
      AUTOBOOST_THREAD_RV(other).m=0;
    }

    //std-2104 unique_lock move-assignment should not be noexcept
    shared_lock& operator=(AUTOBOOST_THREAD_RV_REF_BEG shared_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other) //AUTOBOOST_NOEXCEPT
    {
      shared_lock temp(::autoboost::move(other));
      swap(temp);
      return *this;
    }
#ifndef AUTOBOOST_THREAD_PROVIDES_EXPLICIT_LOCK_CONVERSION
    shared_lock& operator=(AUTOBOOST_THREAD_RV_REF_BEG unique_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other)
    {
      shared_lock temp(::autoboost::move(other));
      swap(temp);
      return *this;
    }

    shared_lock& operator=(AUTOBOOST_THREAD_RV_REF_BEG upgrade_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other)
    {
      shared_lock temp(::autoboost::move(other));
      swap(temp);
      return *this;
    }
#endif

    void swap(shared_lock& other) AUTOBOOST_NOEXCEPT
    {
      std::swap(m,other.m);
      std::swap(is_locked,other.is_locked);
    }

    Mutex* mutex() const AUTOBOOST_NOEXCEPT
    {
      return m;
    }

    Mutex* release() AUTOBOOST_NOEXCEPT
    {
      Mutex* const res=m;
      m=0;
      is_locked=false;
      return res;
    }

    ~shared_lock()
    {
      if(owns_lock())
      {
        m->unlock_shared();
      }
    }
    void lock()
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost shared_lock owns already the mutex"));
      }
      m->lock_shared();
      is_locked=true;
    }
    bool try_lock()
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost shared_lock owns already the mutex"));
      }
      is_locked=m->try_lock_shared();
      return is_locked;
    }
#if defined AUTOBOOST_THREAD_USES_DATETIME
    bool timed_lock(autoboost::system_time const& target_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost shared_lock owns already the mutex"));
      }
      is_locked=m->timed_lock_shared(target_time);
      return is_locked;
    }
    template<typename Duration>
    bool timed_lock(Duration const& target_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost shared_lock owns already the mutex"));
      }
      is_locked=m->timed_lock_shared(target_time);
      return is_locked;
    }
#endif
#ifdef AUTOBOOST_THREAD_USES_CHRONO
    template <class Rep, class Period>
    bool try_lock_for(const chrono::duration<Rep, Period>& rel_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost shared_lock owns already the mutex"));
      }
      is_locked=m->try_lock_shared_for(rel_time);
      return is_locked;
    }
    template <class Clock, class Duration>
    bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost shared_lock owns already the mutex"));
      }
      is_locked=m->try_lock_shared_until(abs_time);
      return is_locked;
    }
#endif
    void unlock()
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if(!owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock doesn't own the mutex"));
      }
      m->unlock_shared();
      is_locked=false;
    }

#if defined(AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
    typedef void (shared_lock<Mutex>::*bool_type)();
    operator bool_type() const AUTOBOOST_NOEXCEPT
    {
      return is_locked?&shared_lock::lock:0;
    }
    bool operator!() const AUTOBOOST_NOEXCEPT
    {
      return !owns_lock();
    }
#else
    explicit operator bool() const AUTOBOOST_NOEXCEPT
    {
      return owns_lock();
    }
#endif
    bool owns_lock() const AUTOBOOST_NOEXCEPT
    {
      return is_locked;
    }

  };

  AUTOBOOST_THREAD_DCL_MOVABLE_BEG(Mutex) shared_lock<Mutex> AUTOBOOST_THREAD_DCL_MOVABLE_END

  template<typename Mutex>
  void swap(shared_lock<Mutex>& lhs,shared_lock<Mutex>& rhs) AUTOBOOST_NOEXCEPT
  {
    lhs.swap(rhs);
  }

  template <typename Mutex>
  class upgrade_lock
  {
  protected:
    Mutex* m;
    bool is_locked;

  public:
    typedef Mutex mutex_type;
    AUTOBOOST_THREAD_MOVABLE_ONLY( upgrade_lock)

    upgrade_lock()AUTOBOOST_NOEXCEPT:
    m(0),is_locked(false)
    {}

    explicit upgrade_lock(Mutex& m_) :
      m(&m_), is_locked(false)
    {
      lock();
    }
    upgrade_lock(Mutex& m_, adopt_lock_t) :
      m(&m_), is_locked(true)
    {
#if ! defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
      AUTOBOOST_ASSERT(is_locked_by_this_thread(m));
#endif
    }
    upgrade_lock(Mutex& m_, defer_lock_t)AUTOBOOST_NOEXCEPT:
    m(&m_),is_locked(false)
    {}
    upgrade_lock(Mutex& m_, try_to_lock_t) :
      m(&m_), is_locked(false)
    {
      try_lock();
    }

#ifdef AUTOBOOST_THREAD_USES_CHRONO
    template <class Clock, class Duration>
    upgrade_lock(Mutex& mtx, const chrono::time_point<Clock, Duration>& t)
    : m(&mtx), is_locked(mtx.try_lock_upgrade_until(t))
    {
    }
    template <class Rep, class Period>
    upgrade_lock(Mutex& mtx, const chrono::duration<Rep, Period>& d)
    : m(&mtx), is_locked(mtx.try_lock_upgrade_for(d))
    {
    }
#endif

    upgrade_lock(AUTOBOOST_THREAD_RV_REF_BEG upgrade_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other) AUTOBOOST_NOEXCEPT:
    m(AUTOBOOST_THREAD_RV(other).m),is_locked(AUTOBOOST_THREAD_RV(other).is_locked)
    {
      AUTOBOOST_THREAD_RV(other).is_locked=false;
      AUTOBOOST_THREAD_RV(other).m=0;
    }

    AUTOBOOST_THREAD_EXPLICIT_LOCK_CONVERSION upgrade_lock(AUTOBOOST_THREAD_RV_REF_BEG unique_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other):
    m(AUTOBOOST_THREAD_RV(other).m),is_locked(AUTOBOOST_THREAD_RV(other).is_locked)
    {
      if(is_locked)
      {
        m->unlock_and_lock_upgrade();
      }
      AUTOBOOST_THREAD_RV(other).is_locked=false;
      AUTOBOOST_THREAD_RV(other).m=0;
    }

    //std-2104 unique_lock move-assignment should not be noexcept
    upgrade_lock& operator=(AUTOBOOST_THREAD_RV_REF_BEG upgrade_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other) //AUTOBOOST_NOEXCEPT
    {
      upgrade_lock temp(::autoboost::move(other));
      swap(temp);
      return *this;
    }

#ifndef AUTOBOOST_THREAD_PROVIDES_EXPLICIT_LOCK_CONVERSION
    upgrade_lock& operator=(AUTOBOOST_THREAD_RV_REF_BEG unique_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other)
    {
      upgrade_lock temp(::autoboost::move(other));
      swap(temp);
      return *this;
    }
#endif

#ifdef AUTOBOOST_THREAD_PROVIDES_SHARED_MUTEX_UPWARDS_CONVERSIONS
    // Conversion from shared locking
    upgrade_lock(AUTOBOOST_THREAD_RV_REF_BEG shared_lock<mutex_type> AUTOBOOST_THREAD_RV_REF_END sl, try_to_lock_t)
    : m(0),is_locked(false)
    {
      if (AUTOBOOST_THREAD_RV(sl).owns_lock())
      {
        if (AUTOBOOST_THREAD_RV(sl).mutex()->try_unlock_shared_and_lock_upgrade())
        {
          m = AUTOBOOST_THREAD_RV(sl).release();
          is_locked = true;
        }
      }
      else
      {
        m = AUTOBOOST_THREAD_RV(sl).release();
      }
    }

#ifdef AUTOBOOST_THREAD_USES_CHRONO
    template <class Clock, class Duration>
    upgrade_lock(AUTOBOOST_THREAD_RV_REF_BEG shared_lock<mutex_type> AUTOBOOST_THREAD_RV_REF_END sl,
        const chrono::time_point<Clock, Duration>& abs_time)
    : m(0),is_locked(false)
    {
      if (AUTOBOOST_THREAD_RV(sl).owns_lock())
      {
        if (AUTOBOOST_THREAD_RV(sl).mutex()->try_unlock_shared_and_lock_upgrade_until(abs_time))
        {
          m = AUTOBOOST_THREAD_RV(sl).release();
          is_locked = true;
        }
      }
      else
      {
        m = AUTOBOOST_THREAD_RV(sl).release();
      }
    }

    template <class Rep, class Period>
    upgrade_lock(AUTOBOOST_THREAD_RV_REF_BEG shared_lock<mutex_type> AUTOBOOST_THREAD_RV_REF_END sl,
        const chrono::duration<Rep, Period>& rel_time)
    : m(0),is_locked(false)
    {
      if (AUTOBOOST_THREAD_RV(sl).owns_lock())
      {
        if (AUTOBOOST_THREAD_RV(sl).mutex()->try_unlock_shared_and_lock_upgrade_for(rel_time))
        {
          m = AUTOBOOST_THREAD_RV(sl).release();
          is_locked = true;
        }
      }
      else
      {
        m = AUTOBOOST_THREAD_RV(sl).release();
      }
    }
#endif // AUTOBOOST_THREAD_USES_CHRONO
#endif // AUTOBOOST_THREAD_PROVIDES_SHARED_MUTEX_UPWARDS_CONVERSIONS
    void swap(upgrade_lock& other)AUTOBOOST_NOEXCEPT
    {
      std::swap(m,other.m);
      std::swap(is_locked,other.is_locked);
    }
    Mutex* mutex() const AUTOBOOST_NOEXCEPT
    {
      return m;
    }

    Mutex* release()AUTOBOOST_NOEXCEPT
    {
      Mutex* const res=m;
      m=0;
      is_locked=false;
      return res;
    }
    ~upgrade_lock()
    {
      if (owns_lock())
      {
        m->unlock_upgrade();
      }
    }
    void lock()
    {
      if (m == 0)
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if (owns_lock())
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost upgrade_lock owns already the mutex"));
      }
      m->lock_upgrade();
      is_locked = true;
    }
    bool try_lock()
    {
      if (m == 0)
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if (owns_lock())
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost upgrade_lock owns already the mutex"));
      }
      is_locked = m->try_lock_upgrade();
      return is_locked;
    }
    void unlock()
    {
      if (m == 0)
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if (!owns_lock())
      {
        autoboost::throw_exception(
            autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost upgrade_lock doesn't own the mutex"));
      }
      m->unlock_upgrade();
      is_locked = false;
    }
#ifdef AUTOBOOST_THREAD_USES_CHRONO
    template <class Rep, class Period>
    bool try_lock_for(const chrono::duration<Rep, Period>& rel_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost shared_lock owns already the mutex"));
      }
      is_locked=m->try_lock_upgrade_for(rel_time);
      return is_locked;
    }
    template <class Clock, class Duration>
    bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time)
    {
      if(m==0)
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::operation_not_permitted), "autoboost shared_lock has no mutex"));
      }
      if(owns_lock())
      {
        autoboost::throw_exception(autoboost::lock_error(static_cast<int>(system::errc::resource_deadlock_would_occur), "autoboost shared_lock owns already the mutex"));
      }
      is_locked=m->try_lock_upgrade_until(abs_time);
      return is_locked;
    }
#endif
#if defined(AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
    typedef void (upgrade_lock::*bool_type)();
    operator bool_type() const AUTOBOOST_NOEXCEPT
    {
      return is_locked?&upgrade_lock::lock:0;
    }
    bool operator!() const AUTOBOOST_NOEXCEPT
    {
      return !owns_lock();
    }
#else
    explicit operator bool() const AUTOBOOST_NOEXCEPT
    {
      return owns_lock();
    }
#endif
    bool owns_lock() const AUTOBOOST_NOEXCEPT
    {
      return is_locked;
    }
    friend class shared_lock<Mutex> ;
    friend class unique_lock<Mutex> ;
  };

  template<typename Mutex>
  void swap(upgrade_lock<Mutex>& lhs, upgrade_lock<Mutex>& rhs)
  AUTOBOOST_NOEXCEPT
  {
    lhs.swap(rhs);
  }

  AUTOBOOST_THREAD_DCL_MOVABLE_BEG(Mutex) upgrade_lock<Mutex> AUTOBOOST_THREAD_DCL_MOVABLE_END

  template<typename Mutex>
  unique_lock<Mutex>::unique_lock(AUTOBOOST_THREAD_RV_REF_BEG upgrade_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other):
  m(AUTOBOOST_THREAD_RV(other).m),is_locked(AUTOBOOST_THREAD_RV(other).is_locked)
  {
    if(is_locked)
    {
      m->unlock_upgrade_and_lock();
    }
    AUTOBOOST_THREAD_RV(other).release();
  }

  template <class Mutex>
  class upgrade_to_unique_lock
  {
  private:
    upgrade_lock<Mutex>* source;
    unique_lock<Mutex> exclusive;

  public:
    typedef Mutex mutex_type;
    AUTOBOOST_THREAD_MOVABLE_ONLY( upgrade_to_unique_lock)

    explicit upgrade_to_unique_lock(upgrade_lock<Mutex>& m_) :
      source(&m_), exclusive(::autoboost::move(*source))
    {
    }
    ~upgrade_to_unique_lock()
    {
      if (source)
      {
        *source = AUTOBOOST_THREAD_MAKE_RV_REF(upgrade_lock<Mutex> (::autoboost::move(exclusive)));
      }
    }

    upgrade_to_unique_lock(AUTOBOOST_THREAD_RV_REF_BEG upgrade_to_unique_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other) AUTOBOOST_NOEXCEPT:
    source(AUTOBOOST_THREAD_RV(other).source),exclusive(::autoboost::move(AUTOBOOST_THREAD_RV(other).exclusive))
    {
      AUTOBOOST_THREAD_RV(other).source=0;
    }

    //std-2104 unique_lock move-assignment should not be noexcept
    upgrade_to_unique_lock& operator=(AUTOBOOST_THREAD_RV_REF_BEG upgrade_to_unique_lock<Mutex> AUTOBOOST_THREAD_RV_REF_END other) //AUTOBOOST_NOEXCEPT
    {
      upgrade_to_unique_lock temp(other);
      swap(temp);
      return *this;
    }

    void swap(upgrade_to_unique_lock& other)AUTOBOOST_NOEXCEPT
    {
      std::swap(source,other.source);
      exclusive.swap(other.exclusive);
    }

#if defined(AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
    typedef void (upgrade_to_unique_lock::*bool_type)(upgrade_to_unique_lock&);
    operator bool_type() const AUTOBOOST_NOEXCEPT
    {
      return exclusive.owns_lock()?&upgrade_to_unique_lock::swap:0;
    }
    bool operator!() const AUTOBOOST_NOEXCEPT
    {
      return !owns_lock();
    }
#else
    explicit operator bool() const AUTOBOOST_NOEXCEPT
    {
      return owns_lock();
    }
#endif

    bool owns_lock() const AUTOBOOST_NOEXCEPT
    {
      return exclusive.owns_lock();
    }
    Mutex* mutex() const AUTOBOOST_NOEXCEPT
    {
      return exclusive.mutex();
    }
  };

AUTOBOOST_THREAD_DCL_MOVABLE_BEG(Mutex) upgrade_to_unique_lock<Mutex> AUTOBOOST_THREAD_DCL_MOVABLE_END

namespace detail
{
  template<typename Mutex>
  class try_lock_wrapper:
private unique_lock<Mutex>
  {
    typedef unique_lock<Mutex> base;
  public:
    AUTOBOOST_THREAD_MOVABLE_ONLY(try_lock_wrapper)

    try_lock_wrapper()
    {}

    explicit try_lock_wrapper(Mutex& m):
    base(m,try_to_lock)
    {}

    try_lock_wrapper(Mutex& m_,adopt_lock_t):
    base(m_,adopt_lock)
    {
#if ! defined AUTOBOOST_THREAD_PROVIDES_NESTED_LOCKS
      AUTOBOOST_ASSERT(is_locked_by_this_thread(m_));
#endif
    }
    try_lock_wrapper(Mutex& m_,defer_lock_t):
    base(m_,defer_lock)
    {}
    try_lock_wrapper(Mutex& m_,try_to_lock_t):
    base(m_,try_to_lock)
    {}
#ifndef AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
    try_lock_wrapper(AUTOBOOST_THREAD_RV_REF(try_lock_wrapper) other):
    base(::autoboost::move(other))
    {}

#elif defined AUTOBOOST_THREAD_USES_MOVE
    try_lock_wrapper(AUTOBOOST_THREAD_RV_REF(try_lock_wrapper) other):
    base(::autoboost::move(static_cast<base&>(other)))
    {}

#else
    try_lock_wrapper(AUTOBOOST_THREAD_RV_REF(try_lock_wrapper) other):
    base(AUTOBOOST_THREAD_RV_REF(base)(*other))
    {}
#endif
    try_lock_wrapper& operator=(AUTOBOOST_THREAD_RV_REF_BEG try_lock_wrapper<Mutex> AUTOBOOST_THREAD_RV_REF_END other)
    {
      try_lock_wrapper temp(other);
      swap(temp);
      return *this;
    }
    void swap(try_lock_wrapper& other)
    {
      base::swap(other);
    }
    void lock()
    {
      base::lock();
    }
    bool try_lock()
    {
      return base::try_lock();
    }
    void unlock()
    {
      base::unlock();
    }
    bool owns_lock() const
    {
      return base::owns_lock();
    }
    Mutex* mutex() const AUTOBOOST_NOEXCEPT
    {
      return base::mutex();
    }
    Mutex* release()
    {
      return base::release();
    }

#if defined(AUTOBOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS)
    typedef typename base::bool_type bool_type;
    operator bool_type() const
    {
      return base::operator bool_type();
    }
    bool operator!() const
    {
      return !this->owns_lock();
    }
#else
    explicit operator bool() const
    {
      return owns_lock();
    }
#endif
  };

  template<typename Mutex>
  void swap(try_lock_wrapper<Mutex>& lhs,try_lock_wrapper<Mutex>& rhs)
  {
    lhs.swap(rhs);
  }
}
}
#include <autoboost/config/abi_suffix.hpp>

#endif
