// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_tuple.h"
#include "autowiring_error.h"
#include "Decompose.h"
#include "index_tuple.h"
#include "noop.h"
#include "spin_lock.h"
#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include TYPE_TRAITS_HEADER

/// <summary>
/// Implements an asynchronous signal concept as an AutoFired alternative
/// </summary>
namespace autowiring {
  template<typename T>
  struct signal;

  struct registration_t;

  namespace detail {
    // Holds true if type T can be copied safely
    template<typename T>
    struct can_copy {
      static const bool value =
        !std::is_abstract<T>::value &&
        std::is_copy_constructible<T>::value;
    };

    // Utility type for handling dereferencing of an input value
    template<typename T, typename = void>
    struct dereferencer
    {
      static_assert(std::is_copy_constructible<T>::value, "T must be copy constructable");
      static_assert(!std::is_abstract<T>::value, "T cannot be abstract");

      dereferencer(dereferencer&& rhs) : val(std::move(rhs.val)) {}
      dereferencer(T&& val) : val(std::move(val)) {}
      T val;
      const T& operator*(void) const { return val; }
    };

    template<typename T>
    struct dereferencer<T&&, void>
    {
      dereferencer(dereferencer&& rhs) : val(std::move(rhs.val)) {}
      dereferencer(T&& val) : val(std::move(val)) {}
      T val;
      const T& operator*(void) const { return val; }
    };

    /// <summary>
    /// Copyable reference argument
    /// </summary>
    template<typename T>
    struct dereferencer<T&, typename std::enable_if<!can_copy<T>::value>::type> {
      dereferencer(const dereferencer& rhs) : val(rhs.val) {}
      dereferencer(const T& val) : val(val) {}
      const T& val;
      const T& operator*(void) const { return val; }
    };

    /// <summary>
    /// Non-copyable reference argument
    /// </summary>
    template<typename T>
    struct dereferencer<T&, typename std::enable_if<can_copy<T>::value>::type> {
      dereferencer(dereferencer&& rhs) : val(std::move(rhs.val)) {}
      dereferencer(const T& val) : val(val) {}
      T val;
      const T& operator*(void) const { return val; }
    };

    // Callable wrapper type, always invoked in a synchronized context
    struct callable_base {
      virtual ~callable_base(void) {}
      virtual void operator()() = 0;
      callable_base* m_pFlink = nullptr;
    };

    template<typename Fn>
    struct callable :
      callable_base
    {
      callable(Fn&& fn) : fn(std::move(fn)) {}
      Fn fn;
      void operator()() override { fn(); }
    };
  }

  struct signal_base {
    /// <summary>
    /// Removes the signal node identified on the rhs without requiring full type information
    /// </summary>
    /// <remarks>
    /// This operation invalidates the specified unique pointer.  If the passed unique pointer is
    /// already nullptr, this operation has no effect.
    /// </remarks>
    virtual void operator-=(registration_t& rhs) = 0;
  };

  struct registration_t {
    registration_t(void) = default;

    registration_t(signal_base* owner, void* pobj) :
      owner(owner),
      pobj(pobj)
    {}

    registration_t(registration_t&& rhs) :
      owner(rhs.owner),
      pobj(rhs.pobj)
    {
      rhs.pobj = nullptr;
    }

    registration_t(const registration_t& rhs) = delete;

    signal_base* owner;
    void* pobj;

    bool operator==(const registration_t& rhs) const { return pobj == rhs.pobj; }

    void operator=(registration_t&& rhs) {
      owner = rhs.owner;
      pobj = rhs.pobj;
      rhs.pobj = nullptr;
    }

    operator bool(void) const { return pobj != nullptr; }
  };

  /// <summary>
  /// A signal registration entry, for use as an embedded member variable of a context member.
  /// </summary>
  template<typename... Args>
  struct signal<void(Args...)>:
    signal_base
  {
  public:
    signal(void) {}
    signal(const signal&) = delete;

    signal(signal&& rhs) :
      m_pFirstListener(rhs.m_pFirstListener),
      m_pLastListener(rhs.m_pLastListener)
    {
      if (rhs.m_pFirstDelayedCall.load())
        throw autowiring_error("Attempted to move a signal where pended lambdas still exist");
      rhs.m_pFirstListener = nullptr;
      rhs.m_pLastListener = nullptr;
    }

    ~signal(void) {
      {
        detail::callable_base* prior = nullptr;
        for (detail::callable_base* cur = m_pFirstDelayedCall; cur; cur = cur->m_pFlink) {
          delete prior;
          prior = cur;
        }
      }
      {
        entry_base* prior = nullptr;
        for (entry_base* cur = m_pFirstListener; cur; cur = cur->pFlink) {
          delete prior;
          prior = cur;
        }
      }
    }

    signal& operator=(signal&& rhs) {
      std::swap(m_pFirstListener, rhs.m_pFirstListener);
      std::swap(m_pLastListener, rhs.m_pLastListener);
      return *this;
    }

  private:
    // Lock held by whomever is currently invoking signal handlers
    mutable autowiring::spin_lock m_callLock;

    // Base type for listeners attached to this signal
    struct entry_base {
      virtual ~entry_base(void) {}
      virtual void operator()(const Args&... args) = 0;

      entry_base* pFlink = nullptr;
      entry_base* pBlink = nullptr;
    };

    template<typename Fn>
    struct entry:
      entry_base
    {
      static_assert(!std::is_reference<Fn>::value, "Cannot construct a reference binding");

      template<typename _Fn>
      entry(signal&, _Fn fn) : fn(std::forward<_Fn>(fn)) {}
      const Fn fn;
      void operator()(const Args&... args) override { fn(args...); }
    };

    template<typename Fn>
    struct entry_reflexive :
      entry_base
    {
      static_assert(!std::is_reference<Fn>::value, "Cannot construct a reference binding");

      template<typename _Fn>
      entry_reflexive(signal& owner, _Fn fn) :
        owner(owner),
        fn(std::forward<_Fn>(fn))
      {}
      signal& owner;
      const Fn fn;
      void operator()(const Args&... args) override { fn(registration_t{ &owner, this }, args...); }
    };

    // Doubly linked list of all of our listeners
    entry_base* m_pFirstListener = nullptr;
    entry_base* m_pLastListener = nullptr;

    // Calls that had to be delayed because the lock was held
    mutable std::atomic<detail::callable_base*> m_pFirstDelayedCall{ nullptr };

    void LinkUnsafe(entry_base& e) {
      // Standard, boring linked list insertion:
      e.pBlink = m_pLastListener;
      if (m_pLastListener)
        m_pLastListener->pFlink = &e;
      if (!m_pFirstListener)
        m_pFirstListener = &e;
      m_pLastListener = &e;
    }

    struct callable_link :
      detail::callable_base
    {
      callable_link(signal& owner, entry_base* entry) :
        owner(owner),
        entry(std::move(entry))
      {}

      signal& owner;
      entry_base*const entry;
      void operator()() override { owner.LinkUnsafe(*entry); }
    };

    void Link(entry_base* e) {
      std::unique_lock<autowiring::spin_lock> callLk(m_callLock, std::try_to_lock);
      if (callLk)
        LinkUnsafe(*e);
      else
        CallLater(new callable_link{ *this, e });
    }

    void UnlinkUnsafe(const entry_base& entry) {
      if (m_pFirstListener == &entry) {
        m_pFirstListener = m_pFirstListener->pFlink;
        if (m_pFirstListener)
          m_pFirstListener->pBlink = nullptr;
      }
      if (m_pLastListener == &entry) {
        m_pLastListener = m_pLastListener->pBlink;
        if (m_pLastListener)
          m_pLastListener->pFlink = nullptr;
      }
    }

    struct callable_unlink :
      detail::callable_base
    {
      callable_unlink(signal& owner, std::unique_ptr<entry_base>&& entry) :
        owner(owner),
        entry(std::move(entry))
      {}

      signal& owner;
      std::unique_ptr<entry_base> entry;
      void operator()() override { owner.UnlinkUnsafe(*entry); }
    };

    /// <summary>
    /// Removes the specified entry from the set of listeners
    /// </summary>
    void Unlink(std::unique_ptr<entry_base> entry) {
      // Try to obtain the call lock.  If a call is already underway then we need to delegate cleanup
      // responsibility to that call.
      std::unique_lock<autowiring::spin_lock> callLk(m_callLock, std::try_to_lock);
      if (callLk)
        // Acquired the call lock, now we need the list lock so we can make changes to the list
        // The list lock is only held under insertion and deletion, not enumeration, and so we
        // do not need to worry about anyone holding outstanding counts anywhere.
        UnlinkUnsafe(*entry);
      else
        // Need to make an unregistration request, append it to our list of callables
        CallLater(new callable_unlink(*this, std::move(entry)));
    }

    /// <summary>
    /// Sequential signaling mechanism, invoked under the call lock
    /// </summary>
    void SignalUnsafe(Args... args) const {
      for (auto cur = m_pFirstListener; cur; cur = cur->pFlink)
        (*cur)(args...);
    }

    template<typename... FnArgs>
    struct callable_signal :
      detail::callable_base
    {
      callable_signal(const signal& owner, FnArgs&&... args) :
        owner(owner),
        args(std::forward<FnArgs&&>(args)...)
      {}

      const signal& owner;
      autowiring::tuple<detail::dereferencer<FnArgs&&>...> args;

      template<int... N>
      void call(index_tuple<N...>) {
        owner.SignalUnsafe(*autowiring::get<N>(args)...);
      }

      void operator()() override {
        call(typename make_index_tuple<sizeof...(FnArgs)>::type{});
      }
    };

    void InvokeDelayedCalls(void) const {
      // If there were any other dispatchers, obtain them and invoke all of them:
      while (m_pFirstDelayedCall.load(std::memory_order_relaxed)) {
        std::unique_lock<autowiring::spin_lock> callLk(m_callLock, std::try_to_lock);
        if (!callLk)
          return;

        // Flip links on the dispatchers so they are called in the right order:
        detail::callable_base* lastLink = nullptr;
        for (
          detail::callable_base* cur = m_pFirstDelayedCall.exchange(nullptr, std::memory_order_relaxed);
          cur;
          std::swap(cur, lastLink)
        )
          std::swap(lastLink, cur->m_pFlink);

        // Call all dispatchers in the right order
        for (std::unique_ptr<detail::callable_base> cur{ lastLink }; cur; cur.reset(cur->m_pFlink))
          (*cur)();
      }
    }

    /// <summary>
    /// Appends a callable to be invoked later
    /// </summary>
    void CallLater(detail::callable_base* pCallable) const {
      do pCallable->m_pFlink = m_pFirstDelayedCall.load(std::memory_order_relaxed);
      while (!m_pFirstDelayedCall.compare_exchange_weak(pCallable->m_pFlink, pCallable, std::memory_order_release, std::memory_order_relaxed));
    }

  public:
    /// <summary>
    /// Attaches the specified handler to this signal
    /// </summary>
    /// <remarks>
    /// If the return value is not captured, the signal cannot be unregistered.  Users are not required
    /// to free this object.
    /// </remarks>
    template<typename Fn>
    registration_t operator+=(Fn fn) {
      typedef typename std::decay<Fn>::type FnDecay;
      typedef typename std::conditional<
        Decompose<decltype(&FnDecay::operator())>::N == sizeof...(Args),
        entry<FnDecay>,
        entry_reflexive<FnDecay>
      >::type EntryType;

      auto* e = new EntryType(*this, std::forward<Fn&&>(fn));
      Link(e);
      return{ this, e };
    }

    /// <summary>
    /// Unregisters the specified registration object and clears its status
    /// </summary>
    /// <remarks>
    /// This method does not guarantee that the named registration object will not be called after
    /// this method returns in multithreaded cases.  The handler is only guaranteed not to be called
    /// if `rhs.unique()` is true.
    /// </remarks>
    void operator-=(registration_t& rhs) override {
      if (rhs.owner != this)
        throw autowiring_error("Attempted to unlink a registration on an unrelated signal");
      if (!rhs.pobj)
        return;

      Unlink(std::unique_ptr<entry_base>{ static_cast<entry_base*>(rhs.pobj) });
      rhs.pobj = nullptr;
    }

    /// <summary>
    /// Raises the signal and invokes all attached handlers
    /// </summary>
    /// <param name="args">
    template<typename... FnArgs>
    void operator()(FnArgs&&... args) const {
      {
        std::unique_lock<autowiring::spin_lock> callLk(m_callLock, std::try_to_lock);
        if (callLk)
          // Can safely make this call under lock because there are never any blocking
          // operations involving this lock.
          SignalUnsafe(std::forward<FnArgs>(args)...);
        else {
          // Need to create a dispatcher thunk for this call and then pend it to the list, we are not
          // responsible for calling this dispatcher
          CallLater(
            new callable_signal<FnArgs...>{
              *this,
              std::forward<FnArgs&&>(args)...
            }
          );
        }
      }
      InvokeDelayedCalls();
    }

    // This overload is provided so that statement completion makes sense.  Because of the
    // template format of the earlier function call overload, overload resolution will never
    // select this variant.
    //void operator()(Args... args) const;
  };
}
