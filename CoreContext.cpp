// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CoreContext.h"
#include "CoreThread.h"
#include "AutoPacketFactory.h"
#include "AutoPacketListener.h"
#include "Autowired.h"
#include "BoltBase.h"
#include "GlobalCoreContext.h"
#include "MicroBolt.h"
#include <algorithm>
#include <memory>
#include <boost/thread/reverse_lock.hpp>
#include <boost/thread/tss.hpp>

using namespace std;

/// <summary>
/// A pointer to the current context, specific to the current thread.
/// </summary>
/// <remarks>
/// All threads have a current context, and this pointer refers to that current context.  If this value is null,
/// then the current context is the global context.  It's very important that threads not attempt to hold a reference
/// to the global context directly because it could change teardown order if the main thread sets the global context
/// as current.
/// </remarks>
boost::thread_specific_ptr<std::shared_ptr<CoreContext>> s_curContext;

CoreContext::CoreContext(std::shared_ptr<CoreContext> pParent, const std::type_info& sigil) :
  m_pParent(pParent),
  m_sigil(sigil),
  m_useOwnershipValidator(false),
  m_initiated(false),
  m_isShutdown(false),
  m_junctionBoxManager(std::make_shared<JunctionBoxManager>()),
  m_packetFactory(std::make_shared<AutoPacketFactory>(GetJunctionBox<AutoPacketListener>()))
{
  assert(pParent.get() != this);
}

// Peer Context Constructor. Called interally by CreatePeer
CoreContext::CoreContext(std::shared_ptr<CoreContext> pParent, const std::type_info& sigil, std::shared_ptr<CoreContext> pPeer) :
  m_pParent(pParent),
  m_sigil(sigil),
  m_useOwnershipValidator(false),
  m_junctionBoxManager(pPeer->m_junctionBoxManager),
  m_packetFactory(pPeer->m_packetFactory)
{}

CoreContext::~CoreContext(void) {
  // The s_curContext pointer holds a shared_ptr to this--if we're in a dtor, and our caller
  // still holds a reference to us, then we have a serious problem.
  assert(
    !s_curContext.get() ||
    !s_curContext.get()->use_count() ||
    s_curContext.get()->get() != this
  );

  // Notify all ContextMember instances that their parent is going away
  NotifyTeardownListeners();

  // Make sure events aren't happening anymore:
  UnregisterEventReceivers();

  // Tell all context members that we're tearing down:
  for(auto q = m_contextMembers.begin(); q != m_contextMembers.end(); q++)
    (**q).NotifyContextTeardown();
}

std::shared_ptr<Object> CoreContext::IncrementOutstandingThreadCount(void) {
  std::shared_ptr<Object> retVal = m_outstanding.lock();
  if(retVal)
    return retVal;

  auto self = shared_from_this();
  retVal.reset(
    (Object*)1,
    [this, self](Object*) {
      // Object being destroyed, notify all recipients
      boost::lock_guard<boost::mutex> lk(m_lock);

      // Unfortunately, this destructor callback is made before weak pointers are
      // invalidated, which requires that we manually reset the outstanding count
      m_outstanding.reset();

      // Wake everyone up
      m_stateChanged.notify_all();
    }
  );
  m_outstanding = retVal;
  return retVal;
}

void CoreContext::AddInternal(const AddInternalTraits& traits) {
  {
    boost::lock_guard<boost::mutex> lk(m_lock);

    // Validate that this addition does not generate an ambiguity:
    auto& v = m_concreteTypes[typeid(*traits.pObject)];
    if(*v == traits.pObject)
      throw std::runtime_error("An attempt was made to add the same value to the same context more than once");
    if(*v)
      throw std::runtime_error("An attempt was made to add the same type to the same context more than once");

    // Perform the insertion at the canonical type identity:
    v = traits.value;

    // Double-check that the type we just inserted passes sanity checks:
    assert(*v == traits.pObject);

    // Insert each context element:
    if(traits.pContextMember) {
      AddContextMember(traits.pContextMember);
    }

    if(traits.pCoreRunnable) {
      AddCoreRunnable(traits.pCoreRunnable);
    }

    if(traits.pFilter) {
      m_filters.insert(traits.pFilter.get());
    }

    if(traits.pBoltBase)
      AddBolt(traits.pBoltBase);
  }

  // Event receivers:
  if(traits.pRecvr) {
    AddEventReceiver(traits.pRecvr);
  }

  // Subscribers, if applicable:
  if(traits.subscriber)
    AddPacketSubscriber(traits.subscriber);

  // Notify any autowiring field that is currently waiting that we have a new member
  // to be considered.
  UpdateDeferredElements(traits.pObject);

  // Ownership validation, as appropriate
  // We do not attempt to pend validation for CoreRunnable instances, because a
  // CoreRunnable could potentially hold the final outstanding reference to this
  // context, and therefore may be responsible for this context's (and, transitively,
  // its own) destruction.
  if(m_useOwnershipValidator && !traits.pCoreRunnable)
    SimpleOwnershipValidator::PendValidation(std::weak_ptr<Object>(traits.pObject));
  
  // Signal listeners that a new object has been created
  GetGlobal()->Invoke(&AutowiringEvents::NewObject)(*this, *traits.pObject.get());
}

std::shared_ptr<CoreContext> CoreContext::GetGlobal(void) {
  return std::static_pointer_cast<CoreContext, GlobalCoreContext>(GlobalCoreContext::Get());
}

std::vector<std::shared_ptr<BasicThread>> CoreContext::CopyBasicThreadList(void) const {
  std::vector<std::shared_ptr<BasicThread>> retVal;

  // It's safe to enumerate this list from outside of a protective lock because a linked list
  // has stable iterators, we do not delete entries from the interior of this list, and we only
  // add entries to the end of the list.
  for(auto q = m_threads.begin(); q != m_threads.end(); q++){
    BasicThread* thread = dynamic_cast<BasicThread*>(*q);
    if (thread)
      retVal.push_back((*thread).GetSelf<BasicThread>());
  }
  return retVal;
}

void CoreContext::Initiate(void) {
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    if(m_initiated)
      // Already running
      return;

    // Short-return if our stop flag has already been set:
    if(m_isShutdown)
      return;
    
    // Update flag value
    m_initiated = true;
  }

  if(m_pParent)
    // Start parent threads first
    m_pParent->Initiate();
  
  AddDelayedEventReceivers(m_delayedEventReceivers.begin(), m_delayedEventReceivers.end());
  m_delayedEventReceivers.clear();
  m_junctionBoxManager->Initiate();

  // Reacquire the lock to prevent m_threads from being modified while we sit on it
  auto outstanding = IncrementOutstandingThreadCount();
  boost::lock_guard<boost::mutex> lk(m_lock);
  
  // Signal our condition variable
  m_stateChanged.notify_all();
  
  for(t_threadList::iterator q = m_threads.begin(); q != m_threads.end(); ++q)
    (*q)->Start(outstanding);
}

void CoreContext::InitiateCoreThreads(void) {
  Initiate();
}

void CoreContext::SignalShutdown(bool wait, ShutdownMode shutdownMode) {
  // Transition as soon as possible:
  m_isShutdown = true;

  // Wipe out the junction box manager:
  (boost::unique_lock<boost::mutex>)m_lock,
  UnregisterEventReceivers();

  {
    // Teardown interleave assurance--all of these contexts will generally be destroyed
    // at the exit of this block, due to the behavior of SignalTerminate, unless exterior
    // context references (IE, related to snooping) exist.
    //
    // This is done in order to provide a stable collection that may be traversed during
    // teardown outside of a lock.
    std::vector<std::shared_ptr<CoreContext>> childrenInterleave;

    {
      // Tear down all the children.
      boost::lock_guard<boost::mutex> lk(m_childrenLock);

      // Fill strong lock series in order to ensure proper teardown interleave:
      childrenInterleave.reserve(m_children.size());
      for(auto q = m_children.begin(); q != m_children.end(); q++) {
        auto childContext = q->lock();

        // Technically, it *is* possible for this weak pointer to be expired, even though
        // we're holding the lock.  This may happen if the context itself is exiting even
        // as we are processing SignalTerminate.  In that case, the child context in
        // question is blocking in its dtor lambda, waiting patiently until we're done,
        // at which point it will modify the m_children collection.
        if(!childContext)
          continue;

        // Add to the interleave so we can SignalTerminate in a controlled way.
        childrenInterleave.push_back(q->lock());
      }
    }

    // Now that we have a locked-down, immutable series, begin termination signalling:
    for(size_t i = childrenInterleave.size(); i--; )
      childrenInterleave[i]->SignalShutdown(wait);
  }

  // Pass notice to all child threads:
  bool graceful = shutdownMode == ShutdownMode::Graceful;
  for(t_threadList::iterator q = m_threads.begin(); q != m_threads.end(); ++q)
    (*q)->Stop(graceful);

  // Signal our condition variable
  m_stateChanged.notify_all();

  // Short-return if required:
  if(!wait)
    return;

  // Wait for the treads to finish before returning.
  for (t_threadList::iterator it = m_threads.begin(); it != m_threads.end(); ++it) {
    auto& cur = **it;

    // Wait for completion only if we're currently running.  The thread cannot be made active
    // at this point, because we're in teardown, and this makes it safe to elide the call to
    // wait if the thread has never been run yet.
    if(cur.IsRunning())
      cur.Wait();
  }
}

bool CoreContext::DelayUntilInitiated(void) {
  boost::unique_lock<boost::mutex> lk(m_lock);
  m_stateChanged.wait(lk, [this]{return m_initiated || m_isShutdown;});
  return !m_isShutdown;
}

std::shared_ptr<CoreContext> CoreContext::CurrentContext(void) {
  if(!s_curContext.get())
    return std::static_pointer_cast<CoreContext, GlobalCoreContext>(GetGlobalContext());

  std::shared_ptr<CoreContext>* retVal = s_curContext.get();
  assert(retVal);
  assert(*retVal);
  return *retVal;
}

void CoreContext::AddCoreRunnable(const std::shared_ptr<CoreRunnable>& ptr) {
  // Insert into the linked list of threads first:
  m_threads.push_front(ptr.get());

  if(m_initiated)
    // We're already running, this means we're late to the game and need to start _now_.
    ptr->Start(IncrementOutstandingThreadCount());
}

void CoreContext::AddBolt(const std::shared_ptr<BoltBase>& pBase) {
  for(auto cur = pBase->GetContextSigils(); *cur; cur++){
    m_nameListeners[**cur].push_back(pBase.get());
  }

  if(!*pBase->GetContextSigils()) {
    m_allNameListeners.push_back(pBase.get());
  }
}

void CoreContext::BuildCurrentState(void) {
  GetGlobal()->Invoke(&AutowiringEvents::NewContext)(*this);
  
  //keep track of already sent objects so there are no duplicates
  std::unordered_set<Object*> allObjects;

  //ContextMembers and CoreRunnables
  for (auto member = m_contextMembers.begin(); member != m_contextMembers.end(); ++member) {
    Object* obj = dynamic_cast<Object*>(*member);
    if (obj && allObjects.find(obj)==allObjects.end()) {
      GetGlobal()->Invoke(&AutowiringEvents::NewObject)(*this, *obj);
      allObjects.insert(obj);
    }
  }

  //Exception Filters
  for (auto filter = m_filters.begin(); filter != m_filters.end(); ++filter) {
    Object* obj = dynamic_cast<Object*>(*filter);
    if (obj && allObjects.find(obj)==allObjects.end()){
      GetGlobal()->Invoke(&AutowiringEvents::NewObject)(*this, *obj);
      allObjects.insert(obj);
    }
  }

  //Event Receivers
  for (auto receiver = m_eventReceivers.begin(); receiver != m_eventReceivers.end(); ++receiver) {
    Object* obj = dynamic_cast<Object*>(receiver->m_ptr.get());
    if (obj && allObjects.find(obj)==allObjects.end()) {
      GetGlobal()->Invoke(&AutowiringEvents::NewObject)(*this, *obj);
      allObjects.insert(obj);
    }
  }

  boost::lock_guard<boost::mutex> lk(m_childrenLock);
  for (auto c = m_children.begin(); c != m_children.end(); ++c) {
    c->lock()->BuildCurrentState();
  }
}

void CoreContext::CancelAutowiringNotification(DeferrableAutowiring* pDeferrable) {
  boost::lock_guard<boost::mutex> lk(m_lock);
  auto q = m_deferred.find(pDeferrable->GetType());
  if(q == m_deferred.end())
    return;

  // Always finalize this entry:
  pDeferrable->Finalize();

  // Stores the immediate predecessor of the node we will linearly scan for in our
  // linked list.
  DeferrableAutowiring* prior = nullptr;

  // Now remove the entry from the list:
  // NOTE:  If a performance bottleneck is tracked to here, the solution is to use
  // a doubly-linked list.
  for(auto cur = q->second; cur != pDeferrable; prior = cur, cur = cur->GetFlink())
    if(!cur)
      // Ran off the end of the list, nothing we can do here
      return;

  if(prior)
    // Erase the entry by using link elision:
    prior->SetFlink(pDeferrable->GetFlink());
  if(pDeferrable->GetFlink())
    // Just update the head at this entry
    q->second = pDeferrable->GetFlink();
  else
    // Erase the entire list, the list is now empty:
    m_deferred.erase(q);
}

void CoreContext::Dump(std::ostream& os) const {
  boost::lock_guard<boost::mutex> lk(m_lock);
  for(auto q = m_concreteTypes.begin(); q != m_concreteTypes.end(); q++) {
    os << q->first.name();
    const void* pObj = q->second;
    if(pObj)
      os << " 0x" << hex << pObj;
    os << endl;
  }

  for(auto q = m_threads.begin(); q != m_threads.end(); q++) {
    BasicThread* pThread = dynamic_cast<BasicThread*>(*q);
    if (!pThread) continue;
    
    const char* name = pThread->GetName();
    os << "Thread " << pThread << " " << (name ? name : "(no name)") << std::endl;
  }
}

void ShutdownCurrentContext(void) {
  CoreContext::CurrentContext()->SignalShutdown();
}

void CoreContext::UnregisterEventReceivers(void) {
  // Release all event receivers originating from this context:
  m_junctionBoxManager->RemoveEventReceivers(m_eventReceivers.begin(), m_eventReceivers.end());

  // Notify our parent (if we have one) that our event receivers are going away:
  if(m_pParent) {
    m_pParent->RemoveEventReceivers(m_eventReceivers.begin(), m_eventReceivers.end());
    m_pParent->RemovePacketSubscribers(m_packetFactory->GetSubscriberVector());
  }

  // Wipe out all collections so we don't try to free these multiple times:
  m_eventReceivers.clear();
}

void CoreContext::BroadcastContextCreationNotice(const std::type_info& sigil) const {
  auto q = m_nameListeners.find(sigil);
  if(q != m_nameListeners.end()) {
    // Iterate through all listeners:
    const auto& list = q->second;
    for(auto q = list.begin(); q != list.end(); q++)
      (**q).ContextCreated();
  }

  for (auto i = m_allNameListeners.begin(); i != m_allNameListeners.end(); ++i) {
    (**i).ContextCreated();
  }

  // Notify the parent next:
  if(m_pParent)
    m_pParent->BroadcastContextCreationNotice(sigil);
}

void CoreContext::UpdateDeferredElements(const std::shared_ptr<Object>& entry) {
  // Collection of satisfiable lists:
  std::vector<DeferrableAutowiring*> satisfiable;

  // Notify any autowired field whose autowiring was deferred
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    for(auto q = m_deferred.begin(); q != m_deferred.end();) {
      auto& cur = q->second;

      if((*cur).TrySatisfyAutowiring(entry)) {
        // If the first entry is satisfiable then ALL entries are satisfiable
        // Move all entries into our satisfiable collection and run them later
        satisfiable.emplace_back(cur);
        q = m_deferred.erase(q);
      }
      else
        q++;
    }
  }

  for(auto listHead : satisfiable) {
    // First entry needs custom finalization and will be used as a witness:
    listHead->Finalize();
    
    // Run through everything else and finalize it all:
    for(auto* pCur = listHead->GetFlink(); pCur; pCur = pCur->GetFlink())
      pCur->SatisfyAutowiring(*listHead);
  }

  // Give children a chance to also update their deferred elements:
  boost::unique_lock<boost::mutex> lk(m_childrenLock);
  for(auto q = m_children.begin(); q != m_children.end(); q++) {
    // Hold reference to prevent this iterator from becoming invalidated:
    auto ctxt = q->lock();
    if(!ctxt)
      continue;

    // Reverse lock before satisfying children:
    lk.unlock();
    ctxt->UpdateDeferredElements(entry);
    lk.lock();
  }
}

void CoreContext::AddEventReceiver(JunctionBoxEntry<EventReceiver> receiver) {
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    
    if (!m_initiated) { //Delay adding receiver until context in initialized;
      m_delayedEventReceivers.insert(receiver);
      return;
    }
  }

  m_junctionBoxManager->AddEventReceiver(receiver);

  // Delegate ascending resolution, where possible.  This ensures that the parent context links
  // this event receiver to compatible senders in the parent context itself.
  if(m_pParent)
    m_pParent->AddEventReceiver(receiver);
}

void CoreContext::AddDelayedEventReceivers(t_rcvrSet::const_iterator first, t_rcvrSet::const_iterator last) {
  assert(m_initiated); //Must be initiated
  
  m_junctionBoxManager->AddEventReceivers(first, last);
  
  // Delegate ascending resolution, where possible.  This ensures that the parent context links
  // this event receiver to compatible senders in the parent context itself.
  if(m_pParent)
    m_pParent->AddDelayedEventReceivers(first, last);
}


void CoreContext::RemoveEventReceiver(JunctionBoxEntry<EventReceiver> pRecvr) {
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    m_eventReceivers.erase(pRecvr);
  }

  m_junctionBoxManager->RemoveEventReceiver(pRecvr);

  // Delegate to the parent:
  if(m_pParent)
    m_pParent->RemoveEventReceiver(pRecvr);
}

void CoreContext::RemoveEventReceivers(t_rcvrSet::const_iterator first, t_rcvrSet::const_iterator last) {
  {
    boost::lock_guard<boost::mutex> lk(m_lock);
    for(auto q = first; q != last; q++)
      m_eventReceivers.erase(*q);
  }

  m_junctionBoxManager->RemoveEventReceivers(first, last);

  // Detour to the parent collection (if necessary)
  if(m_pParent)
    m_pParent->RemoveEventReceivers(first, last);
}

void CoreContext::FilterException(void) {
  bool handled = false;
  for(auto q = m_filters.begin(); q != m_filters.end(); q++)
    try {
      (*q)->Filter(
        [] {
          std::rethrow_exception(std::current_exception());
        }
      );
      handled = true;
    } catch(...) {
      // Do nothing
    }

  // Pass to parent if one exists:
  if(m_pParent) {
    try {
      // See if the parent wants to handle this exception:
      m_pParent->FilterException();

      // Parent handled it, we're good to go
      return;
    } catch(...) {
    }
  }

  // Rethrow if unhandled:
  if(!handled)
    std::rethrow_exception(std::current_exception());
}

void CoreContext::FilterFiringException(const JunctionBoxBase* pProxy, EventReceiver* pRecipient) {
  auto rethrower = [] () {
    std::rethrow_exception(std::current_exception());
  };

  // Filter in order:
  for(CoreContext* pCur = this; pCur; pCur = pCur->GetParentContext().get())
    for(auto q = pCur->m_filters.begin(); q != pCur->m_filters.end(); q++)
      try {
        (*q)->Filter(rethrower, pProxy, pRecipient);
      } catch(...) {
        // Do nothing, filter didn't want to filter this exception
      }
}

void CoreContext::AddContextMember(const std::shared_ptr<ContextMember>& ptr) {
  // Reflexive assignment:
  ptr->m_self = ptr;

  // Always add to the set of context members
  m_contextMembers.insert(ptr.get());
}

void CoreContext::AddPacketSubscriber(const AutoPacketSubscriber& rhs) {
  m_packetFactory->AddSubscriber(rhs);
  if(m_pParent)
    m_pParent->AddPacketSubscriber(rhs);
}

void CoreContext::RemovePacketSubscribers(const std::vector<AutoPacketSubscriber> &subscribers) {
  // Notify the parent that it will have to remove these subscribers as well
  if(m_pParent)
    m_pParent->RemovePacketSubscribers(subscribers);

  // Remove subscribers from our factory AFTER the parent eviction has taken place
  m_packetFactory->RemoveSubscribers(subscribers.begin(), subscribers.end());
}

std::ostream& operator<<(std::ostream& os, const CoreContext& rhs) {
  rhs.Dump(os);
  return os;
}

void CoreContext::DebugPrintCurrentExceptionInformation() {
  try {
    std::rethrow_exception(std::current_exception());
  } catch(std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  } catch(...) {
    // Nothing can be done, we don't know what exception type this is.
  }
}

std::shared_ptr<CoreContext> CoreContext::SetCurrent(void) {
  std::shared_ptr<CoreContext> newCurrent = this->shared_from_this();

  if(!newCurrent)
    throw std::runtime_error("Attempted to make a CoreContext current from a CoreContext ctor");

  std::shared_ptr<CoreContext> retVal = CoreContext::CurrentContext();
  s_curContext.reset(new std::shared_ptr<CoreContext>(newCurrent));
  return retVal;
}

void CoreContext::EvictCurrent(void) {
  s_curContext.reset();
}
