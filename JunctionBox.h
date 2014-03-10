#pragma once
#include "DispatchQueue.h"
#include "EventDispatcher.h"
#include "EventReceiver.h"
#include "ObjectPool.h"
#include "PolymorphicTypeForest.h"
#include "fast_pointer_cast.h"
#include <boost/thread/mutex.hpp>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include SHARED_PTR_HEADER
#include STL_UNORDERED_SET
#include TYPE_TRAITS_HEADER

class CoreContext;
class EventReceiver;
class JunctionBoxBase;

/// <summary>
/// Function pointer relay type
/// </summary>
template<class FnPtr>
class InvokeRelay {};

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

  /// <summary>
  /// Invokes SignalTerminate on each context in the specified vector
  /// </summary>
  static void TerminateAll(const std::vector<std::weak_ptr<CoreContext>>& teardown);

  /// <summary>
  /// Convenience routine for Fire calls
  /// </summary>
  /// <remarks>
  /// This is a convenience routine, its only purpose is to add the "this" parameter to the
  /// call to FilterFiringException
  /// </remarks>
  void FilterFiringException(const std::shared_ptr<EventReceiver>& pReceiver) const;

  /// <summary>
  /// Converts a dumb pointer into a weak pointer
  /// </summary>
  /// <remarks>
  /// An exterior hold guarantee must be made to call this method safely
  /// </remarks>
  static std::weak_ptr<CoreContext> ContextDumbToWeak(CoreContext* pContext);

public:
  // Accessor methods:
  const std::unordered_set<DispatchQueue*> GetDispatchQueue(void) const { return m_dispatch; }
  boost::mutex& GetDispatchQueueLock(void) const { return m_lock; }

  virtual bool HasListeners(void) const = 0;

  /// <summary>
  /// Invoked by the parent context when the context is shutting down in order to release all references
  /// </summary>
  virtual void RemoveAll(void) = 0;

  // Event attachment and detachment pure virtuals
  virtual void Add(CoreContext* owner, const std::shared_ptr<EventReceiver>& rhs) = 0;
  virtual void Remove(const std::shared_ptr<EventReceiver>& rhs) = 0;
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
  
  JunctionBox(void):
    m_numberOfDeletions(0)
  {}

  virtual ~JunctionBox(void) {}

protected:
  // Internal structure representing a junction box entry
  struct JunctionBoxEntry
  {
    JunctionBoxEntry(CoreContext* owner, std::shared_ptr<T> ptr) :
      m_owner(owner),
      m_ptr(ptr)
    {}

    std::shared_ptr<T> m_ptr;

    bool operator==(const JunctionBoxEntry& rhs) const {
      return get() == rhs.get();
    }

    bool operator<(const JunctionBoxEntry& rhs) const {
      return m_ptr < rhs.m_ptr;
    }
  };

  // Collection of all known listeners:
  typedef std::set<JunctionBoxEntry> t_listenerSet;
  t_listenerSet m_st;
  
  // Incremented every time an event is deleted to notify potentially invalidated iterators
  volatile int m_numberOfDeletions;

public:
  void RemoveAll() override {
    boost::lock_guard<boost::mutex> lk(m_lock);

    // Deleted entry count must be incremented unconditionally:
    m_numberOfDeletions++;

    // Clear the dispatch queue and membership set:
    m_dispatch.clear();
    m_st.clear();
  }

  /// <summary>
  /// Convenience method allowing consumers to quickly determine whether any listeners exist
  /// </summary>
  bool HasListeners(void) const override {
    return (boost::lock_guard<boost::mutex>)m_lock, !m_st.empty();
  }

  void Add(CoreContext* owner, const std::shared_ptr<EventReceiver>& rhs) override {
    auto casted = std::dynamic_pointer_cast<T, EventReceiver>(rhs);
    if(casted)
      // Proposed type is directly one of our receivers
      Add(owner, casted);
  }

  void Remove(const std::shared_ptr<EventReceiver>& rhs) override {
    auto casted = std::dynamic_pointer_cast<T, EventReceiver>(rhs);
    if(casted)
      Remove(casted);
  }

  /// <summary>
  /// Adds the specified observer to receive events dispatched from this instace
  /// </summary>
  void Add(CoreContext* owner, const std::shared_ptr<T>& rhs) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    
    // Trivial insert
    m_st.insert(rhs);

    // If the RHS implements DispatchQueue, add it to that collection as well:
    DispatchQueue* pDispatch = leap::fast_pointer_cast<DispatchQueue, T>(rhs).get();
    if(pDispatch)
      m_dispatch.insert(pDispatch);
  }

  /// <summary>
  /// Removes the specified observer from the set currently configured to receive events
  /// </summary>
  void Remove(const std::shared_ptr<T>& rhs) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    
    // Update the deletion count
    m_numberOfDeletions++;
    
    // If the RHS implements DispatchQueue, remove it from the dispatchers collection
    DispatchQueue* pDispatch = leap::fast_pointer_cast<DispatchQueue, T>(rhs).get();
    if(pDispatch)
      m_dispatch.erase(pDispatch);

    m_st.erase(rhs);
  }

  /// <summary>
  /// Zero-argument deferred call relay
  /// </summary>
  /// <param name="fn">A nearly-curried routine to be invoked</param>
  /// <return>False if an exception was thrown by a recipient, true otherwise</return>
  template<class Fn>
  bool FireCurried(Fn&& fn) const {
    boost::unique_lock<boost::mutex> lk(m_lock);
    int deleteCount = m_numberOfDeletions;

    // Set of contexts that need to be torn down in the event of an exception:
    std::vector<std::weak_ptr<CoreContext>> teardown;
    
    for(auto it = m_st.begin(); it != m_st.end(); ){
      JunctionBoxEntry currentEvent(*it);
      
      lk.unlock();
      try {
        fn(*currentEvent.m_ptr);
      } catch(...) {
        teardown.push_back(ContextDumbToWeak(it->m_owner));
        this->FilterFiringException(currentEvent.m_ptr);
      }
      lk.lock();
      
      // Increment iterator correctly even if it's been invalidated
      if (deleteCount == m_numberOfDeletions){
        ++it;
      } else {
        it = m_st.upper_bound(currentEvent);
        deleteCount = m_numberOfDeletions;
      }
    }
    if(teardown.empty())
      // Nobody threw any exceptions, end here
      return true;

    // Exceptions thrown, teardown and then indicate an error
    lk.unlock();
    TerminateAll(teardown);
    return false;
  }

  // Two-parenthetical deferred invocations:
  template<class FnPtr>
  auto Invoke(FnPtr fnPtr) -> InvokeRelay<decltype(fnPtr)> {
    return InvokeRelay<decltype(fnPtr)>(this, fnPtr);
  }
};


template<typename T, typename ...Args>
class InvokeRelay<Deferred (T::*)(Args...)> {
public:
  InvokeRelay(const JunctionBox<T>* erp, Deferred (T::*fnPtr)(Args...)):
    erp(erp),
    fnPtr(fnPtr)
  {}
  
  // Null constructor
  InvokeRelay():
    erp(nullptr)
  {}

private:
  const JunctionBox<T>* erp;
  Deferred (T::*fnPtr)(Args...);
  
public:
  void operator()(const typename std::decay<Args>::type&... args) const {
    if (!erp) return; //Context has already been destroyed
    
    const auto& dq = erp->GetDispatchQueue();
    boost::lock_guard<boost::mutex> lk(erp->GetDispatchQueueLock());
    
    for(auto q = dq.begin(); q != dq.end(); q++) {
      auto* pCur = *q;
      if(!pCur->CanAccept())
        continue;
      
      // Pass the copy into the lambda:
      auto f = fnPtr;
      auto lambda = [=] (EventReceiver& obj, Args... args) {
          // Now we perform the cast:
          T* pObj = dynamic_cast<T*>(&obj);
          (pObj->*f)(std::move(args)...);
        };
      
      auto bound_lambda = std::bind<void>(lambda, std::placeholders::_1, args...);
      pCur->AttachProxyRoutine(bound_lambda);
    }
  }
};


template<class T, typename... Args>
class InvokeRelay<void (T::*)(Args...)> {
public:
  InvokeRelay(JunctionBox<T>* erp, void (T::*fnPtr)(Args...)) :
    erp(erp),
    fnPtr(fnPtr)
  {}
  
  // Null constructor
  InvokeRelay():
    erp(nullptr)
  {}
  
private:
  JunctionBox<T>* erp;
  void (T::*fnPtr)(Args...);

public:
  /// <summary>
  /// The function call operation itself
  /// </summary>
  /// <returns>False if an exception was thrown by a recipient, true otherwise</returns>
  bool operator()(Args... args) const {
    if(!erp)
      // Context has already been destroyed
      return true;

    return erp->FireCurried([&](T& obj) {(obj.*fnPtr)(args...); });
  }
};
