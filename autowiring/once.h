// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_signal.h"
#include "spin_lock.h"
#include <vector>

namespace autowiring {
  namespace detail {
    struct once_fn {
      virtual ~once_fn(void) {}
      virtual void operator()() = 0;
    };

    template<typename Fn>
    struct once_fn_t :
      once_fn
    {
      once_fn_t(Fn&& fn) :
        fn(std::forward<Fn&&>(fn))
      {}

      const Fn fn;

      void operator()() override final { fn(); }
    };
  }

  /// <summary>
  /// Implements an observable boolean variable that can only be set to true once
  /// </summary>
  /// <remarks>
  /// There are many one-way state transitions in Autowiring which may require that
  /// specific 
  /// </remarks>
  struct once
  {
  protected:
    bool flag = false;
    autowiring::spin_lock m_spin;
    std::vector<std::unique_ptr<detail::once_fn>> m_fns;

  public:
    template<typename Fn>
    void operator+=(Fn&& rhs) {
      // Initial check of the flag, we don't even want to bother with the rest
      // of this function if the flag is already set
      if (flag) {
        rhs();
        return;
      }

      // Move the lambda into our unique pointer outside of the lock to reduce
      // total contention time
      std::unique_ptr<detail::once_fn_t<Fn>> fn{
        new detail::once_fn_t<Fn>{ std::forward<Fn&&>(rhs) }
      };

      // Double-check the flag under lock:
      {
        std::lock_guard<autowiring::spin_lock> lk(m_spin);
        if (!flag) {
          m_fns.push_back(std::move(fn));
          return;
        }
      }

      // We got here, the flag was set in the double-check.  We need to call
      // the lambda function, but unfortunately we've already moved it, so we
      // need to call the pointer version of the lambda.
      (*fn)();
    }

    /// <summary>
    /// Sets the control flag
    /// </summary>
    /// <remarks>
    /// This method is idempotent
    /// </remarks>
    void signal(void);

    /// <summary>
    /// Sets the control flag
    /// </summary>
    /// <remarks>
    /// It is an error to assign the once field to any value other than zero
    /// This method is identical to signal
    /// </remarks>
    void operator=(bool rhs);

    /// <summary>
    /// Sets the control flag
    /// </summary>
    /// <remarks>
    /// This method is identical to signal
    /// </remarks>
    void operator()() { signal(); }
  };

  /// <summary>
  /// Implements the observable portion of a once flag
  /// </summary>
  template<typename T>
  struct once_signal:
    private once
  {
  public:
    friend T;

    template<typename Fn>
    void operator+=(Fn&& fn) {
      return once::operator+=<Fn>(std::forward<Fn&&>(fn));
    }
  };
}
