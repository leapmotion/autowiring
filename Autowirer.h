#pragma once
#include "EventManager.h"
#include "SharedPtrWrap.h"
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <boost/thread/mutex.hpp>

class ContextMember;
class DestroyTracker;

namespace AutowirerHelpers {
  template<class T, bool isPolymorphic = cpp11::is_base_of<Object, T>::value>
  struct FindByCastInternal;
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
  Autowirer(const cpp11::shared_ptr<Autowirer>& pParent);
  ~Autowirer(void);

protected:
  // General purpose lock for this class
  boost::mutex m_lock;

  // A pointer to the parent context
  cpp11::shared_ptr<Autowirer> m_pParent;

  // A back-reference to ourselves, weak in order to prevent a degenerate cyclic reference
  cpp11::weak_ptr<Autowirer> m_self;
  
  // This is a map of the context members by type and, where appropriate, by name
  // This map keeps all of its objects resident at least until the context goes
  // away.
  // TODO:  Use type_index instead.
  // It's more efficient to use type_index, here, instead of trying to index directly by
  // the class name.  A lower bound operation is never required, making this implementation
  // inefficient.
  typedef std::multimap<std::string, SharedPtrWrapBase*> t_mpType;
  t_mpType m_byType;
  
  // Only one object in a context can bear a particular name
  typedef std::map<std::string, ContextMember*> t_mpName;
  t_mpName m_byName;

  // Set of objects waiting to be autowired
  typedef std::list<cpp11::function<bool ()> > t_deferredList;
  t_deferredList m_deferred;

  // All known event receivers
  std::vector<cpp11::shared_ptr<EventReceiver> > m_eventReceivers;
  std::vector<EventManagerBase*> m_eventSenders;

  /// <summary>
  /// Erasure routine, designed to be invoked from inside SharedPtrWrap
  /// </summary>
  void erase(t_mpType::iterator q) {
    m_byType.erase(q);
  }
  
  void erase(t_mpName::iterator q) {
    m_byName.erase(q);
  }

  /// <summary>
  /// Final addition method that inserts a generic object to the m_byType map
  /// </summary>
  template<class T>
  cpp11::shared_ptr<T> AddInternal(T* pValue) {
    // Create the wrap:
    SharedPtrWrap<T>* pWrap = new SharedPtrWrap<T>(m_self, pValue);

    // Add to the map:
    {
      boost::lock_guard<boost::mutex> lk(m_lock);
      m_byType.insert(
        t_mpType::value_type(
          std::string(typeid(*pValue).name()),
          pWrap
        )
      );

      // If the value is an event type, we can add it to the collection of event
      // manager things:
      AddToEventSenders(pValue);
      AddToEventReceivers(pValue, *pWrap);
    }

    // Notify any autowired field whose autowiring was deferred
    // TODO:  We should also notify any descendant autowiring
    // contexts that a new member is now available.
    for(t_deferredList::iterator r = m_deferred.begin(); r != m_deferred.end(); ) {
      bool rs = (*r)();
      
      boost::lock_guard<boost::mutex> lk(m_lock);
      if(rs)
        r = m_deferred.erase(r);
      else
        r++;
    }

    // Done, return
    return *pWrap;
  }

  void AddToEventSenders(EventManagerBase* pSender) {
    m_eventSenders.push_back(pSender);

    // Scan the list for compatible receivers:
    for(size_t i = 0; i < m_eventReceivers.size(); i++)
      *pSender += m_eventReceivers[i];
  }
  inline void AddToEventSenders(void*) {}

  template<class T>
  void AddToEventReceivers(EventReceiver* pEventReceiver, cpp11::shared_ptr<T>& sharedPtr) {
    m_eventReceivers.push_back(
      cpp11::static_pointer_cast<EventReceiver, T>(sharedPtr)
    );

    // The cast is a loop invariant; store it here for convenience
    cpp11::shared_ptr<EventReceiver> casted = cpp11::static_pointer_cast<EventReceiver, T>(sharedPtr);

    // Scan the list of compatible senders:
    for(size_t i = 0; i < m_eventSenders.size(); i++)
      *m_eventSenders[i] += casted;
  }

  template<class T>
  inline void AddToEventReceivers(void*, const cpp11::shared_ptr<T>&) {}

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
  cpp11::shared_ptr<Autowirer>& GetParentContext(void) {return m_pParent;}

  /// <summary>
  /// Adds an object of any kind to the IOC container
  /// </summary>
  /// <param name="pContextMember">The member which was added</param>
  /// <return>The shared pointer which contains the context member.</return>
  /// <remarks>
  /// It's safe to allow the returned shared_ptr to go out of scope; the core context
  /// will continue to hold a reference to it until Remove is invoked.
  /// </remarks>
  template<class T>
  cpp11::shared_ptr<T> Add(T* pValue) {
    cpp11::shared_ptr<T> retVal = Autowirer::Add(pValue);
    AddContextMember(&retVal);
    return retVal;
  }

  /// <summary>
  /// Overload of Add based on ContextMember
  /// </summary>
  void AddContextMember(ContextMember* pPtr);
  void AddContextMember(void* ptr) {}
  
  /// <summary>
  /// Attempts to find a member in the container that can be passed to the specified type
  /// </summary>
  /// <remarks>
  /// This method will throw an exception if there is more than one object castable to
  /// type T.  This method cannot be invoked with any type that does not inherit from Object
  /// due to limitations on the way that dynamic_cast works internally.
  /// </remarks>
  template<class T>
  cpp11::shared_ptr<T> FindByCast(void) {
    return ((AutowirerHelpers::FindByCastInternal<T>&)*this)();
  }

  /// <summary>
  /// Locates an available context member by its exact type, if known
  /// </summary>
  template<class T>
  cpp11::shared_ptr<T> FindByType(const Autowired<T>&) {
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
    cpp11::shared_ptr<T>& retVal = *(SharedPtrWrap<T>*)q->second;
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
  cpp11::shared_ptr<T> FindByType(const AutowiredNoForwardDeclaration<T, isPolymorphic, isAbstract>&) {
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
    class Lambda {
    public:
      Lambda(Autowirer* pThis, S& slot):
        pThis(pThis),
        slot(slot),
        tracker(slot.m_tracker) {
      }
      
      Autowirer* pThis;
      S& slot;

      // Store a weak reference to the slot's tracker so we can be informed
      // if it goes away before we have a chance to autowire it
      cpp11::weak_ptr<DestroyTracker> tracker;

      bool operator()() {
        return
          this->tracker.expired() ||
          this->slot ||
          this->pThis->DoAutowire(this->slot);
      }
    };

    // Resolution failed, add this autowired value for a delayed attempt
    m_deferred.push_back(Lambda(this, slot));
  }
};

namespace AutowirerHelpers {

template<class T, bool isPolymorphic>
struct FindByCastInternal;

template<class T>
struct FindByCastInternal<T, true>:
  public Autowirer
{
  cpp11::shared_ptr<T> operator()(void) {
    boost::lock_guard<boost::mutex> lk(m_lock);
    cpp11::shared_ptr<Object> matchedObject;

    static_assert(!cpp11::is_same<Object, T>::value, "FindByCastInternal on type Object is an overly broad search criteria");

    cpp11::shared_ptr<Object> obj;
    for(t_mpType::iterator q = m_byType.begin(); q != m_byType.end(); q++) {
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
      cpp11::static_pointer_cast<T, Object>(matchedObject) :
      cpp11::shared_ptr<T>();
  }
};

template<class T>
struct FindByCastInternal<T, false>:
  public Autowirer
{
  cpp11::shared_ptr<T> operator()(void) {
    return cpp11::shared_ptr<T>();
  }
};

}
