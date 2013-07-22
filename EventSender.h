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
#include "TransientPoolBase.h"
#include <boost/thread/shared_mutex.hpp>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include SHARED_PTR_HEADER
#include STL_UNORDERED_SET
#include TYPE_TRAITS_HEADER
#include <set>

class EventReceiverProxyBase;
class EventReceiver;

/// <summary>
/// Service routine called inside Fire calls in order to decide how to handle an exception
/// </summary>
/// <remarks>
/// This routine MUST NOT be called outside of a "catch" handler.  This function, and the functions that
/// it call, rely on the validity of the std::current_exception return value, which will not be valid
/// outside of a call block.
/// </remarks>
void FilterFiringException(const EventReceiverProxyBase* pSender, EventReceiver* pRecipient);

/// <summary>
/// Used to identify event managers
/// </summary>
class EventReceiverProxyBase {
public:
  virtual ~EventReceiverProxyBase(void);

public:
  virtual bool HasListeners(void) const = 0;

  /// <summary>
  /// Invoked by the parent context when the context is shutting down in order to release all references
  /// </summary>
  virtual void ReleaseRefs(void) = 0;

  // Event attachment and detachment pure virtuals
  virtual EventReceiverProxyBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) = 0;
  virtual EventReceiverProxyBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) = 0;
};

struct NoType {};

template<class T>
class EventReceiverProxy:
  public EventReceiverProxyBase
{
public:
  virtual ~EventReceiverProxy(void) {}

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

  // Just the DispatchQueue listeners:
  typedef std::unordered_set<DispatchQueue*> t_stType;
  t_stType m_dispatch;

  // Collection of all transient pools:
  typedef LockReducedCollection<std::shared_ptr<TransientPoolBase>, SharedPtrHash<TransientPoolBase>> t_transientSet;
  t_transientSet m_stTransient;

public:
  /// <summary>
  /// Convenience method allowing consumers to quickly determine whether any listeners exist
  /// </summary>
  bool HasListeners(void) const override {return !m_st.GetImage()->empty();}

  void ReleaseRefs() override {
    m_st.Clear();
    m_dispatch.clear();
  }

  EventReceiverProxyBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) override {
    auto casted = std::dynamic_pointer_cast<T, EventReceiver>(rhs);
    if(casted)
      // Proposed type is directly one of our receivers
      *this += casted;
    else {
      auto pool = std::dynamic_pointer_cast<TransientPoolBase, EventReceiver>(rhs);
      if(pool)
        *this += pool;
    }

    return *this;
  }

  EventReceiverProxyBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) override {
    auto casted = std::dynamic_pointer_cast<T, EventReceiver>(rhs);
    if(casted)
      *this -= casted;
    else {
      auto pool = std::dynamic_pointer_cast<TransientPoolBase, EventReceiver>(rhs);
      if(pool)
        *this -= pool;
    }

    return *this;
  }

  void operator+=(const std::shared_ptr<TransientPoolBase>& rhs) {
    // Obtain the witness and ascertain whether this transient pool supports our recipient type:
    if(!dynamic_cast<const T*>(&rhs->GetWitness()))
      return;

    // All transient pools are dispatchers, add it in to the dispatch pool:
    DispatchQueue* pDeferred = dynamic_cast<DispatchQueue*>(rhs.get());
    m_dispatch.insert(pDeferred);

    // Insertion:
    m_stTransient.Insert(rhs);
  }

  void operator-=(const std::shared_ptr<TransientPoolBase>& rhs) {
    m_stTransient.Erase(rhs);
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
    FilterFiringException(this, pReceiver);
  }

  /// <summary>
  /// Zero-argument deferred call relay
  /// </summary>
  /// <param name="fn">A nearly-curried routine to be invoked</param>
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

    // Transient pools next:
    {
      auto relay = [fn] (EventReceiver& er) {
        auto* casted = dynamic_cast<T*>(&er);
        if(casted)
          fn(*casted);
      };
      auto st = m_stTransient.GetImage();
      for(auto q = st->begin(); q != st->end(); q++)
        (**q).PoolInvoke(relay);
    }
  }

public:
  // Two-parenthetical invocations
  std::function<void ()> Fire(void (T::*fnPtr)()) const {
    return
      [this, fnPtr] () {
        this->FireCurried(
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
        this->FireCurried(
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
        this->FireCurried(
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
        this->FireCurried(
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
        this->FireCurried(
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
        this->FireCurried(
          [fnPtrCpy, arg1Ptr, arg2Ptr, arg3Ptr, arg4Ptr, arg5Ptr] (T& obj) {
            (obj.*fnPtrCpy)(*arg1Ptr, *arg2Ptr, *arg3Ptr, *arg4Ptr, *arg5Ptr);
          }
        );
      };
  }

  // Two-parenthetical deferred invocations:
  std::function<void ()> Defer(Deferred (T::*fnPtr)()) const {
    return
      [this, fnPtr] () {
        auto f = fnPtr;
        for(auto q = m_dispatch.begin(); q != m_dispatch.end(); q++) {
          auto* pCur = *q;
          if(!pCur->CanAccept())
            continue;

          typedef T targetType;

          // Straight dispatch queue insertion:
          pCur->AttachProxyRoutine([f] (EventReceiver& obj) {
            // Now we perform the cast:
            targetType* pObj = dynamic_cast<targetType*>(&obj);

            (pObj->*f)();
          });
        }
      };
  }

  template<class Arg1, class tArg1>
  class DeferredRelay {
  public:
    DeferredRelay(const EventReceiverProxy<T>& erp, Deferred (T::*fnPtr)(Arg1)):
      erp(erp),
      fnPtr(fnPtr)
    {
    }

  private:
    const EventReceiverProxy<T>& erp;
    Deferred (T::*fnPtr)(Arg1);

  public:
    void operator()(const tArg1& arg1) {
      auto f = fnPtr;
      for(auto q = erp.m_dispatch.begin(); q != erp.m_dispatch.end(); q++) {
        auto* pCur = *q;
        if(!pCur->CanAccept())
          continue;

        // Pass the copy into the lambda:
        pCur->AttachProxyRoutine(
          [f, arg1] (EventReceiver& obj) mutable {
            // Now we perform the cast:
            T* pObj = dynamic_cast<T*>(&obj);

            (pObj->*f)(
              std::move(arg1)
            );
          }
        );
      }
    }
  };

  template<class Arg1>
  DeferredRelay<Arg1, const typename std::decay<Arg1>::type&> Defer(Deferred (T::*fnPtr)(Arg1)) const {
    typedef typename const std::decay<Arg1>::type& tArg1;
    return DeferredRelay<Arg1, tArg1>(*this, fnPtr);
  }
};

#endif
