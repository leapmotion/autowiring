// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _EVENT_MANAGER_H
#define _EVENT_MANAGER_H
#include "ocuConfig.h"
#include "DispatchQueue.h"
#include "EventDispatcher.h"
#include "EventReceiver.h"
#include "SharedPtrHash.h"
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include SHARED_PTR_HEADER
#include STL_UNORDERED_SET
#include TYPE_TRAITS_HEADER
#include <set>

class EventManagerBase;
class EventReceiver;

/// <summary>
/// Service routine called inside Fire calls in order to decide how to handle an exception
/// </summary>
void FilterFiringException(const EventManagerBase* pSender, EventReceiver* pRecipient);

/// <summary>
/// Used to identify event managers
/// </summary>
class EventManagerBase {
public:
  virtual ~EventManagerBase(void);

public:
  virtual bool HasListeners(void) const = 0;

  /// <summary>
  /// Invoked by the parent context when the context is shutting down in order to release all references
  /// </summary>
  virtual void ReleaseRefs(void) = 0;

  // Event attachment and detachment pure virtuals
  virtual EventManagerBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) = 0;
  virtual EventManagerBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) = 0;
};

struct NoType {};

template<class T>
class EventManagerSingle
{
public:
  virtual ~EventManagerSingle(void) {}

protected:
  static_assert(
    std::is_base_of<EventReceiver, T>::value,
    "If you want an event interface, the interface must inherit from EventReceiver"
  );

  // Collection of all known listeners:
  typedef std::unordered_set<std::shared_ptr<T>, SharedPtrHash<T>> t_listenerSet;
  t_listenerSet m_st;
  
  // Just the DispatchQueue listeners:
  typedef std::unordered_set<DispatchQueue*> t_stType;
  t_stType m_dispatch;

public:
  /// <summary>
  /// Convenience method allowing consumers to quickly determine whether any listeners exist
  /// </summary>
  bool HasListeners(void) const {return !m_st.empty();}

  void ReleaseRefs() {
    m_st.clear();
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
  /// </su8mmary>
  void operator+=(const std::shared_ptr<T>& rhs) {
    // Trivial insertion, don't even bother checking for existence:
    m_st.insert(rhs);

    // If the RHS implements DispatchQueue, add it to that collection as well:
    DispatchQueue* pDispatch = dynamic_cast<DispatchQueue*>(rhs.get());
    if(pDispatch)
      m_dispatch.insert(pDispatch);
  }

  /// <summary>
  /// Removes the specified observer from the set currently configured to receive events
  /// </su8mmary>
  void operator-=(const std::shared_ptr<T>& rhs) {
    // Trivial removal:
    size_t nErased = m_st.erase(rhs);
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
    FilterFiringException(dynamic_cast<const EventManagerBase*>(this), pReceiver);
  }

  // Convenience defines for Fire overloads, consider removing
  #define FIRE_CATCHER_START try {
  #define FIRE_CATCHER_END } catch(...) { this->PassFilterFiringException((*q).get()); }

protected:
  // Two-parenthetical invocations
  std::function<void ()> Fire(void (T::*fnPtr)()) const {
    return
      [this, fnPtr] () {
        for(auto q = m_st.begin(); q != m_st.end(); ++q)
          FIRE_CATCHER_START
            (**q.*fnPtr)();
          FIRE_CATCHER_END
      };
  }

  template<class Arg1>
  std::function<void (Arg1)> Fire(void (T::*fnPtr)(Arg1)) const {
    return
      [this, fnPtr] (Arg1 arg1) {
        for(auto q = m_st.begin(); q != m_st.end(); ++q)
          FIRE_CATCHER_START
            (**q.*fnPtr)(arg1);
          FIRE_CATCHER_END
      };
  }

  template<class Arg1, class Arg2>
  std::function<void (Arg1, Arg2)> Fire(void (T::*fnPtr)(Arg1, Arg2)) const {
    return
      [this, fnPtr] (Arg1 arg1, Arg2 arg2) {
        for(auto q = m_st.begin(); q != m_st.end(); ++q)
          FIRE_CATCHER_START
            (**q.*fnPtr)(arg1, arg2);
          FIRE_CATCHER_END
      };
  }

  template<class Arg1, class Arg2, class Arg3>
  std::function<void (Arg1, Arg2, Arg3)> Fire(void (T::*fnPtr)(Arg1, Arg2, Arg3)) const {
    return
      [this, fnPtr] (Arg1 arg1, Arg2 arg2, Arg3 arg3) {
        for(auto q = m_st.begin(); q != m_st.end(); ++q)
          FIRE_CATCHER_START
            (**q.*fnPtr)(arg1, arg2, arg3);
          FIRE_CATCHER_END
      };
  }

  template<class Arg1, class Arg2, class Arg3, class Arg4>
  std::function<void (Arg1, Arg2, Arg3, Arg4)> Fire(void (T::*fnPtr)(Arg1, Arg2, Arg3, Arg4)) const {
    return
      [this, fnPtr] (Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
        for(auto q = m_st.begin(); q != m_st.end(); ++q)
          FIRE_CATCHER_START
            (**q.*fnPtr)(arg1, arg2, arg3, arg4);
          FIRE_CATCHER_END
      };
  }

  template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
  std::function<void (Arg1, Arg2, Arg3, Arg4, Arg5)> Fire(void (T::*fnPtr)(Arg1, Arg2, Arg3, Arg4, Arg5)) const {
    return
      [this, fnPtr] (Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
        for(auto q = m_st.begin(); q != m_st.end(); ++q)
          FIRE_CATCHER_START
            (**q.*fnPtr)(arg1, arg2, arg3, arg4, arg5);
          FIRE_CATCHER_END
      };
  }

  // Two-parenthetical deferred invocations:
  std::function<void ()> Defer(void (T::*fnPtr)()) const {
    return
      [this, fnPtr] () {
        auto f = fnPtr;
        for(EventManagerSingle<T>::t_stType::const_iterator q = m_dispatch.begin(); q != m_dispatch.end(); q++) {
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
        for(EventManagerSingle<T>::t_stType::const_iterator q = m_dispatch.begin(); q != m_dispatch.end(); q++) {
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
class EventManagerSingle<NoType> {
};

/// <summary>
/// A simple event manager class
/// </summary>
/// <param name="T">The event interface type</param>
/// <param name="A">Another event interface type to manage</param>
/// <param name="B">Another event interface type to manage</param>
/// <param name="C">Another event interface type to manage</param>
template<class T, class A = NoType, class B = NoType, class C = NoType>
class EventManager;

template<class T, class A, class B, class C>
class EventManager:
  public EventManager<A, B, C>,
  public EventManagerSingle<T>
{
private:
  typedef EventManager<A, B, C> t_base;

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
  bool HasListeners(void) const override {return EventManagerSingle<T>::HasListeners() || t_base::HasListeners();}

  template<class W>
  bool HasListeners(void) const {
    return
      std::is_same<W, T>::value ?
      EventManagerSingle<T>::HasListeners() :
      t_base::HasListeners();
  }
  
  virtual void ReleaseRefs(void) override {
    t_base::ReleaseRefs();
    EventManagerSingle<T>::ReleaseRefs();
  }

  // Event attachment and detachment virtuals
  virtual EventManagerBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) override {
    t_base::operator+=(rhs);
    EventManagerSingle<T>::operator+=(rhs);
    return *this;
  }

  virtual EventManagerBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) override {
    t_base::operator-=(rhs);
    EventManagerSingle<T>::operator-=(rhs);
    return *this;
  }

  template<class MemFn>
  std::function<typename Decompose<MemFn>::fnType> Fire(MemFn fn) {
    return EventManagerSingle<typename Decompose<MemFn>::type>::Fire(fn);
  }

  template<class MemFn>
  std::function<typename Decompose<MemFn>::fnType> Defer(MemFn fn) {
    return EventManagerSingle<typename Decompose<MemFn>::type>::Defer(fn);
  }
};

template<class T>
class EventManager<T, NoType, NoType, NoType>:
  public EventManagerSingle<T>,
  public EventManagerBase
{
public:
  virtual bool HasListeners(void) const override {return EventManagerSingle<T>::HasListeners();}

  template<class W>
  bool HasListeners(void) const {
    static_assert(std::is_same<W, T>::value, "Cannot query listeners on unbound type W");
    return EventManagerSingle<T>::HasListeners();
  }
  
  virtual void ReleaseRefs(void) override {
    EventManagerSingle<T>::ReleaseRefs();
  }

  // Event attachment and detachment pure virtuals
  virtual EventManagerBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) override {
    EventManagerSingle<T>::operator+=(rhs);
    return *this;
  }

  virtual EventManagerBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) override {
    EventManagerSingle<T>::operator-=(rhs);
    return *this;
  }

protected:
  using EventManagerSingle<T>::Fire;
};

#endif
