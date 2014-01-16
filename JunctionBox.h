#pragma once
#include "DispatchQueue.h"
#include "EventDispatcher.h"
#include "EventReceiver.h"
#include "EventOutputStream.h"
#include "EventInputStream.h"
#include "ObjectPool.h"
#include "PolymorphicTypeForest.h"
#include "SharedPtrHash.h"
#include <boost/thread/mutex.hpp>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include SHARED_PTR_HEADER
#include STL_UNORDERED_SET
#include TYPE_TRAITS_HEADER

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
  
  typedef std::unordered_set<std::shared_ptr<EventReceiver>, SharedPtrHash<EventReceiver>> t_rcvrSet;

public:
  // Accessor methods:
  std::vector<std::weak_ptr<EventOutputStreamBase> > * m_PotentialMarshals;
  void SetPotentialMarshals(std::vector<std::weak_ptr<EventOutputStreamBase> > * inVec){ 
    m_PotentialMarshals = inVec; 
  }

  const std::unordered_set<DispatchQueue*> GetDispatchQueue(void) const {return m_dispatch;}
  boost::mutex& GetDispatchQueueLock(void) const { return m_lock; }

  virtual bool HasListeners(void) const = 0;

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
  typedef std::unordered_set<std::shared_ptr<T>, SharedPtrHash<T>> t_listenerSet;
  t_listenerSet m_st;

public:

  /// <summary>
  /// Recursive serialize message: Initial Processing- 0 arg case
  /// </summary> 
  template <typename Memfn>
  void SerializeInit(Memfn memfn){
    //First distribute the arguments to any listening serializers in current context
    if (m_PotentialMarshals){
      auto m_vector = *m_PotentialMarshals;
      auto it = m_vector.begin();

      while (it != m_vector.end()){
        auto testptr = (*it).lock();
        if (testptr) {
          //if given eventid is enabled for given eventoutputstream, serialize!
          if (testptr->IsEnabled(memfn)){
            testptr->EmitHeader(memfn);
            testptr->EmitFooter();
          }
          ++it;
        }
        else it = m_vector.erase(it); //opportunistically kill dead references.
      }
    }
  }

  /// <summary>
  /// Recursive serialize message: Initial Processing- n arg case
  /// </summary>
  template <typename Memfn, typename Head, typename... Targs>
  void SerializeInit(Memfn memfn, Head & val, Targs ... args){
    //First distribute the arguments to any listening serializers in current context
    if (m_PotentialMarshals){
      auto m_vector = *m_PotentialMarshals;
      auto it = m_vector.begin();

      while (it != m_vector.end()){
        auto testptr = (*it).lock();
        if (testptr) {
          //if given eventid is enabled for given eventoutputstream, serialize!
          if (testptr->IsEnabled(memfn)){
            testptr->EmitHeader(memfn);
            testptr->Serialize2( val, args ...);
            testptr->EmitFooter();            
          }
          ++it;
        }
        else it = m_vector.erase(it); //opportunistically kill dead references.
      }
    }
  }


  /// <summary>
  /// Convenience method allowing consumers to quickly determine whether any listeners exist
  /// </summary>
  bool HasListeners(void) const override {
    return (boost::lock_guard<boost::mutex>)m_lock, !m_st.empty();
  }

  JunctionBoxBase& operator+=(const std::shared_ptr<EventReceiver>& rhs) override {
    auto casted = std::dynamic_pointer_cast<T, EventReceiver>(rhs);
    if(casted){
      // Proposed type is directly one of our receivers
      *this += casted;
    }
    return *this;
  }

  JunctionBoxBase& operator-=(const std::shared_ptr<EventReceiver>& rhs) override {
    auto casted = std::dynamic_pointer_cast<T, EventReceiver>(rhs);
    if(casted){
      *this -= casted;
    }
    return *this;
  }

  /// <summary>
  /// Adds the specified observer to receive events dispatched from this instace
  /// </summary>
  void operator+=(const std::shared_ptr<T>& rhs) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    
    // Trivial insert
    m_st.insert(rhs);

    // If the RHS implements DispatchQueue, add it to that collection as well:
    DispatchQueue* pDispatch = std::fast_pointer_cast<DispatchQueue, T>(rhs).get();
    if(pDispatch)
      m_dispatch.insert(pDispatch);
  }

  /// <summary>
  /// Removes the specified observer from the set currently configured to receive events
  /// </summary>
  void operator-=(const std::shared_ptr<T>& rhs) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    
    // If the RHS implements DispatchQueue, remove it from the dispatchers collection
    DispatchQueue* pDispatch = std::fast_pointer_cast<DispatchQueue, T>(rhs).get();
    if(pDispatch)
      m_dispatch.erase(pDispatch);

    // Trivial removal:
    m_st.erase(rhs);
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
    
    // Copy listeners in "m_st" so we can iterate through them without thread problems
    m_lock.lock();
    t_listenerSet listeners(m_st.begin(), m_st.end());
    m_lock.unlock();
    
    // Held names first:
    for(auto q = listeners.begin(); q != listeners.end(); ++q){
      try {
        fn(**q);
      } catch(...) {
        this->PassFilterFiringException((*q).get());
      }
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

          auto called = std::move(arg1);
          (pObj->*f)(called);
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
    //First distribute the arguments to any listening serializers in current context
    erp.SerializeInit(fnPtr);
    //Then wrap up stuff in a lambda and get ready to pass to eventreceivers
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
   //First distribute the arguments to any listening serializers in current context
   erp.SerializeInit(fnPtr, arg1);
	//Then wrap up stuff in a lambda and get ready to pass to eventreceivers
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
    //First distribute the arguments to any listening serializers in current context
    erp.SerializeInit(fnPtr, arg1, arg2);
    //Then wrap up stuff in a lambda and get ready to pass to eventreceivers
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
    //First distribute the arguments to any listening serializers in current context
    erp.SerializeInit(fnPtr, arg1, arg2, arg3);
    //Then wrap up stuff in a lambda and get ready to pass to eventreceivers
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
    //First distribute the arguments to any listening serializers in current context
    erp.SerializeInit(fnPtr, arg1, arg2, arg3, arg4);
    //Then wrap up stuff in a lambda and get ready to pass to eventreceivers
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
    //First distribute the arguments to any listening serializers in current context
    erp.SerializeInit(fnPtr, arg1, arg2, arg3, arg4, arg5);
    //Then wrap up stuff in a lambda and get ready to pass to eventreceivers
    erp.FireCurried([&] (T& obj) {(obj.*fnPtr)(arg1, arg2, arg3, arg4, arg5);});
  }
};

