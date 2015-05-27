// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Decompose.h"
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

/// <summary>
/// Implements an asynchronous signal concept as an AutoFired alternative
/// </summary>
namespace autowiring {
  template<typename T>
  struct signal;

  struct registration_t;

  namespace internal {
    struct signal_node_base {};

    template<typename... Args>
    struct signal_node_t_base:
      signal_node_base
    {
      virtual void operator()(Args... args) = 0;
    };

    // Holds a reference to one of the signal holders
    template<typename Fn, typename... Args>
    struct signal_node_t :
      signal_node_t_base<Args...>
    {
      signal_node_t(Fn&& fn) : fn(fn) {}
      const Fn fn;

      virtual void operator()(Args... args) override {
        fn(args...);
      }
    };
  }

  struct signal_base {
    /// <summary>
    /// Removes the signal node identified on the rhs without requiring full type information
    /// </summary>
    virtual void operator-=(internal::signal_node_base* rhs);

    /// <summary>
    /// Removes the specified registration
    /// </summary>
    void operator-=(const registration_t& reg);
  };

  struct registration_t
  {
    registration_t(void) = default;

    registration_t(signal_base* parent, internal::signal_node_base* entry) :
      parent(parent),
      entry(entry)
    {}

    signal_base* parent = nullptr;
    internal::signal_node_base* entry;

    explicit operator bool(void) const { return !!parent; }

    bool operator==(const registration_t& rhs) const {
      return parent == rhs.parent && entry == rhs.entry;
    }

    void reset(void) {
      if (parent)
        *parent -= entry;
      parent = nullptr;
    }
  };

  /// <summary>
  /// A signal registration entry, for use as an embedded member variable of a context member.
  /// </summary>
  template<typename... Args>
  struct signal<void(Args...)>:
    signal_base
  {
    typedef std::shared_ptr<internal::signal_node_t_base<Args...>> entry_t;

  private:
    // Listeners and the corresponding lock:
    mutable std::mutex m_lock;
    std::vector<entry_t> m_listeners;

    // Perfect match override:
    template<typename Fn>
    entry_t make_registration(Fn fn, void (Fn::*)(Args...) const) {
      return std::make_shared<
        internal::signal_node_t<Fn, Args...>
      >(std::forward<Fn>(fn));
    }

    template<class Fn>
    struct Forwarder:
      internal::signal_node_t_base<Args...>
    {
      Forwarder(signal* parent, Fn&& fn) :
        parent(parent),
        fn(std::forward<Fn&&>(fn))
      {}

      signal*const parent;
      const Fn fn;

      void operator()(Args... args) override {
        registration_t reg{parent, this};
        fn(&reg, args...);
      }
    };

    template<typename Fn>
    entry_t make_registration(Fn fn, void (Fn::*)(registration_t*, Args...) const) {
      return std::make_shared<Forwarder<Fn>>(this, std::forward<Fn>(fn));
    }

  public:

    /// <summary>
    /// Attaches the specified handler to this signal
    /// </summary>
    template<typename Fn>
    registration_t operator+=(Fn fn) {
      auto entry = make_registration<Fn>(std::forward<Fn>(fn), &Fn::operator());

      std::lock_guard<std::mutex> lk(m_lock);
      m_listeners.push_back(entry);
      return {this, entry.get()};
    }

    void operator-=(internal::signal_node_base* rhs) override {
      std::lock_guard<std::mutex> lk(m_lock);
      for (size_t i = 0; i < m_listeners.size(); i++)
        if (m_listeners[i].get() == rhs) {
          m_listeners[i] = m_listeners.back();
          m_listeners.pop_back();
          return;
        }
      throw std::runtime_error("Attempted to remove node which is not part of this list.");
    }

    using signal_base::operator-=;

    /// <summary>
    /// Unregisters the specified registration object and clears its status
    /// </summary>
    /// <remarks>
    /// This method does not guarantee that the named registration object will not be called after
    /// this method returns in multithreaded cases.  The handler is only guaranteed not to be called
    /// if `rhs.unique()` is true.
    /// </remarks>
    void operator-=(entry_t rhs) {
      *this -= rhs.get();
    }

    /// <summary>
    /// Raises the signal and invokes all attached handlers
    /// </summary>
    void operator()(Args... args) const {
      // Take a copy of the callee set:
      for (auto& callee : ((std::lock_guard<std::mutex>)m_lock, m_listeners))
        (*callee)(args...);
    }
  };
}
