// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _EVENT_MANAGER_H
#define _EVENT_MANAGER_H
#include "ocuConfig.h"
#include "DispatchQueue.h"
#include "EventDispatcher.h"
#include "EventReceiver.h"
#include "LockFreeList.h"
#include "LockReducedCollection.h"
#include "SharedPtrHash.h"
#include <boost/thread/shared_mutex.hpp>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include SHARED_PTR_HEADER
#include STL_UNORDERED_SET
#include TYPE_TRAITS_HEADER
#include <set>

class EventSenderBase;
class EventReceiver;

/// <summary>
/// Service routine called inside Fire calls in order to decide how to handle an exception
/// </summary>
/// <remarks>
/// This routine MUST NOT be called outside of a "catch" handler.  This function, and the functions that
/// it call, rely on the validity of the std::current_exception return value, which will not be valid
/// outside of a call block.
/// </remarks>
void FilterFiringException(const EventSenderBase* pSender, EventReceiver* pRecipient);

/// <summary>
/// Used to identify event managers
/// </summary>
class EventSenderBase {
public:
  virtual ~EventSenderBase(void);

public:
  virtual bool HasListeners(void) const = 0;

  /// <summary>
  /// Invoked by the parent context when the context is shutting down in order to release all references
  /// </summary>
  virtual void ReleaseRefs(void) = 0;

  // Event attachment and detachment pure virtuals
  virtual EventSenderBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) = 0;
  virtual EventSenderBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) = 0;
};

struct NoType {};

template<class T>
class EventSenderSingle
{
public:
  virtual ~EventSenderSingle(void) {}

protected:
  static_assert(
    std::is_base_of<EventReceiver, T>::value,
    "If you want an event interface, the interface must inherit from EventReceiver"
  );

  // Reader-writer lock:
  boost::shared_mutex m_rwLock;

  // Collection of all known listeners:
  typedef LockReducedCollection<std::shared_ptr<T>, SharedPtrHash<T>> t_listenerSet;
  t_listenerSet m_st;

  // Collection of all transient listeners:
  typedef LockFreeList<std::weak_ptr<T>> t_transientSet;
  t_transientSet m_stTransient;

  // Just the DispatchQueue listeners:
  typedef std::unordered_set<DispatchQueue*> t_stType;
  t_stType m_dispatch;

public:
  /// <summary>
  /// Convenience method allowing consumers to quickly determine whether any listeners exist
  /// </summary>
  bool HasListeners(void) const {return !m_st.GetImage()->empty();}

  void ReleaseRefs() {
    m_st.Clear();
    m_dispatch.clear();
  }

  void operator+=(const std::shared_ptr<EventReceiver>& rhs) {
    std::shared_ptr<T> casted = std::dynamic_pointer_cast<T, EventReceiver>(rhs);
    if(casted)
      *this += casted;
  }

  void operator-=(const std::shared_ptr<EventReceiver>& rhs) {
    std::shared_ptr<T> casted = std::dynamic_pointer_cast<T, EventReceiver>(rhs);
    if(casted)
      *this -= casted;
  }

  /// <summary>
  /// Adds the specified observer to receive events dispatched from this instace
  /// </summary>
  void operator+=(const std::shared_ptr<T>& rhs) {
    // Trivial insertion
    m_st.Insert(rhs);

    // If the RHS implements DispatchQueue, add it to that collection as well:
    DispatchQueue* pDispatch = dynamic_cast<DispatchQueue*>(rhs.get());
    if(pDispatch)
      m_dispatch.insert(pDispatch);
  }

  /// <summary>
  /// Removes the specified observer from the set currently configured to receive events
  /// </summary>
  void operator-=(const std::shared_ptr<T>& rhs) {
    // Trivial removal:
    auto nErased = m_st.Erase(rhs);
    if(!nErased)
      return;

    // If the RHS implements DispatchQueue, add it to that collection as well:
    DispatchQueue* pDispatch = dynamic_cast<DispatchQueue*>(rhs.get());
    if(pDispatch)
      m_dispatch.erase(pDispatch);
  }

  /// <summary>
  /// Convenience routine for Fire calls
  /// </summary>
  /// <remarks>
  /// This is a convenience routine, its only purpose is to add the "this" parameter to the
  /// call to FilterFiringException
  /// </remarks>
  inline void PassFilterFiringException(EventReceiver* pReceiver) const {
    FilterFiringException(dynamic_cast<const EventSenderBase*>(this), pReceiver);
  }

  // Convenience defines for Fire overloads, consider removing
  #define FIRE_CATCHER_START try {
  #define FIRE_CATCHER_END } catch(...) { this->PassFilterFiringException((*q).get()); }

  // Zero-argument deferred call:
  template<class Fn>
  void FireCurried(Fn&& fn) const {
    // Held names first:
    {
      auto st = m_st.GetImage();
      for(auto q = st->begin(); q != st->end(); ++q)
        try {
          fn(**q);
        } catch(...) {
          this->PassFilterFiringException((*q).get());
        }
    }

    // Weak names next:
    m_stTransient.Enumerate([&fn] (std::weak_ptr<T>& ptrWeak) {
      auto ptr = ptrWeak.lock();
      if(ptr)
        fn(*ptr);
    });
  }

protected:
  // Two-parenthetical invocations
  std::function<void ()> Fire(void (T::*fnPtr)()) const {
    return
      [this, fnPtr] () {
        FireCurried(
          [=] (T& obj) {
            (obj.*fnPtr)();
          }
        );
      };
  }

  template<class Arg1>
  std::function<void (const Arg1&)> Fire(void (T::*fnPtr)(Arg1)) const {
    return
      [this, fnPtr] (const Arg1& arg1) {
        auto fnPtrCpy = fnPtr;
        auto arg1Ptr = &arg1;
        FireCurried(
          [fnPtrCpy, arg1Ptr] (T& obj) {
            (obj.*fnPtrCpy)(*arg1Ptr);
          }
        );
      };
  }

  template<class Arg1, class Arg2>
  std::function<void (const Arg1&, const Arg2&)> Fire(void (T::*fnPtr)(Arg1, Arg2)) const {
    return
      [this, fnPtr] (const Arg1& arg1, const Arg2& arg2) {
        auto fnPtrCpy = fnPtr;
        auto arg1Ptr = &arg1;
        auto arg2Ptr = &arg2;
        FireCurried(
          [fnPtrCpy, arg1Ptr, arg2Ptr] (T& obj) {
            (obj.*fnPtrCpy)(*arg1Ptr, *arg2Ptr);
          }
        );
      };
  }

  template<class Arg1, class Arg2, class Arg3>
  std::function<void (const Arg1&, const Arg2&, const Arg3&)> Fire(void (T::*fnPtr)(Arg1, Arg2, Arg3)) const {
    return
      [this, fnPtr] (Arg1 arg1, Arg2 arg2, Arg3 arg3) {
        auto fnPtrCpy = fnPtr;
        auto arg1Ptr = &arg1;
        auto arg2Ptr = &arg2;
        auto arg3Ptr = &arg3;
        FireCurried(
          [fnPtrCpy, arg1Ptr, arg2Ptr, arg3Ptr] (T& obj) {
            (obj.*fnPtrCpy)(*arg1Ptr, *arg2Ptr, *arg3Ptr);
          }
        );
      };
  }

  template<class Arg1, class Arg2, class Arg3, class Arg4>
  std::function<void (const Arg1&, const Arg2&, const Arg3&, const Arg4&)> Fire(void (T::*fnPtr)(Arg1, Arg2, Arg3, Arg4)) const {
    return
      [this, fnPtr] (Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
        auto fnPtrCpy = fnPtr;
        auto arg1Ptr = &arg1;
        auto arg2Ptr = &arg2;
        auto arg3Ptr = &arg3;
        auto arg4Ptr = &arg4;
        FireCurried(
          [fnPtrCpy, arg1Ptr, arg2Ptr, arg3Ptr, arg4Ptr] (T& obj) {
            (obj.*fnPtrCpy)(*arg1Ptr, *arg2Ptr, *arg3Ptr, *arg4Ptr);
          }
        );
      };
  }

  template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
  std::function<void (const Arg1&, const Arg2&, const Arg3&, const Arg4&, const Arg5&)> Fire(void (T::*fnPtr)(Arg1, Arg2, Arg3, Arg4, Arg5)) const {
    return
      [this, fnPtr] (Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
        auto fnPtrCpy = fnPtr;
        auto arg1Ptr = &arg1;
        auto arg2Ptr = &arg2;
        auto arg3Ptr = &arg3;
        auto arg4Ptr = &arg4;
        auto arg5Ptr = &arg5;
        FireCurried(
          [fnPtrCpy, arg1Ptr, arg2Ptr, arg3Ptr, arg4Ptr, arg5PTr] (T& obj) {
            (obj.*fnPtrCpy)(*arg1Ptr, *arg2Ptr, *arg3Ptr, *arg4Ptr, *arg5Ptr);
          }
        );
      };
  }

  // Two-parenthetical deferred invocations:
  std::function<void ()> Defer(void (T::*fnPtr)()) const {
    return
      [this, fnPtr] () {
        auto f = fnPtr;
        for(auto q = m_dispatch.begin(); q != m_dispatch.end(); q++) {
          T* ptr = dynamic_cast<T*>(*q);

          // EventDispatcher check:
          EventDispatcher* pDispatchable = dynamic_cast<EventDispatcher*>(ptr);
          if(!pDispatchable || !pDispatchable->CanAccept())
            continue;

          **q += [ptr, f] () {
            (ptr->*f)();
          };
        }
      };
  }

  template<class Arg1>
  std::function<void (const typename std::decay<Arg1>::type&)> Defer(void (T::*fnPtr)(Arg1)) const {
    // Converted args:
    typedef typename std::decay<Arg1>::type tArg1;

    return
      [this, fnPtr] (const tArg1& arg1) {
        auto f = fnPtr;
        for(EventSenderSingle<T>::t_stType::const_iterator q = m_dispatch.begin(); q != m_dispatch.end(); q++) {
          T* ptr = dynamic_cast<T*>(*q);

          // If the pointer cannot support a EventDispatcher, we _cannot_ defer it
          // TODO:  Consider constructing a separate set just containing EventDispatcher-castables.  Whether an object can
          // be cast to a particular type is declarative, anyway.
          EventDispatcher* pDispatchable = dynamic_cast<EventDispatcher*>(ptr);
          if(!pDispatchable || !pDispatchable->CanAccept())
            continue;

          // Force off the const modifier so we can copy into the lambda just once
          tArg1& arg1Forced = (tArg1&)arg1;

          // Pass the copy into the lambda:
          **q += [ptr, f, arg1Forced] () mutable {
            (ptr->*f)(
              std::move(arg1Forced)
            );
          };
        }
      };
  }
};

template<>
class EventSenderSingle<NoType> {
};

/// <summary>
/// A simple event manager class
/// </summary>
/// <param name="T">The event interface type</param>
/// <param name="A">Another event interface type to manage</param>
/// <param name="B">Another event interface type to manage</param>
/// <param name="C">Another event interface type to manage</param>
template<class T, class A = NoType, class B = NoType, class C = NoType>
class EventSender;

template<class T, class A, class B, class C>
class EventSender:
  public EventSender<A, B, C>,
  public EventSenderSingle<T>
{
private:
  typedef EventSender<A, B, C> t_base;

  template<class MemFn>
  struct Decompose;

  template<class W>
  struct Decompose<void (W::*)()> {
    typedef void fnType();
    typedef W type;
  };

  template<class W, class Arg1>
  struct Decompose<void (W::*)(Arg1)> {
    typedef void fnType(Arg1);
    typedef W type;
  };

  template<class W, class Arg1, class Arg2>
  struct Decompose<void (W::*)(Arg1, Arg2)> {
    typedef void fnType(Arg1, Arg2);
    typedef W type;
  };

public:
  bool HasListeners(void) const override {return EventSenderSingle<T>::HasListeners() || t_base::HasListeners();}

  template<class W>
  bool HasListeners(void) const {
    return
      std::is_same<W, T>::value ?
      EventSenderSingle<T>::HasListeners() :
      t_base::HasListeners();
  }

  virtual void ReleaseRefs(void) override {
    t_base::ReleaseRefs();
    EventSenderSingle<T>::ReleaseRefs();
  }

  // Event attachment and detachment virtuals
  virtual EventSenderBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) override {
    t_base::operator+=(rhs);
    EventSenderSingle<T>::operator+=(rhs);
    return *this;
  }

  virtual EventSenderBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) override {
    t_base::operator-=(rhs);
    EventSenderSingle<T>::operator-=(rhs);
    return *this;
  }

  template<class MemFn>
  std::function<typename Decompose<MemFn>::fnType> Fire(MemFn fn) {
    return EventSenderSingle<typename Decompose<MemFn>::type>::Fire(fn);
  }

  template<class MemFn>
  std::function<typename Decompose<MemFn>::fnType> Defer(MemFn fn) {
    return EventSenderSingle<typename Decompose<MemFn>::type>::Defer(fn);
  }
};

template<class T>
class EventSender<T, NoType, NoType, NoType>:
  public EventSenderSingle<T>,
  public EventSenderBase
{
public:
  virtual bool HasListeners(void) const override {return EventSenderSingle<T>::HasListeners();}

  template<class W>
  bool HasListeners(void) const {
    static_assert(std::is_same<W, T>::value, "Cannot query listeners on unbound type W");
    return EventSenderSingle<T>::HasListeners();
  }

  virtual void ReleaseRefs(void) override {
    EventSenderSingle<T>::ReleaseRefs();
  }

  // Event attachment and detachment pure virtuals
  virtual EventSenderBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) override {
    EventSenderSingle<T>::operator+=(rhs);
    return *this;
  }

  virtual EventSenderBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) override {
    EventSenderSingle<T>::operator-=(rhs);
    return *this;
  }

protected:
  using EventSenderSingle<T>::Fire;
};

#endif
