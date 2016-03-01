// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_signal.h"
#include "callable.h"
#include "signal_base.h"
#include "spin_lock.h"
#include <mutex>
#include <vector>

namespace autowiring {
  /// <summary>
  /// Implements an observable boolean variable that can only be set to true once
  /// </summary>
  /// <remarks>
  /// There are many one-way state transitions in Autowiring which much implement the
  /// observable pattern.  Uers should be able to register a callback that will be
  /// invoked when a state transition occurs.  If the state transition has already
  /// occurred, then the callback must be made immediately.
  ///
  /// Once the state transition takes place, this type releases all memory associated
  /// with registered callbacks.
  ///
  /// There are a few thread contexts from which the callback may be made:
  ///
  /// 1. If the signal is already set, the callback is made immediately in the current
  ///    thread context.
  /// 2. If the signal has not yet been set, the callback is registered to be executed
  ///    later.  The callback will be invoked from the thread that eventually sets the
  ///    flag.
  ///
  /// If a callback is being registered at about the same time as the flag is being
  /// set, there is an ambiguity as to who will be responsible for making the call,
  /// but that ambiguity will always be resolved and the callback will be invoked at
  /// some point.
  ///
  /// There are no sequentiality or concurrency guarantees made with respect to
  /// registered listeners.  The last registered listener could potentially be the
  /// first one invoked, and may be executed concurrently with any other listener.
  /// This behavior represents a substantial departure from the behavior of the plain
  /// autowiring signal datatype.
  /// </remarks>
  struct once :
    signal_base
  {
  public:
    once(void) = default;
    once(const once& rhs) = delete;
    once(once&& rhs);

    enum class state {
      // Flag not set
      unsignalled,

      // Flag set, deferred handlers presently running
      signalling,

      // Flag set
      signalled
    };

  protected:
    state m_state = state::unsignalled;
    autowiring::spin_lock m_spin;
    std::vector<std::unique_ptr<callable_base>> m_fns;

  public:
    // Getter methods:
    bool get(void) const { return m_state != state::unsignalled; }
    bool is_executing(void) const override { return m_state == state::signalling; }

    template<typename Fn>
    registration_t operator+=(Fn&& rhs) {
      // Initial check of the flag, we don't even want to bother with the rest
      // of this function if the flag is already set
      if (m_state != state::unsignalled) {
        rhs();
        return{};
      }

      // Move the lambda into our unique pointer outside of the lock to reduce
      // total contention time
      std::unique_ptr<callable<Fn>> fn{
        new callable<Fn>{ std::forward<Fn&&>(rhs) }
      };

      // Double-check the flag under lock:
      {
        std::lock_guard<autowiring::spin_lock> lk(m_spin);
        if (!get()) {
          m_fns.push_back(std::move(fn));
          return{ this, m_fns.back().get() };
        }
      }

      // We got here, the flag was set in the double-check.  We need to call
      // the lambda function, but unfortunately we've already moved it, so we
      // need to call the pointer version of the lambda.
      (*fn)();
      return{};
    }

    // Unlink function
    bool operator-=(registration_t& rhs) override final;

    /// <returns>
    /// True if the flag has been set
    /// </returns>
    operator bool(void) const { return get(); }

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
    once_signal(void) = default;

    once_signal(once_signal&& rhs) :
      once(std::move(rhs))
    {}

    template<typename Fn>
    registration_t operator+=(Fn&& fn) {
      return once::operator+=<Fn>(std::forward<Fn&&>(fn));
    }

    using once::get;
    using once::is_executing;
    using once::operator bool;
    using once::operator-=;

  private:
    // We don't allow these to be called by just anyone, only by T
    using once::operator=;
    using once::operator();
    friend T;
  };
}
