// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _AUTOWIRER_H
#define _AUTOWIRER_H
#include "EventManager.h"
#include "DeferredBase.h"
#include "safe_dynamic_cast.h"
#include "SharedPtrWrap.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <boost/thread/mutex.hpp>

class ContextMember;
class AutowirableSlot;

namespace AutowirerHelpers {
  template<class T, bool isPolymorphic = std::is_base_of<Object, T>::value>
  struct FindByCastInternal;

  template<class T, bool isPolymorphic = std::is_polymorphic<T>::value>
  struct AddEventReceiver;
}

template<class T>
class Autowired;

template<class T, bool isPolymorphic, bool isAbstract>
class AutowiredNoForwardDeclaration;

/// <summary>
/// This is a service class that performs the acutal autowiring operations
/// </summary>
/// <remarks>
/// End-users should generally not use this class
/// </remarks>
class Autowirer:
  public Object
{
public:
  Autowirer(const std::shared_ptr<Autowirer>& pParent);
  ~Autowirer(void);

protected:
  // General purpose lock for this class
  mutable boost::mutex m_lock;

  // A pointer to the parent context
  std::shared_ptr<Autowirer> m_pParent;

  // A back-reference to ourselves, weak in order to prevent a degenerate cyclic reference
  std::weak_ptr<Autowirer> m_self;
  
  // This is a map of the context members by type and, where appropriate, by name
  // This map keeps all of its objects resident at least until the context goes
  // away.
  // TODO:  Use type_index instead.
  // It's more efficient to use type_index, here, instead of trying to index directly by
  // the class name.  A lower bound operation is never required, making this implementation
  // inefficient.
  typedef std::multimap<std::string, SharedPtrWrapBase*> t_mpType;
  t_mpType m_byType;
  
  // All ContextMember objects known in this autowirer:
  std::unordered_set<ContextMember*> m_contextMembers;

  // Collection of objects waiting to be autowired, and a specific lock exclusively for this collection
  boost::mutex m_deferredLock;
  typedef std::map<const AutowirableSlot*, DeferredBase*> t_deferred;
  t_deferred m_deferred;

  // All known event senders and receivers
  typedef std::unordered_set<std::shared_ptr<EventReceiver>, SharedPtrHash<EventReceiver>> t_rcvrSet;
  t_rcvrSet m_eventReceivers;
  std::set<EventManagerBase*> m_eventSenders;

  /// <summary>
  /// Invokes all deferred autowiring fields, generally called after a new member has been added
  /// </summary>
  void UpdateDeferredElements(void);
  
  /// Adds an object of any kind to the IOC container
  /// </summary>
  /// <param name="value">The member to be added</param>
  /// <remarks>
  /// It's safe to allow the returned shared_ptr to go out of scope; the core context
  /// will continue to hold a reference to it until it is destroyed
  /// </remarks>
  template<class T>
  void AddInternal(std::shared_ptr<T> value) {
    // Add to the map:
    {
      SharedPtrWrap<T>* pWrap = new SharedPtrWrap<T>(m_self, value);
      boost::lock_guard<boost::mutex> lk(m_lock);
      m_byType.insert(
        t_mpType::value_type(
          std::string(typeid(*value.get()).name()),
          pWrap
        )
      );

      // If the value is an event type, we can add it to the collection of event
      // manager things:
      AddToEventSenders(value.get());
      ((AutowirerHelpers::AddEventReceiver<T>&)*this)(value);
    }

    UpdateDeferredElements();
  }

  void AddToEventSenders(EventManagerBase* pSender);
  inline void AddToEventSenders(void*) {}

  /// <summary>
  /// Removes all recognized event receivers in the indicated range
  /// </summary>
  void RemoveEventSenders(t_rcvrSet::iterator first, t_rcvrSet::iterator last);

  template<class W>
  bool DoAutowire(W& slot) {
    typename W::t_ptrType retVal;
    for(Autowirer* pCur = this; pCur; pCur = pCur->m_pParent.get()) {
      retVal = pCur->FindByType(slot);
      if(retVal) {
        slot.swap(retVal);
        return true;
      }
    }
    return false;
  }

  template<class Mp>
  friend void AutowirerErase(Autowirer* pAutowirer, typename Mp::iterator q);
  friend class SharedPtrWrapBase;

public:
  // Accessor methods:
  std::shared_ptr<Autowirer>& GetParentContext(void) {return m_pParent;}

  /// <summary>
  /// Adds an object of any kind to the IOC container
  /// </summary>
  /// <param name="pContextMember">The member which was added</param>
  /// <remarks>
  /// It's safe to allow the returned shared_ptr to go out of scope; the core context
  /// will continue to hold a reference to it until Remove is invoked.
  /// </remarks>
  template<class T>
  void Add(const std::shared_ptr<T>& value) {
    AddInternal(value);
    ContextMember* pContextMember = safe_dynamic_cast<ContextMember, T>::Cast(value.get());
    if(pContextMember)
      AddContextMember(pContextMember);
  }

  /// <summary>
  /// Overload of Add based on ContextMember
  /// </summary>
  void AddContextMember(ContextMember* pPtr);
  
  /// <summary>
  /// Attempts to find a member in the container that can be passed to the specified type
  /// </summary>
  /// <remarks>
  /// This method will throw an exception if there is more than one object castable to
  /// type T.  This method cannot be invoked with any type that does not inherit from Object
  /// due to limitations on the way that dynamic_cast works internally.
  /// </remarks>
  template<class T>
  std::shared_ptr<T> FindByCast(void) {
    return ((AutowirerHelpers::FindByCastInternal<T>&)*this)();
  }

  /// <summary>
  /// Locates an available context member by its exact type, if known
  /// </summary>
  template<class T>
  std::shared_ptr<T> FindByType(const Autowired<T>&) {
    return FindByType<T>();
  }
  
  template<class T>
  std::shared_ptr<T> FindByType(void) {
    // Attempt a resolution by type first:
    std::string typeName = typeid(T).name();
    t_mpType::iterator q;
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      q = m_byType.find(typeName);
    }

    if(q == m_byType.end())
      // We couldn't find by a perfect name match, now we'll try to find something in the
      // table which can be safely casted to the type we're looking for.  A common case
      // where this is needed is where the autowiring calls for an interface, and we have
      // a concrete implementation of this interface.
      // Note that this only works for polymorphic types!
      // POCO types do not have a VFT and cannot be used in a dynamic_cast.
      return FindByCast<T>();

    // If find has been requested by type, there should be only one match.
    std::shared_ptr<T>& retVal = *(SharedPtrWrap<T>*)q->second;
    q++;
    if(q != m_byType.end() && q->first == typeName)
      // Ambiguous match, exception:
      throw std::runtime_error("An autowiring operation resulted in an ambiguous match");
    return retVal;
  }

  /// <summary>
  /// Specialization for autowired members which have no forward declaration
  /// </summary>
  template<class T, bool isPolymorphic, bool isAbstract>
  std::shared_ptr<T> FindByType(const AutowiredNoForwardDeclaration<T, isPolymorphic, isAbstract>&) {
    // It's an object, the only thing we're going to attempt is a cast resolution
    return ((AutowirerHelpers::FindByCastInternal<T, isPolymorphic>&)*this)();
  }

  /// <summary>
  /// Registers a pointer to be autowired
  /// </summary>
  /// <remarks>
  /// The autowired member must generally inherit from Autowired, or there
  /// may be issues.
  /// </remarks>
  template<class W>
  bool Autowire(W& slot) {
    if(DoAutowire(slot))
      return true;

    // Failed, defer
    Defer(slot);
    return false;
  }
  
  template<class S>
  void Defer(S& slot) {
    class Deferred:
      public DeferredBase {
    public:
      Deferred(Autowirer* pThis, S& slot):
        DeferredBase(pThis, slot.m_tracker),
        slot(slot)
      {}
      
      S& slot;

      bool operator()() override {
        return
          this->tracker.expired() ||
          this->slot ||
          this->pThis->DoAutowire(this->slot);
      }
    };

    // Resolution failed, add this autowired value for a delayed attempt
    boost::lock_guard<boost::mutex> lk(m_deferredLock);
    if(slot)
      // Someone autowired this before we did, short-circuit
      return;

    DeferredBase*& pDeferred = m_deferred[&slot];
    if(pDeferred)
      throw std::runtime_error("A slot is being autowired, but a deferred instance already exists at this location");
    pDeferred = new Deferred(this, slot);
  }

  /// <summary>
  /// Adds a post-attachment listener in this context for a particular autowired member
  /// </summary>
  /// <remarks>
  /// This method will succeed if slot was constructed in this context or any parent context.  If this
  /// assumption is false, an exception will be thrown.
  ///
  /// It's possible that the passed slot will never be filled, and instead the corresponding instance
  /// destroyed without ever having been initialized.
  ///
  /// If the passed slot is already autowired, then the listener will be invoked immediately from the
  /// body of this method.  Care should be taken to avoid deadlocks in this case--either the caller must
  /// not be holding any locks when this method is invoked, or the caller should design the listener
  /// method such that it may be substitutde in place for the notification routine.
  /// </remarks>
  void NotifyWhenAutowired(const AutowirableSlot& slot, const std::function<void()>& listener);

  void Dump(std::ostream& os) const;
};

namespace AutowirerHelpers {

template<class T, bool isPolymorphic>
struct AddEventReceiver:
  public Autowirer
{
public:
  void operator()(std::shared_ptr<T>& value) {
    EventReceiver* pRecvr = dynamic_cast<EventReceiver*>(value.get());
    if(!pRecvr)
      return;

    // The cast is a loop invariant; store it here for convenience
    std::shared_ptr<EventReceiver> casted = std::dynamic_pointer_cast<EventReceiver, T>(value);
    m_eventReceivers.insert(casted);

    // Scan the list of compatible senders:
    for(auto q = m_eventSenders.begin(); q != m_eventSenders.end(); q++)
      **q += casted;
    
    // Delegate ascending resolution, where possible.  This ensures that the parent context links
    // this event receiver to compatible senders in the parent context itself.
    if(m_pParent)
      ((AddEventReceiver<T, true>&)*m_pParent)(value);
  }
};

template<class T>
struct AddEventReceiver<T, false>:
  public Autowirer
{
public:
  inline void operator()(const std::shared_ptr<T>&) {}
};

template<class T, bool isPolymorphic>
struct FindByCastInternal:
  public Autowirer
{
  std::shared_ptr<T> operator()(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    std::shared_ptr<Object> matchedObject;

    static_assert((!std::is_same<Object, T>::value), "FindByCastInternal on type Object is an overly broad search criteria");

    std::shared_ptr<Object> obj;
    for(t_mpType::iterator q = m_byType.begin(); q != m_byType.end(); ++q) {
      SharedPtrWrapBase* pBase = q->second;

      // See if this wrap contains an object, which we could use to access other types
      obj = pBase->AsObject();
      if(!obj)
        continue;

      // Okay, we can work with this, it's an Object
      try {
        // Try the cast
        if(!dynamic_cast<T*>(obj.get()))
          // Sometimes the above will throw an exception, sometimes it will return null.
          // We handle both cases by wrapping this in a try-except block.
          continue;

        // Verify that we didn't try storing something already
        if(matchedObject)
          throw std::runtime_error("An autowiring operation resulted in an ambiguous match");

        // Record this value to be returned
        matchedObject.swap(obj);
      } catch(std::bad_cast&) {
      } catch(...) {
        // Very bad exception happened
      };
    }

    return
      matchedObject ?
      // Generally, we don't care whether the originally constructed value is
      std::static_pointer_cast<T, Object>(matchedObject) :
      std::shared_ptr<T>();
  }
};

template<class T>
struct FindByCastInternal<T, false>:
  public Autowirer
{
  std::shared_ptr<T> operator()(void) {
    return std::shared_ptr<T>();
  }
};

}

std::ostream& operator<<(std::ostream& os, const Autowirer& rhs);

#endif
