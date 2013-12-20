#pragma once
#include "DispatchQueue.h"
#include "EventDispatcher.h"
#include "EventReceiver.h"
#include "LockFreeList.h"
#include "LockReducedCollection.h"
#include "SharedPtrHash.h"
#include "TransientPoolBase.h"
#include <boost/thread/mutex.hpp>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include SHARED_PTR_HEADER
#include STL_UNORDERED_SET
#include TYPE_TRAITS_HEADER
#include <set>

class JunctionBoxBase;
class EventReceiver;

/// <summary>
/// Service routine called inside Fire calls in order to decide how to handle an exception
/// </summary>
/// <remarks>
/// This routine MUST NOT be called outside of a "catch" handler.  This function, and the functions that
/// it call, rely on the validity of the std::current_exception return value, which will not be valid
/// outside of a call block.
/// </remarks>
void FilterFiringException(const JunctionBoxBase* pSender, EventReceiver* pRecipient);

/// <summary>
/// Function pointer relay type
/// </summary>
template<class FnPtr>
class InvokeRelay {};

template<class T>
class InvokeRelay<Deferred (T::*)()>;

template<class T, class Arg1>
class InvokeRelay<Deferred (T::*)(Arg1)>;

template<class T>
class InvokeRelay<void (T::*)()>;

template<class T, class Arg1>
class InvokeRelay<void (T::*)(Arg1)>;

template<class T, class Arg1, class Arg2>
class InvokeRelay<void (T::*)(Arg1, Arg2)>;

template<class T, class Arg1, class Arg2, class Arg3>
class InvokeRelay<void (T::*)(Arg1, Arg2, Arg3)>;

template<class T, class Arg1, class Arg2, class Arg3, class Arg4>
class InvokeRelay<void (T::*)(Arg1, Arg2, Arg3, Arg4)>;

template<class T, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
class InvokeRelay<void (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5)>;

/// <summary>
/// Used to identify event managers
/// </summary>
class JunctionBoxBase {
public:
  virtual ~JunctionBoxBase(void);

protected:
  // Dispatch queue lock:
  mutable boost::mutex m_lock;

  // Just the DispatchQueue listeners:
  typedef std::unordered_set<DispatchQueue*> t_stType;
  t_stType m_dispatch;

  // Collection of all transient pools:
  typedef LockReducedCollection<std::shared_ptr<TransientPoolBase>, SharedPtrHash<TransientPoolBase>> t_transientSet;
  t_transientSet m_stTransient;

public:
  // Accessor methods:
  const std::unordered_set<DispatchQueue*> GetDispatchQueue(void) const {return m_dispatch;}
  boost::mutex& GetDispatchQueueLock(void) const { return m_lock; }

  virtual bool HasListeners(void) const = 0;

  /// <summary>
  /// Invoked by the parent context when the context is shutting down in order to release all references
  /// </summary>
  virtual void ReleaseRefs(void) = 0;

  // Event attachment and detachment pure virtuals
  virtual JunctionBoxBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) = 0;
  virtual JunctionBoxBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) = 0;
};

struct NoType {};

template<class T>
class JunctionBox:
  public JunctionBoxBase
{
public:
  static_assert(
    std::is_base_of<EventReceiver, T>::value,
    "If you want an event interface, the interface must inherit from EventReceiver"
  );

  virtual ~JunctionBox(void) {}

protected:
  // Collection of all known listeners:
  typedef LockReducedCollection<std::shared_ptr<T>, SharedPtrHash<T>> t_listenerSet;
  t_listenerSet m_st;

public:
  /// <summary>
  /// Convenience method allowing consumers to quickly determine whether any listeners exist
  /// </summary>
  bool HasListeners(void) const override {return !m_st.GetImage()->empty();}

  void ReleaseRefs() override {
    (boost::lock_guard<boost::mutex>)m_lock,
    m_dispatch.clear();

    m_st.Clear();
    m_stTransient.Clear();
  }

  JunctionBoxBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) override {
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

  JunctionBoxBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) override {
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
    DispatchQueue* pDispatch = dynamic_cast<DispatchQueue*>(rhs.get());

    (boost::lock_guard<boost::mutex>)m_lock,
    m_dispatch.insert(pDispatch);

    // Insertion:
    m_stTransient.Insert(rhs);
  }

  void operator-=(const std::shared_ptr<TransientPoolBase>& rhs) {
    // Remove the non-reference-counted pointer first
    (boost::lock_guard<boost::mutex>)m_lock,
    m_dispatch.erase(dynamic_cast<DispatchQueue*>(rhs.get()));

    // Then remove from our reference counted collection
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
      (boost::lock_guard<boost::mutex>)m_lock,
      m_dispatch.insert(pDispatch);
  }

  /// <summary>
  /// Removes the specified observer from the set currently configured to receive events
  /// </summary>
  void operator-=(const std::shared_ptr<T>& rhs) {
    // If the RHS implements DispatchQueue, remove it from the dispatchers collection
    DispatchQueue* pDispatch = dynamic_cast<DispatchQueue*>(rhs.get());
    if(pDispatch)
      (boost::lock_guard<boost::mutex>)m_lock,
      m_dispatch.erase(pDispatch);

    // Trivial removal:
    auto nErased = m_st.Erase(rhs);
    if(!nErased)
      return;
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
  // Two-parenthetical deferred invocations:
  template<class FnPtr>
  auto Invoke(FnPtr fnPtr) -> InvokeRelay<decltype(fnPtr)> {
    return InvokeRelay<decltype(fnPtr)>(*this, fnPtr);
  }
};


template<class T>
class InvokeRelay<Deferred (T::*)()> {
public:
  InvokeRelay(const JunctionBoxBase& erp, Deferred (T::*fnPtr)()):
    erp(erp),
    fnPtr(fnPtr)
  {
  }

private:
  const JunctionBoxBase& erp;
  Deferred (T::*fnPtr)();

public:
  void operator()(void) const {
    const auto& dq = erp.GetDispatchQueue();
    boost::lock_guard<boost::mutex> lk(erp.GetDispatchQueueLock());

    for(auto q = dq.begin(); q != dq.end(); q++) {
      auto* pCur = *q;
      if(!pCur->CanAccept())
        continue;

      typedef T targetType;

      // Straight dispatch queue insertion:
      auto f = fnPtr;
      pCur->AttachProxyRoutine([f] (EventReceiver& obj) {
        // Now we perform the cast:
        targetType* pObj = dynamic_cast<targetType*>(&obj);

        (pObj->*f)();
      });
    }
  }
};

template<class T, class Arg1>
class InvokeRelay<Deferred (T::*)(Arg1)> {
public:
  typedef typename std::decay<Arg1>::type tArg1;

  InvokeRelay(const JunctionBoxBase& erp, Deferred (T::*fnPtr)(Arg1)):
    erp(erp),
    fnPtr(fnPtr)
  {
  }

private:
  const JunctionBoxBase& erp;
  Deferred (T::*fnPtr)(Arg1);

public:
  void operator()(const tArg1& arg1) const {
    const auto& dq = erp.GetDispatchQueue();
    boost::lock_guard<boost::mutex> lk(erp.GetDispatchQueueLock());

    for(auto q = dq.begin(); q != dq.end(); q++) {
      auto* pCur = *q;
      if(!pCur->CanAccept())
        continue;

      // Pass the copy into the lambda:
      auto f = fnPtr;
      pCur->AttachProxyRoutine(
        [f, arg1] (EventReceiver& obj) {
          // Now we perform the cast:
          T* pObj = dynamic_cast<T*>(&obj);

          (pObj->*f)(std::move(arg1));
        }
      );
    }
  }
};

template<class T>
class InvokeRelay<void (T::*)()> {
public:
  InvokeRelay(JunctionBox<T>& erp, void (T::*fnPtr)(void)):
    erp(erp),
    fnPtr(fnPtr)
  {
  }

private:
  JunctionBox<T>& erp;
  void (T::*fnPtr)();

public:
  void operator()() const {
    erp.FireCurried([&] (T& obj) {(obj.*fnPtr)();});
  }
};

template<class T, class Arg1>
class InvokeRelay<void (T::*)(Arg1)> {
public:
  InvokeRelay(JunctionBox<T>& erp, void (T::*fnPtr)(Arg1)):
    erp(erp),
    fnPtr(fnPtr)
  {
  }

private:
  JunctionBox<T>& erp;
  void (T::*fnPtr)(Arg1);

public:
  void operator()(Arg1 arg1) const {
    erp.FireCurried([&] (T& obj) {(obj.*fnPtr)(arg1);});
  }
};

template<class T, class Arg1, class Arg2>
class InvokeRelay<void (T::*)(Arg1, Arg2)> {
public:
  InvokeRelay(JunctionBox<T>& erp, void (T::*fnPtr)(Arg1, Arg2)):
    erp(erp),
    fnPtr(fnPtr)
  {
  }

private:
  JunctionBox<T>& erp;
  void (T::*fnPtr)(Arg1, Arg2);

public:
  void operator()(Arg1 arg1, Arg2 arg2) const {
    erp.FireCurried([&] (T& obj) {(obj.*fnPtr)(arg1, arg2);});
  }
};

template<class T, class Arg1, class Arg2, class Arg3>
class InvokeRelay<void (T::*)(Arg1, Arg2, Arg3)> {
public:
  InvokeRelay(JunctionBox<T>& erp, void (T::*fnPtr)(Arg1, Arg2, Arg3)):
    erp(erp),
    fnPtr(fnPtr)
  {
  }

private:
  JunctionBox<T>& erp;
  void (T::*fnPtr)(Arg1, Arg2, Arg3);

public:
  void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3) const {
    erp.FireCurried([&] (T& obj) {(obj.*fnPtr)(arg1, arg2, arg3);});
  }
};

template<class T, class Arg1, class Arg2, class Arg3, class Arg4>
class InvokeRelay<void (T::*)(Arg1, Arg2, Arg3, Arg4)> {
public:
  InvokeRelay(JunctionBox<T>& erp, void (T::*fnPtr)(Arg1, Arg2, Arg3, Arg4)):
    erp(erp),
    fnPtr(fnPtr)
  {
  }

private:
  JunctionBox<T>& erp;
  void (T::*fnPtr)(Arg1, Arg2, Arg3, Arg4);

public:
  void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) const {
    erp.FireCurried([&] (T& obj) {(obj.*fnPtr)(arg1, arg2, arg3, arg4);});
  }
};

template<class T, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
class InvokeRelay<void (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5)> {
public:
  InvokeRelay(JunctionBox<T>& erp, void (T::*fnPtr)(Arg1, Arg2, Arg3, Arg4, Arg5)):
    erp(erp),
    fnPtr(fnPtr)
  {
  }

private:
  JunctionBox<T>& erp;
  void (T::*fnPtr)(Arg1, Arg2, Arg3, Arg4, Arg5);

public:
  void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) const {
    erp.FireCurried([&] (T& obj) {(obj.*fnPtr)(arg1, arg2, arg3, arg4, arg5);});
  }
};

