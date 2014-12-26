// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CoreContext.h"
#include "AutoInjectable.h"
#include "AutoPacketFactory.h"
#include "BoltBase.h"
#include "CoreThread.h"
#include "demangle.h"
#include "GlobalCoreContext.h"
#include "JunctionBox.h"
#include "MicroBolt.h"
#include "NewAutoFilter.h"
#include <algorithm>
#include <stack>
#include "thread_specific_ptr.h"

/// <summary>
/// A pointer to the current context, specific to the current thread.
/// </summary>
/// <remarks>
/// All threads have a current context, and this pointer refers to that current context.  If this value is null,
/// then the current context is the global context.  It's very important that threads not attempt to hold a reference
/// to the global context directly because it could change teardown order if the main thread sets the global context
/// as current.
/// </remarks>
static autowiring::thread_specific_ptr<std::shared_ptr<CoreContext>> autoCurrentContext;

// Peer Context Constructor. Called interally by CreatePeer
CoreContext::CoreContext(std::shared_ptr<CoreContext> pParent, t_childList::iterator backReference, std::shared_ptr<CoreContext> pPeer) :
  m_pParent(pParent),
  m_backReference(backReference),
  m_stateBlock(new CoreContextStateBlock),
  m_beforeRunning(false),
  m_initiated(false),
  m_isShutdown(false),
  m_junctionBoxManager(
    pPeer ? pPeer->m_junctionBoxManager : std::make_shared<JunctionBoxManager>()
  )
{}

CoreContext::~CoreContext(void) {
  // Evict from the parent's child list first, if we have a parent:
  if(m_pParent)
  {
    // Notify AutowiringEvents listeners
    GetGlobal()->Invoke(&AutowiringEvents::ExpiredContext)(*this);

    // Also clear out any parent pointers:
    std::lock_guard<std::mutex> lk(m_pParent->m_stateBlock->m_lock);
    m_pParent->m_children.erase(m_backReference);
  }

  // The autoCurrentContext pointer holds a shared_ptr to this--if we're in a dtor, and our caller
  // still holds a reference to us, then we have a serious problem.
  assert(
    !autoCurrentContext.get() ||
    !autoCurrentContext.get()->use_count() ||
    autoCurrentContext.get()->get() != this
  );

  // Notify all ContextMember instances that their parent is going away
  NotifyTeardownListeners();

  // Make sure events aren't happening anymore:
  UnregisterEventReceiversUnsafe();

  // Tell all context members that we're tearing down:
  for(ContextMember* q : m_contextMembers)
    q->NotifyContextTeardown();
}

std::shared_ptr<CoreContext> CoreContext::CreateInternal(t_pfnCreate pfnCreate, std::shared_ptr<CoreContext> pPeer) {
  return CreateInternal(pfnCreate, pPeer, AutoInjectable());
}

std::shared_ptr<CoreContext> CoreContext::CreateInternal(t_pfnCreate pfnCreate, std::shared_ptr<CoreContext> pPeer, AutoInjectable&& inj)
{
  // don't allow new children if shutting down
  if(m_isShutdown)
    throw autowiring_error("Cannot create a child context; this context is already shut down");
    
  t_childList::iterator childIterator;
  {
    // Lock the child list while we insert
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

    // Reserve a place in the list for the child
    childIterator = m_children.insert(m_children.end(), std::weak_ptr<CoreContext>());
  }

  // Create the shared pointer for the context--do not add the context to itself,
  // this creates a dangerous cyclic reference.
  std::shared_ptr<CoreContext> retVal = pfnCreate(shared_from_this(), childIterator, pPeer);

  // Notify AutowiringEvents listeners
  GetGlobal()->Invoke(&AutowiringEvents::NewContext)(*retVal);
  
  // Remainder of operations need to happen with the created context made current
  CurrentContextPusher pshr(retVal);

  // Inject before broadcasting the creation notice
  inj();

  // Fire all explicit bolts if not an "anonymous" context (has void sigil type)
  BroadcastContextCreationNotice(retVal->GetSigilType());

  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  // We only consider the context to be completely constructed at this point, after all bolts
  // have been fired, the injection has taken place, and we're about to return.  We delay
  // finalizing the introduction of the return value into the list to this point for that
  // reason.
  *childIterator = retVal;
  if(m_isShutdown)
    retVal->SignalShutdown();
  return retVal;
}

size_t CoreContext::GetChildCount(void) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  return m_children.size();
}

std::shared_ptr<CoreContext> CoreContext::FirstChild(void) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

  // Just return the first child we successfully obtain a shared pointer for:
  for(const auto& cur : m_children) {
    auto child = cur.lock();
    if(child)
      return child;
  }

  // Seems like we have no children, return here
  return std::shared_ptr<CoreContext>();
}

std::shared_ptr<CoreContext> CoreContext::NextSibling(void) const {
  // Root contexts do not have siblings
  if(!m_pParent)
    return std::shared_ptr<CoreContext>();

  // Our iterator will always be valid in our parent collection.  Take a copy, lock the parent collection down
  // to prevent it from being modified, and then see what happens when we increment

  std::lock_guard<std::mutex> lk(m_pParent->m_stateBlock->m_lock);
  for(
    auto cur = m_backReference;
    ++cur != m_pParent->m_children.end();
  ) {
    auto sibling = cur->lock();
    if(sibling)
      return sibling;
  }

  // Failed to lock any successor child in the parent context, return unsuccessful
  return std::shared_ptr<CoreContext>();
}

const std::type_info& CoreContext::GetAutoTypeId(const AnySharedPointer& ptr) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

  const std::type_info& ti = ptr->type();
  auto q = m_typeMemos.find(ti);
  if (q == m_typeMemos.end() || !q->second.pObjTraits)
    throw autowiring_error("Attempted to obtain the true type of a shared pointer that was not a member of this context");

  const ObjectTraits* pObjTraits = q->second.pObjTraits;
  return pObjTraits->type;
}

std::shared_ptr<Object> CoreContext::IncrementOutstandingThreadCount(void) {
  std::shared_ptr<Object> retVal = m_outstanding.lock();
  if(retVal)
    return retVal;

  auto self = shared_from_this();

  // Increment the parent's outstanding count as well.  This will be held by the lambda, and will cause the enclosing
  // context's outstanding thread count to be incremented by one as long as we have any threads still running in our
  // context.  This property is relied upon in order to get the Wait function to operate properly.
  std::shared_ptr<Object> parentCount;
  if(m_pParent)
    parentCount = m_pParent->IncrementOutstandingThreadCount();

  retVal.reset(
    (Object*)1,
    [this, self, parentCount](Object*) {
      // Object being destroyed, notify all recipients
      std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

      // Unfortunately, this destructor callback is made before weak pointers are
      // invalidated, which requires that we manually reset the outstanding count
      m_outstanding.reset();

      // Wake everyone up
      m_stateBlock->m_stateChanged.notify_all();
    }
  );
  m_outstanding = retVal;
  return retVal;
}

void CoreContext::AddInternal(const ObjectTraits& traits) {
  {
    std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);

    // Validate that this addition does not generate an ambiguity.  We need to use the actual type of
    // the value, rather than the type passed in via traits.type, because the proper type might be a
    // concrete type defined in another context or potentially a unifier type.  Creating a slot here
    // is also undesirable because the complete type is not available and we can't create a dynaimc
    // caster to identify when this slot gets satisfied.
    auto q = m_typeMemos.find(traits.actual_type);
    if(q != m_typeMemos.end()) {
      auto& v = q->second;
      if(*v.m_value == traits.pObject)
        throw std::runtime_error("An attempt was made to add the same value to the same context more than once");
      if(*v.m_value)
        throw std::runtime_error("An attempt was made to add the same type to the same context more than once");
    }

    // Add the new concrete type:
    m_concreteTypes.push_back(traits);

    // Insert each context element:
    if(traits.pContextMember)
      AddContextMember(traits.pContextMember);

    if(traits.pFilter)
      m_filters.push_back(traits.pFilter.get());

    if(traits.pBoltBase)
      AddBolt(traits.pBoltBase);

    // Notify any autowiring field that is currently waiting that we have a new member
    // to be considered.
    UpdateDeferredElements(std::move(lk), m_concreteTypes.back());
  }
  
  // Moving this outside the lock because AddCoreRunnable will perform the checks inside its function
  if(traits.pCoreRunnable)
    AddCoreRunnable(traits.pCoreRunnable);

  // Event receivers:
  if(traits.receivesEvents) {
    JunctionBoxEntry<Object> entry(this, traits.pObject);

    // Add to our vector of local receivers first:
    (std::lock_guard<std::mutex>)m_stateBlock->m_lock,
    m_eventReceivers.insert(entry);

    // Recursively add to all junction box managers up the stack:
    AddEventReceiver(entry);
  }

  // Subscribers, if applicable:
  const auto& stump = traits.stump;
  if(traits.subscriber) {
    AddPacketSubscriber(traits.subscriber);

    // Ancilliary subscribers, if present:
    for(const auto* pCur = stump.pFirstAutoFilter; pCur; pCur = pCur->pFlink) {
      AutoFilterDescriptor subscriber(traits.subscriber.GetAutoFilter(), pCur->stub);
      AddPacketSubscriber(subscriber);
    }
  }
  else if(stump.pFirstAutoFilter)
    throw autowiring_error("It is an error to make use of NewAutoFilter in a type which does not have an AutoFilter member; please provide an AutoFilter method on this type");

  // Signal listeners that a new object has been created
  GetGlobal()->Invoke(&AutowiringEvents::NewObject)(*this, traits);
}

void CoreContext::FindByType(AnySharedPointer& reference) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  FindByTypeUnsafe(reference);
}

CoreContext::MemoEntry& CoreContext::FindByTypeUnsafe(AnySharedPointer& reference) const {
  const std::type_info& type = reference->type();

  // If we've attempted to search for this type before, we will return the value of the memo immediately:
  auto q = m_typeMemos.find(type);
  if(q != m_typeMemos.end()) {
    // We can copy over and return here
    reference = q->second.m_value;
    return q->second;
  }

  // Resolve based on iterated dynamic casts for each concrete type:
  const ObjectTraits* pObjTraits = nullptr;
  for(const auto& type : m_concreteTypes) {
    if(!reference->try_assign(*type.value))
      // No match, try the next entry
      continue;

    if (pObjTraits)
      // Resolution ambiguity, cannot proceed
      throw autowiring_error("An attempt was made to resolve a type which has multiple possible clients");

    // Update the object traits reference:
    pObjTraits = &type;
  }

  // This entry was not formerly memoized.  Memoize unconditionally.
  MemoEntry& retVal = m_typeMemos[type];
  retVal.m_value = reference;
  retVal.pObjTraits = pObjTraits;
  return retVal;
}

void CoreContext::FindByTypeRecursive(AnySharedPointer& reference, const AutoSearchLambda& searchFn) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  FindByTypeUnsafe(reference);
  if(!m_pParent || reference)
    // Type satisfied in current context, or there is nowhere to recurse to--call the terminal function in any case
    return searchFn();

  // Recurse while holding lock on this context
  // NOTE: Racing Deadlock is only possible if there is an ambiguity or cycle in the traversal order
  // of contexts.  Because context trees are guaranteed to be acyclical and are also fixed at construction
  // time, a strict locking heirarchy is inferred, and a deadlock therefore impossible.
  m_pParent->FindByTypeRecursive(reference, searchFn);
}

std::shared_ptr<CoreContext> CoreContext::GetGlobal(void) {
  return std::static_pointer_cast<CoreContext, GlobalCoreContext>(GlobalCoreContext::Get());
}

std::vector<std::shared_ptr<BasicThread>> CoreContext::CopyBasicThreadList(void) const {
  std::vector<std::shared_ptr<BasicThread>> retVal;

  // It's safe to enumerate this list from outside of a protective lock because a linked list
  // has stable iterators, we do not delete entries from the interior of this list, and we only
  // add entries to the end of the list.
  for(CoreRunnable* q : m_threads){
    BasicThread* thread = dynamic_cast<BasicThread*>(q);
    if (thread)
      retVal.push_back(thread->GetSelf<BasicThread>());
  }
  return retVal;
}

void CoreContext::Initiate(void) {
  // First-pass check, used to prevent recursive deadlocks traceable to here that might
  // result from entities trying to initiate subcontexts from CoreRunnable::Start
  if(m_initiated || m_isShutdown)
    return;

  {
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
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

  // Now we can add the event receivers we haven't been able to add because the context
  // wasn't yet started:
  AddEventReceivers(m_delayedEventReceivers);
  m_delayedEventReceivers.clear();
  m_junctionBoxManager->Initiate();

  // Reacquire the lock to prevent m_threads from being modified while we sit on it
  auto outstanding = IncrementOutstandingThreadCount();
  
  // Get the beginning of the thread list that we have at the time of lock acquisition
  t_threadList::iterator beginning;
  
  {
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
    beginning = m_threads.begin();
    
    // Signal our condition variable
    m_stateBlock->m_stateChanged.notify_all();
  }

  for (auto q = beginning; q != m_threads.end(); ++q)
    (*q)->Start(outstanding);
}

void CoreContext::InitiateCoreThreads(void) {
  Initiate();
}

void CoreContext::SignalShutdown(bool wait, ShutdownMode shutdownMode) {
  // As we signal shutdown, there may be a CoreRunnable that is in the "running" state.  If so,
  // then we will skip that thread as we signal the list of threads to shutdown.
  t_threadList::iterator firstThreadToStop;
  
  // Wipe out the junction box manager, notify anyone waiting on the state condition:
  {
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
    UnregisterEventReceiversUnsafe();
    m_isShutdown = true;
    
    firstThreadToStop = m_threads.begin();
    if (m_beforeRunning)
      ++firstThreadToStop;
    
    m_stateBlock->m_stateChanged.notify_all();
  }

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
      std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

      // Fill strong lock series in order to ensure proper teardown interleave:
      childrenInterleave.reserve(m_children.size());
      for(const auto& entry : m_children) {
        auto childContext = entry.lock();

        // Technically, it *is* possible for this weak pointer to be expired, even though
        // we're holding the lock.  This may happen if the context itself is exiting even
        // as we are processing SignalTerminate.  In that case, the child context in
        // question is blocking in its dtor lambda, waiting patiently until we're done,
        // at which point it will modify the m_children collection.
        if(!childContext)
          continue;

        // Add to the interleave so we can SignalTerminate in a controlled way.
        childrenInterleave.push_back(childContext);
      }
    }

    // Now that we have a locked-down, immutable series, begin termination signalling:
    for(size_t i = childrenInterleave.size(); i--; )
      childrenInterleave[i]->SignalShutdown(wait);
  }

  // Pass notice to all child threads:
  bool graceful = (shutdownMode == ShutdownMode::Graceful);
  for (auto itr = firstThreadToStop; itr != m_threads.end(); ++itr)
    (*itr)->Stop(graceful);

  // Signal our condition variable
  m_stateBlock->m_stateChanged.notify_all();

  // Short-return if required:
  if(!wait)
    return;

  // Wait for the treads to finish before returning.
  for (CoreRunnable* runnable : m_threads)
    runnable->Wait();
}

void CoreContext::Wait(void) {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
  m_stateBlock->m_stateChanged.wait(lk, [this] {return m_isShutdown && this->m_outstanding.expired(); });
}

bool CoreContext::Wait(const std::chrono::nanoseconds duration) {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
  return m_stateBlock->m_stateChanged.wait_for(lk, duration, [this] {return m_isShutdown && this->m_outstanding.expired(); });
}

bool CoreContext::DelayUntilInitiated(void) {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
  m_stateBlock->m_stateChanged.wait(lk, [this] {return m_initiated || m_isShutdown;});
  return !m_isShutdown;
}

std::shared_ptr<CoreContext> CoreContext::CurrentContext(void) {
  if(!autoCurrentContext.get())
    return std::static_pointer_cast<CoreContext, GlobalCoreContext>(GetGlobalContext());

  std::shared_ptr<CoreContext>* retVal = autoCurrentContext.get();
  assert(retVal);
  assert(*retVal);
  return *retVal;
}

void CoreContext::AddCoreRunnable(const std::shared_ptr<CoreRunnable>& ptr) {
  // Performing a double check.
  bool shouldRun;
  {
    std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
    
    // Insert into the linked list of threads first:
    m_threads.push_front(ptr.get());
    
    // Check if we're already running, this means we're late to the party and need to start _now_.
    shouldRun = m_initiated;
    
    // Signal that we are in the "running"
    m_beforeRunning = true;
  }

  // Run this thread without the lock
  if(shouldRun)
    ptr->Start(IncrementOutstandingThreadCount());
  

  // Check if the stop signal was sent between the time we started running until now.  If so, then
  // we will stop the thread manually here.
  bool shouldStopHere;
  {
    std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
    
    // Signal that we have stopped "running"
    m_beforeRunning = false;
    
    // If SignalShutdown() was invoked while we were "running", then we will need to stop this thread ourselves
    shouldStopHere = m_isShutdown;
  }

  if(shouldStopHere)
    ptr->Stop(false);
}

void CoreContext::AddBolt(const std::shared_ptr<BoltBase>& pBase) {
  for(auto cur = pBase->GetContextSigils(); *cur; cur++){
    m_nameListeners[**cur].push_back(pBase.get());
  }

  if(!*pBase->GetContextSigils())
    m_nameListeners[typeid(void)].push_back(pBase.get());
}

void CoreContext::BuildCurrentState(void) {
  AutoGlobalContext glbl;
  glbl->Invoke(&AutowiringEvents::NewContext)(*this);
    
  // Enumerate objects injected into this context
  for(auto& object : m_concreteTypes) {
    GetGlobal()->Invoke(&AutowiringEvents::NewObject)(*this, object);
  }
  
  // Recurse on all children
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  for(const auto& c : m_children) {
    auto cur = c.lock();
    if(!cur)
      continue;

    // Recurse into the child instance:
    cur->BuildCurrentState();
  }
}

void CoreContext::CancelAutowiringNotification(DeferrableAutowiring* pDeferrable) {
  if (!pDeferrable)
    return;

  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  auto q = m_typeMemos.find(pDeferrable->GetType());
  if(q == m_typeMemos.end())
    return;

  // Always finalize this entry:
  auto strategy = pDeferrable->GetStrategy();
  if(strategy)
    strategy->Finalize(pDeferrable);

  // Stores the immediate predecessor of the node we will linearly scan for in our
  // linked list.
  DeferrableAutowiring* prior = nullptr;

  // Now remove the entry from the list:
  // NOTE:  If a performance bottleneck is tracked to here, the solution is to use
  // a doubly-linked list.
  for(auto cur = q->second.pFirst; cur && cur != pDeferrable; prior = cur, cur = cur->GetFlink())
    if(!cur)
      // Ran off the end of the list, nothing we can do here
      return;

  if(prior)
    // Erase the entry by using link elision:
    prior->SetFlink(pDeferrable->GetFlink());

  if(pDeferrable == q->second.pFirst)
    // Current deferrable is at the head, update the flink:
    q->second.pFirst = pDeferrable->GetFlink();
}

void CoreContext::Dump(std::ostream& os) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  
  for(const auto& entry : m_typeMemos) {
    os << autowiring::demangle(entry.first);
    const void* pObj = entry.second.m_value->ptr();
    if(pObj)
      os << " 0x" << std::hex << pObj;
    os << std::endl;
  }

  for(CoreRunnable* runnable : m_threads) {
    BasicThread* pThread = dynamic_cast<BasicThread*>(runnable);
    if (!pThread) continue;

    const char* name = pThread->GetName();
    os << "Thread " << pThread << " " << (name ? name : "(no name)") << std::endl;
  }
}

void ShutdownCurrentContext(void) {
  CoreContext::CurrentContext()->SignalShutdown();
}

void CoreContext::UnregisterEventReceiversUnsafe(void) {
  // Release all event receivers originating from this context:
  for(const auto& entry : m_eventReceivers)
    m_junctionBoxManager->RemoveEventReceiver(entry);

  // Notify our parent (if we have one) that our event receivers are going away:
  if(m_pParent)
    m_pParent->RemoveEventReceivers(m_eventReceivers);

  // Recursively unregister packet factory subscribers:
  AnySharedPointerT<AutoPacketFactory> pf;
  FindByTypeUnsafe(pf);

  // Wipe out all collections so we don't try to free these multiple times:
  m_eventReceivers.clear();
}

void CoreContext::BroadcastContextCreationNotice(const std::type_info& sigil) const {
  auto listeners = m_nameListeners.find(sigil);
  if(listeners != m_nameListeners.end()) {
    // Iterate through all listeners:
    for(BoltBase* bolt : listeners->second)
      bolt->ContextCreated();
  }

  // In the case of an anonymous sigil type, we do not notify the all-types
  // listeners a second time.
  if(sigil != typeid(void)) {
    listeners = m_nameListeners.find(typeid(void));
    if(listeners != m_nameListeners.end())
      for(BoltBase* bolt : listeners->second)
        bolt->ContextCreated();
  }

  // Notify the parent next:
  if(m_pParent)
    m_pParent->BroadcastContextCreationNotice(sigil);
}

void CoreContext::UpdateDeferredElements(std::unique_lock<std::mutex>&& lk, const ObjectTraits& entry) {
  // Collection of satisfiable lists:
  std::vector<std::pair<const DeferrableUnsynchronizedStrategy*, DeferrableAutowiring*>> satisfiable;

  // Notify any autowired field whose autowiring was deferred.  We do this by processing each entry
  // in the entire type memos collection.  These entries are keyed on the type of the memo, and the
  // value is a linked list of trees of deferred autowiring instances that will need to be called
  // if the corresponding memo type has been satisfied.
  //
  // A tree data structure is used, here, specifically because there are cases where child nodes
  // on a tree should only be called if and only if the root node is still present.  For instance,
  // creating an Autowired field adds a tree to this list with the root node referring to the
  // Autowired field itself, and then invoking Autowired::NotifyWhenAutowired attaches a child to
  // this tree.  If the Autowired instance is destroyed, the lambda registered for notification is
  // also removed at the same time.
  //
  // Each connected nonroot deferrable autowiring is referred to as a "dependant chain".
  std::stack<DeferrableAutowiring*> stk;
  for(auto& cur : m_typeMemos) {
    MemoEntry& value = cur.second;

    if(value.m_value)
      // This entry is already satisfied, no need to process it
      continue;

    // Determine whether the current candidate element satisfies the autowiring we are considering.
    // This is done internally via a dynamic cast on the interface type for which this polymorphic
    // base type was constructed.
    if(!value.m_value->try_assign(entry.pObject))
      continue;

    // Success, assign the traits
    value.pObjTraits = &entry;

    // Now we need to take on the responsibility of satisfying this deferral.  We will do this by
    // nullifying the flink, and by ensuring that the memo is satisfied at the point where we
    // release the lock.
    stk.push(value.pFirst);
    value.pFirst = nullptr;

    // Finish satisfying the remainder of the chain while we hold the lock:
    while(!stk.empty()) {
      auto top = stk.top();
      stk.pop();

      for(DeferrableAutowiring* pNext = top; pNext; pNext = pNext->GetFlink()) {
        pNext->SatisfyAutowiring(value.m_value);

        // See if there's another chain we need to process:
        auto child = pNext->ReleaseDependentChain();
        if(child)
          stk.push(child);

        // Not everyone needs to be finalized.  The entities that don't require finalization
        // are identified by an empty strategy, and we just skip them.
        auto strategy = pNext->GetStrategy();
        if(strategy)
          satisfiable.push_back(
            std::make_pair(strategy, pNext)
          );
      }
    }
  }

  // Give children a chance to also update their deferred elements:
  for(const auto& weak_child : m_children) {
    // Hold reference to prevent this iterator from becoming invalidated:
    auto ctxt = weak_child.lock();
    if(!ctxt)
      continue;

    // Reverse lock before satisfying children:
    lk.unlock();
    ctxt->UpdateDeferredElements(
      std::unique_lock<std::mutex>(ctxt->m_stateBlock->m_lock),
      entry
    );
    lk.lock();
  }
  lk.unlock();

  // Run through everything else and finalize it all:
  for(const auto& cur : satisfiable)
    cur.first->Finalize(cur.second);
}

void CoreContext::AddEventReceiver(const JunctionBoxEntry<Object>& entry) {
  {
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

    if (!m_initiated) {
      // Delay adding receiver until context is initialized
      m_delayedEventReceivers.insert(entry);
      return;
    }
  }

  m_junctionBoxManager->AddEventReceiver(entry);

  // Delegate ascending resolution, where possible.  This ensures that the parent context links
  // this event receiver to compatible senders in the parent context itself.
  if(m_pParent)
    m_pParent->AddEventReceiver(entry);
}


void CoreContext::AddEventReceivers(const t_rcvrSet& receivers) {
  // Must be initiated
  assert(m_initiated);

  for(const auto& q : receivers)
    m_junctionBoxManager->AddEventReceiver(q);

  // Delegate ascending resolution, where possible.  This ensures that the parent context links
  // this event receiver to compatible senders in the parent context itself.
  if(m_pParent)
    m_pParent->AddEventReceivers(receivers);
}

void CoreContext::RemoveEventReceivers(const t_rcvrSet& receivers) {
  for(const auto& q : receivers)
    m_junctionBoxManager->RemoveEventReceiver(q);

  // Detour to the parent collection (if necessary)
  if(m_pParent)
    m_pParent->RemoveEventReceivers(receivers);
}

void CoreContext::UnsnoopEvents(Object* oSnooper, const JunctionBoxEntry<Object>& receiver) {
  {
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
    if(
      // If the passed value is currently a snooper, then the caller has snooped a context and also
      // one of its parents.  End here.
      m_snoopers.count(oSnooper) ||

      // If we are an event receiver in this context, then the caller has snooped a context and
      // is also a proper EventReceiver in one of that context's ancestors--this is a fairly
      // common case.  We return here, rather than incorrectly unregistering the caller.
      m_eventReceivers.count(receiver)
    )
      return;

    m_delayedEventReceivers.erase(receiver);
  }
  
  // Always remove from this junction box manager:
  m_junctionBoxManager->RemoveEventReceiver(receiver);

  // Handoff to parent type:
  if(m_pParent)
    m_pParent->UnsnoopEvents(oSnooper, receiver);
}

void CoreContext::FilterException(void) {
  bool handled = false;
  for(ExceptionFilter* filter : m_filters) {
    try {
      filter->Filter();
      handled = true;
    } catch(...) {
      // Do nothing
    }
  }

  // Pass to parent if one exists:
  if(m_pParent) {
    try {
      // See if the parent wants to handle this exception:
      m_pParent->FilterException();

      // Parent handled it, we're good to go
      return;
    } catch(...) {
      // Do nothing
    }
  }

  // Rethrow if unhandled:
  if(!handled)
    throw;
}

void CoreContext::FilterFiringException(const JunctionBoxBase* pProxy, Object* pRecipient) {
  // Filter in order:
  for(CoreContext* pCur = this; pCur; pCur = pCur->GetParentContext().get())
    for(ExceptionFilter* filter : pCur->m_filters) {
      try {
        filter->Filter(pProxy, pRecipient);
      } catch(...) {
        // Do nothing, filter didn't want to filter this exception
      }
    }
}

void CoreContext::AddDeferredUnsafe(DeferrableAutowiring* deferrable) {
  // Determine whether a type memo exists right now for the thing we're trying to defer.  If it doesn't
  // exist, we need to inject one in order to allow deferred satisfaction to know what kind of type we
  // are trying to satisfy at this point.
  size_t found = m_typeMemos.count(deferrable->GetType());

  if(!found)
    // Slot not presently initialized, need to initialize it:
    m_typeMemos[deferrable->GetType()].m_value = deferrable->GetSharedPointer();

  // Obtain the entry (potentially a second time):
  MemoEntry& entry = m_typeMemos[deferrable->GetType()];

  // Chain forward the linked list:
  deferrable->SetFlink(entry.pFirst);
  entry.pFirst = deferrable;
}

void CoreContext::InsertSnooper(std::shared_ptr<Object> snooper) {
  (std::lock_guard<std::mutex>)m_stateBlock->m_lock,
    m_snoopers.insert(snooper.get());
}

void CoreContext::RemoveSnooper(std::shared_ptr<Object> snooper) {
  (std::lock_guard<std::mutex>)m_stateBlock->m_lock,
  m_snoopers.erase(snooper.get());
}

void CoreContext::AddContextMember(const std::shared_ptr<ContextMember>& ptr) {
  // Always add to the set of context members
  m_contextMembers.push_back(ptr.get());
}

void CoreContext::AddPacketSubscriber(const AutoFilterDescriptor& rhs) {
  Inject<AutoPacketFactory>()->AddSubscriber(rhs);
}

void CoreContext::UnsnoopAutoPacket(const ObjectTraits& traits) {
  {
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
    
    // If the passed value is currently a snooper, then the caller has snooped a context and also
    // one of its parents.  End here.
    if ( m_snoopers.count(traits.pObject.get()) )
      return;
  }
  
  // Always remove from this context's PacketFactory:
  Inject<AutoPacketFactory>()->RemoveSubscriber(traits.subscriber);
  
  // Handoff to parent:
  if (m_pParent)
    m_pParent->UnsnoopAutoPacket(traits);
}

std::ostream& operator<<(std::ostream& os, const CoreContext& rhs) {
  rhs.Dump(os);
  return os;
}

std::shared_ptr<CoreContext> CoreContext::SetCurrent(void) {
  std::shared_ptr<CoreContext> newCurrent = this->shared_from_this();

  if(!newCurrent)
    throw std::runtime_error("Attempted to make a CoreContext current from a CoreContext ctor");

  std::shared_ptr<CoreContext> retVal = CoreContext::CurrentContext();
  autoCurrentContext.reset(new std::shared_ptr<CoreContext>(newCurrent));
  return retVal;
}

void CoreContext::EvictCurrent(void) {
  autoCurrentContext.reset();
}
