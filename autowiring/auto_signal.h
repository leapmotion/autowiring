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

  // Current state of the signal, used as a type of lock.
  enum class SignalState {
    // Signal is totally idle, this is the default state
    Free,

    // Someone is presently inserting or removing a listener
    Updating,

    // Signal is being asserted in some thread
    Asserting,

    // Signal is being asserted, and there is deferred work to be done
    Deferred,
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
    mutable std::atomic<SignalState> m_state{ SignalState::Free };

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
      Fn fn;
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

    // Calls that had to be delayed due to asynchronous issues
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
      entry_base* entry;
      void operator()() override {
        if(entry)
          owner.LinkUnsafe(*entry);
      }
    };

    void Link(entry_base* e) {
      SignalState state = SignalState::Free;

      // We don't mind rare failures, here, because our algorithm is correct regardless
      // of the return value of this comparison, it's just slightly more efficient if
      // there is no failure.
      if (!m_state.compare_exchange_weak(state, SignalState::Updating)) {
        // Control is contended, we need to hand off linkage responsibility to someone else.
        auto link = new callable_link{ *this, e };
        CallLater(link);

        for (;;) {
          // Try to transition from Asserting to the Deferred state first, if we succeed here
          // then the call operator will take responsibility for our insertion request.
          state = SignalState::Asserting;
          if (m_state.compare_exchange_weak(state, SignalState::Deferred))
            return;

          // If we observed the state as being Deferred, then we can also short-circuit
          if (state == SignalState::Deferred)
            return;

          // Next try to transition from Free to Updating.
          state = SignalState::Free;
          if (m_state.compare_exchange_weak(state, SignalState::Updating)) {
            // Success, cancel the operation and exit here.  We can't delete this link because
            // it has already been submitted to the queue, but calling it has no effect and it
            // will be cleaned up later.
            link->entry = nullptr;
            break;
          }
        }
      }

      // Link and go straight to the Free state.  Updating is exclusive.
      LinkUnsafe(*e);
      m_state = SignalState::Free;
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

      // Neighbor unlink:
      if (entry.pFlink)
        entry.pFlink->pBlink = entry.pBlink;
      if (entry.pBlink)
        entry.pBlink->pFlink = entry.pFlink;
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
      void operator()() override {
        if(entry)
          owner.UnlinkUnsafe(*entry);
      }
    };

    /// <summary>
    /// Removes the specified entry from the set of listeners
    /// </summary>
    void Unlink(std::unique_ptr<entry_base> e) {
      // See discussion in Link
      SignalState state = SignalState::Free;
      if (!m_state.compare_exchange_weak(state, SignalState::Updating)) {
        auto link = new callable_unlink(*this, std::move(e));
        CallLater(link);

        for (;;) {
          state = SignalState::Asserting;
          if (m_state.compare_exchange_weak(state, SignalState::Deferred))
            return;
          if (state == SignalState::Deferred)
            return;

          state = SignalState::Free;
          if (m_state.compare_exchange_weak(state, SignalState::Updating)) {
            e = std::move(link->entry);
            break;
          }
        }
      }
      UnlinkUnsafe(*e);
      m_state = SignalState::Free;
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

    /// <summary>
    /// Appends a callable to be invoked later
    /// </summary>
    void CallLater(detail::callable_base* pCallable) const {
      detail::callable_base* newFlink;
      do {
        newFlink = m_pFirstDelayedCall.load(std::memory_order_relaxed);
        pCallable->m_pFlink = newFlink;
      } while (
        !m_pFirstDelayedCall.compare_exchange_weak(
          newFlink,
          pCallable,
          std::memory_order_acq_rel,
          std::memory_order_relaxed
        )
      );
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
      for (;;) {
        SignalState state = SignalState::Free;
        if (m_state.compare_exchange_weak(state, SignalState::Asserting, std::memory_order_release, std::memory_order_relaxed)) {
          // Can safely make this call under state because there are never any blocking
          // operations involving this lock.  We cannot let exceptions propagate out of
          // here because we might not actually be able to give up control.
          try {
            SignalUnsafe(std::forward<FnArgs>(args)...);
          } catch(...) {}
          break;
        }
        
        switch (state) {
        case SignalState::Free:
        case SignalState::Updating:
          // Spurious failure, or insertion.
          // We cannot delegate control to insertion, and spurious failure should be retried.
          continue;
          case SignalState::Asserting:
          case SignalState::Deferred:
          // Some other thread is already asserting this signal, doing work, we need to ensure
          // that thread takes responsibility for this call.
          break;
        }

        // We failed to obtain control.  Create a thunk here.
        CallLater(
          new callable_signal<FnArgs...>{
            *this,
            std::forward<FnArgs&&>(args)...
          }
        );

        // Now ensure that someone is going to take responsibility to make this invocation
        do {
          state = SignalState::Asserting;
          if (m_state.compare_exchange_weak(state, SignalState::Deferred))
            // Success, we have transferred responsibility for this call to the other entity.
            return;
          if (state == SignalState::Deferred)
            // Failure, egg came back on our face.  Responsibility was reflected back on to us.
            // Take charge.
            break;
          state = SignalState::Free;
        } while (!m_state.compare_exchange_weak(state, SignalState::Asserting));

        break;
      }

      if (m_state == SignalState::Free) return;

      // Great.  If we got here it's because everyone got away and we're left holding the bag.
      // We thought we were going to need to pend a callable signal but instead we're still going
      // to need to invoke it ourselves.
      // Try to get out of the asserting state and into the free state
      for(
        SignalState state = SignalState::Asserting;
        !m_state.compare_exchange_weak(state, SignalState::Free, std::memory_order_release, std::memory_order_relaxed);
        state = SignalState::Asserting
      ) {
        // Failed to give up control.  Our state was deferred or the delayed call list was non-empty,
        // we need to transition to the Asserting state and empty our list before proceeding
        state = SignalState::Deferred;
        m_state.compare_exchange_strong(state, SignalState::Asserting);
        if (state == SignalState::Asserting)
          // Spurious failure, circle around
          continue;

        // Take ownership of the dispatcher list and reverse this forward-linked list.
        detail::callable_base* lastLink = nullptr;
        for (
          detail::callable_base* cur = m_pFirstDelayedCall.exchange(nullptr, std::memory_order_acquire);
          cur;
          std::swap(cur, lastLink)
        )
          std::swap(lastLink, cur->m_pFlink);

        // Call all dispatchers in the right order
        for (std::unique_ptr<detail::callable_base> cur{ lastLink }; cur; cur.reset(cur->m_pFlink))
          // Eat any exception that occurs
          try { (*cur)(); }
          catch(...) {}
      }
    }

    // This overload is provided so that statement completion makes sense.  Because of the
    // template format of the earlier function call overload, overload resolution will never
    // select this variant.
    //void operator()(Args... args) const;
  };
}
