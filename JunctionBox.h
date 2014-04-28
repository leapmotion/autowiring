#pragma once
#include "DispatchQueue.h"
#include "DispatchThunk.h"
#include "EventReceiver.h"
#include "fast_pointer_cast.h"
#include "EventOutputStream.h"
#include "EventInputStream.h"
#include <boost/thread/mutex.hpp>
#include "fast_pointer_cast.h"
#include <set>
#include TUPLE_HEADER
#include RVALUE_HEADER
#include MEMORY_HEADER
#include STL_UNORDERED_SET
#include TYPE_TRAITS_HEADER

class CoreContext;
class EventReceiver;
class JunctionBoxBase;

/// <summary>
/// Utility routine which shuts down the current context
/// </summary>
void ShutdownCurrentContext(void);

/// <summary>
/// Function pointer relay type
/// </summary>
template<class FnPtr>
class InvokeRelay {};

/// <summary>
/// Utility structure representing a junction box entry together with its owner
/// </summary>
template<class T>
struct JunctionBoxEntry
{
  JunctionBoxEntry(CoreContext* owner, std::shared_ptr<T> ptr) :
    m_owner(owner),
    m_ptr(ptr)
  {}

  CoreContext* const m_owner;
  std::shared_ptr<T> m_ptr;
  
  JunctionBoxEntry& operator=(const JunctionBoxEntry& rhs) {
    // This shouldn't be used. non-c++11 containers require this...
    throw std::runtime_error("Can't copy a JunctionBoxEntry");
    return *this;
  }

  bool operator==(const JunctionBoxEntry& rhs) const {
    return m_ptr == rhs.m_ptr;
  }

  bool operator<(const JunctionBoxEntry& rhs) const {
    return m_ptr < rhs.m_ptr;
  }

  operator bool(void) const {
    return !!m_ptr.get();
  }

  template<class U>
  JunctionBoxEntry<U> Rebind(void) const {
    return JunctionBoxEntry<U>(
      m_owner,
      std::dynamic_pointer_cast<U, T>(m_ptr)
    );
  }
};

namespace std {
  /// <summary>
  /// Hash specialization for the junction box entry
  /// </summary>
  template<class T>
  struct hash<JunctionBoxEntry<T>> :
    public hash<std::shared_ptr<T>>
  {
    size_t operator()(const JunctionBoxEntry<T>& jbe) const {
      return hash<std::shared_ptr<T>>::operator()(jbe.m_ptr);
    }
  };
}
/// <summary>
/// Used to identify event managers
/// </summary>
class JunctionBoxBase {
public:
  JunctionBoxBase(void):
    m_isInitiated(false)
  {}
  
  virtual ~JunctionBoxBase(void);

protected:
  // Dispatch queue lock:
  mutable boost::mutex m_lock;

  // Just the DispatchQueue listeners:
  typedef std::unordered_set<DispatchQueue*> t_stType;
  t_stType m_dispatch;
  
  // This JunctionBox can fire and receive events
  bool m_isInitiated;

  /// <summary>
  /// Invokes SignalTerminate on each context in the specified vector.  Does not wait.
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
  bool IsInitiated(void) const {return m_isInitiated;}
  void Initiate(void) {m_isInitiated=true;}
  
  // Accessor methods:
  std::vector<std::weak_ptr<EventOutputStreamBase> > * m_PotentialMarshals;

  void SetPotentialMarshals(std::vector<std::weak_ptr<EventOutputStreamBase>> * inVec){
    m_PotentialMarshals = inVec;
  }

  const std::unordered_set<DispatchQueue*> GetDispatchQueue(void) const { return m_dispatch; }
  boost::mutex& GetDispatchQueueLock(void) const { return m_lock; }

  virtual bool HasListeners(void) const = 0;

  // Event attachment and detachment pure virtuals
  virtual void Add(const JunctionBoxEntry<EventReceiver>& rhs) = 0;
  virtual void Remove(const JunctionBoxEntry<EventReceiver>& rhs) = 0;
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
  // Collection of all known listeners:
  typedef std::set<JunctionBoxEntry<T>> t_listenerSet;
  t_listenerSet m_st;

  // Incremented every time an event is deleted to notify potentially invalidated iterators
  volatile int m_numberOfDeletions;

public:
  /// <summary>
  /// Recursive serialize message: Initial Processing- n arg case
  /// </summary>
  template <typename Memfn, typename... Targs>
  void SerializeInit(Memfn memfn, Targs&... args) {
    //First distribute the arguments to any listening serializers in current context
    if (m_PotentialMarshals){
      auto m_vector = *m_PotentialMarshals;
      auto it = m_vector.begin();

      while (it != m_vector.end()){
        auto testptr = (*it).lock();
        if (testptr) {
          //if given eventid is enabled for given eventoutputstream, serialize!
          if (testptr->IsEnabled(memfn))
            testptr->SerializeInit(memfn, args...);
          ++it;
        }
        else
          it = m_vector.erase(it); //opportunistically kill dead references.
      }
    }
  }

  /// <summary>
  /// Convenience method allowing consumers to quickly determine whether any listeners exist
  /// </summary>
  bool HasListeners(void) const override {
    return (boost::lock_guard<boost::mutex>)m_lock, !m_st.empty();
  }

  void Add(const JunctionBoxEntry<EventReceiver>& rhs) override {
    auto casted = rhs.Rebind<T>();
    if(casted)
      // Proposed type is directly one of our receivers
      Add(casted);
  }

  void Remove(const JunctionBoxEntry<EventReceiver>& rhs) override {
    auto casted = rhs.Rebind<T>();
    if(casted)
      Remove(casted);
  }

  /// <summary>
  /// Adds the specified observer to receive events dispatched from this instace
  /// </summary>
  void Add(const JunctionBoxEntry<T>& rhs) {
    boost::lock_guard<boost::mutex> lk(m_lock);

    // Trivial insert
    m_st.insert(rhs);

    // If the RHS implements DispatchQueue, add it to that collection as well:
    DispatchQueue* pDispatch = leap::fast_pointer_cast<DispatchQueue, T>(rhs.m_ptr).get();
    if(pDispatch)
      m_dispatch.insert(pDispatch);
  }

  /// <summary>
  /// Removes the specified observer from the set currently configured to receive events
  /// </summary>
  void Remove(const JunctionBoxEntry<T>& rhs) {
    boost::lock_guard<boost::mutex> lk(m_lock);

    // Update the deletion count
    m_numberOfDeletions++;

    // If the RHS implements DispatchQueue, remove it from the dispatchers collection
    DispatchQueue* pDispatch = leap::fast_pointer_cast<DispatchQueue, T>(rhs.m_ptr).get();
    if(pDispatch)
      m_dispatch.erase(pDispatch);

    m_st.erase(rhs);
  }

  /// <summary>
  /// Zero-argument deferred call relay
  /// </summary>
  /// <param name="fn">A nearly-curried routine to be invoked</param>
  /// <return>False if an exception was thrown by a recipient, true otherwise</return>
  template<class Fn, class... Args>
  bool FireCurried(const Fn& fn, Args&... args) const {
    boost::unique_lock<boost::mutex> lk(m_lock);
    int deleteCount = m_numberOfDeletions;

    // Set of contexts that need to be torn down in the event of an exception:
    std::vector<std::weak_ptr<CoreContext>> teardown;

    for(auto it = m_st.begin(); it != m_st.end(); ){
      JunctionBoxEntry<T> currentEvent(*it);

      lk.unlock();
      try {
        fn(*currentEvent.m_ptr, args...);
      } catch(...) {
        teardown.push_back(ContextDumbToWeak(currentEvent.m_owner));
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
  template<typename FnPtr>
  auto Invoke(FnPtr fnPtr) -> InvokeRelay<FnPtr> {
    return InvokeRelay<FnPtr>(this, fnPtr);
  }
};

// Generate and index tuple
template<int ...>
struct seq {};

template<int N, int... S>
struct gen_seq: gen_seq<N - 1, N - 1, S...> {};

template<int... S>
struct gen_seq<0, S...> {
  typedef seq<S...> type;
};

// Check if any T::value is true
template<typename... T>
struct is_any{
  static const bool value = false;
};

template<typename Head, typename... Tail>
struct is_any<Head, Tail...>{
  static const bool value = Head::value || is_any<Tail...>::value;
};

/// <summary>
/// A fully bound member function call
/// </summary>
/// <remarks>
/// </remarks>
template<class T, class... Args>
class CurriedInvokeRelay:
  public DispatchThunkBase
{
public:
  CurriedInvokeRelay(CurriedInvokeRelay& rhs) = delete;
  CurriedInvokeRelay(CurriedInvokeRelay&& rhs) = delete;

  CurriedInvokeRelay(T& obj, Deferred(T::*fnPtr)(Args...), Args... args) :
    m_obj(obj),
    m_fnPtr(fnPtr),
    m_args(std::forward<Args>(args)...)
  {}

private:
  // The object on which we are bound
  T& m_obj;

  // Function call to be made, and its arguments:
  Deferred(T::*m_fnPtr)(Args...);
  std::tuple<typename std::decay<Args>::type...> m_args;

  /// <summary>
  /// Places a call to the bound member function pointer by unpacking a lambda into it
  /// </summary>
  template<int... S>
  void CallByUnpackingTuple(seq<S...>) {
    (m_obj.*m_fnPtr)(std::move(std::get<S>(m_args))...);
  }

public:
  void operator()(void) override {
    CallByUnpackingTuple(typename gen_seq<sizeof...(Args)>::type());
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

  static_assert(!is_any<std::is_rvalue_reference<Args>...>::value, "Can't use rvalue references as event argument type");

private:
  const JunctionBox<T>* erp;
  Deferred (T::*fnPtr)(Args...);

public:
  void operator()(const typename std::decay<Args>::type&... args) const {
    if(!erp)
      // Context has already been destroyed
      return;
    
    if(!erp->IsInitiated())
      // Context not yet started
      return;

    const auto& dq = erp->GetDispatchQueue();
    boost::lock_guard<boost::mutex> lk(erp->GetDispatchQueueLock());

    for(auto q = dq.begin(); q != dq.end(); q++)
        (**q).AddExisting(new CurriedInvokeRelay<T, Args...>(dynamic_cast<T&>(**q), fnPtr, args...));
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

  static_assert(!is_any<std::is_rvalue_reference<Args>...>::value, "Can't use rvalue references as event argument type");

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
    
    if(!erp->IsInitiated())
      // Context not yet started
      return true;

    // Give the serializer a chance to handle these arguments:
    erp->SerializeInit(fnPtr, args...);

    auto fw = [this](T& obj, Args... args) {
      (obj.*fnPtr)(args...);
    };

    return erp->FireCurried(
      std::move(fw),
      args...
    );
  }
};
