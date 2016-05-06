// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "CoreContext.h"
#include "AutoPacketFactory.h"
#include "AutowirableSlot.h"
#include "CoreThread.h"
#include "demangle.h"
#include "GlobalCoreContext.h"
#include "ManualThreadPool.h"
#include "MicroBolt.h"
#include "NullPool.h"
#include "SystemThreadPool.h"
#include "thread_specific_ptr.h"
#include <cassert>
#include <sstream>
#include <stdexcept>

using namespace autowiring;

static_assert(!std::is_copy_constructible<CoreContext>::value, "Copy constructor incorrectly inferred on CoreContext");

class DelayedContextHold:
  public CoreRunnable
{
public:
  DelayedContextHold(std::shared_ptr<CoreContext> context):
    m_context(context)
  {}

  // A pointer to the enclosing context.  This pointer is held until this instance is started or stopped
  std::shared_ptr<CoreContext> m_context;

  bool OnStart(void) override {
    m_context.reset();
    return false;
  }

  void OnStop(bool) override {
    m_context.reset();
  }
};

/// <summary>
/// A pointer to the current context, specific to the current thread.
/// </summary>
/// <remarks>
/// All threads have a current context, and this pointer refers to that current context.  If this value is null,
/// then the current context is the global context.  It's very important that threads not attempt to hold a reference
/// to the global context directly because it could change teardown order if the main thread sets the global context
/// as current.
/// </remarks>
static thread_specific_ptr<std::shared_ptr<CoreContext>> autoCurrentContext;

// Peer Context Constructor. Called interally by CreatePeer
CoreContext::CoreContext(const std::shared_ptr<CoreContext>& pParent, t_childList::iterator backReference, auto_id sigilType) :
  m_pParent(pParent),
  m_backReference(backReference),
  SigilType(sigilType),
  AncestorCount(pParent ? pParent->AncestorCount + 1 : 0),
  m_stateBlock(std::make_shared<CoreContextStateBlock>(pParent ? pParent->m_stateBlock : nullptr))
{}

CoreContext::~CoreContext(void) {
  // Evict from the parent's child list first, if we have a parent:
  if(m_pParent)
  {
    expiredContext();

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
  onTeardown(*this);

  // Tell all context members that we're tearing down:
  for(ContextMember* q : m_contextMembers)
    q->NotifyContextTeardown();

  // Perform unlinking, if requested:
  if(m_unlinkOnTeardown)
    for (const auto& ccType : m_concreteTypes) {
      uint8_t* pBase = (uint8_t*)ccType.value.ptr();

      // Enumerate all slots and unlink them one at a time
      for (auto cur = ccType.stump->pHead; cur; cur = cur->pFlink) {
        if (cur->autoRequired)
          // Only unlink slots that were Autowired.  AutoRequired slots will never participate
          // in a cycle (because we would wind up with constructive chaos) so we don't really
          // need to worry about them.  Furthermore, there are cases where users may want to
          // refer to a context member in their destructor; in that case, they should use
          // AutoRequired to enforce the relationship.
          continue;

        auto& slot = *reinterpret_cast<DeferrableAutowiring*>(pBase + cur->slotOffset);
        if (!slot.IsAutowired())
          // Nothing to do here, just short-circuit
          continue;

        auto q = m_typeMemos.find(slot.GetType());
        if (q == m_typeMemos.end())
          // Weird.  A slot is present on a member of this context, but the wired type doesn't
          // have a memo entry anywhere.
          continue;

        if (!q->second.m_local)
          // Entry exists, but was not locally satisfied.  We can circle around.
          continue;

        // OK, interior pointer and context teardown is underway, clear it out
        slot.reset();
      }
    }
}

std::shared_ptr<CoreContext> CoreContext::CreateInternal(t_pfnCreate pfnCreate)
{
  // don't allow new children if shutting down
  if (IsShutdown())
    throw dispatch_aborted_exception("Cannot create a child context; this context is already shut down");

  t_childList::iterator childIterator;
  {
    // Lock the child list while we insert
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

    // Reserve a place in the list for the child
    childIterator = m_children.insert(m_children.end(), std::weak_ptr<CoreContext>());
  }

  // Create the shared pointer for the context--do not add the context to itself,
  // this creates a dangerous cyclic reference.
  std::shared_ptr<CoreContext> retVal = pfnCreate(shared_from_this(), childIterator);

  // Notify AutowiringEvents listeners
  newContext(retVal.get());

  // Remainder of operations need to happen with the created context made current
  CurrentContextPusher pshr(retVal);

  // If we're currently running, we would like the child context to know that it can optionally
  // transition directly to the running state without having to wait in Initiated
  if (IsRunning())
    retVal->m_state = State::CanRun;

  // Fire all explicit bolts if not an "anonymous" context (has void sigil type)
  BroadcastContextCreationNotice(retVal->GetSigilType());

  // We only consider the context to be completely constructed at this point, after all bolts
  // have been fired, the injection has taken place, and we're about to return.  We delay
  // finalizing the introduction of the return value into the list to this point for that
  // reason.
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  *childIterator = retVal;
  if(IsShutdown())
    retVal->SignalShutdown();
  return retVal;
}

size_t CoreContext::GetChildCount(void) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  return m_children.size();
}

std::vector<CoreRunnable*> CoreContext::GetRunnables(void) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  return std::vector<CoreRunnable*>(m_threads.begin(), m_threads.end());
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
  return nullptr;
}

std::shared_ptr<CoreContext> CoreContext::NextSibling(void) const {
  // Root contexts do not have siblings
  if(!m_pParent)
    return nullptr;

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
  return nullptr;
}

auto_id CoreContext::GetAutoTypeId(const AnySharedPointer& ptr) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

  auto ti = ptr.type();
  auto q = m_typeMemos.find(ti);
  if (q == m_typeMemos.end() || !q->second.pObjTraits)
    throw autowiring_error("Attempted to obtain the true type of a shared pointer that was not a member of this context");

  const CoreObjectDescriptor* pObjTraits = q->second.pObjTraits;
  return pObjTraits->type;
}

void CoreContext::AddInternal(const CoreObjectDescriptor& traits) {
  {
    std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);

    // Validate that this addition does not generate an ambiguity.  We need to use the actual type of
    // the value, rather than the type passed in via traits.type, because the proper type might be a
    // concrete type defined in another context or potentially a unifier type.  Creating a slot here
    // is also undesirable because the complete type is not available and we can't create a dynaimc
    // caster to identify when this slot gets satisfied. If a slot was non-local, overwrite it.
    auto q = m_typeMemos.find(traits.actual_type);
    if(q != m_typeMemos.end()) {
      auto& v = q->second;

      if (v.m_local) {
        if (traits.pCoreObject && *v.m_value == traits.pCoreObject)
          throw autowiring_error("An attempt was made to add the same value to the same context more than once");
        if (*v.m_value)
          throw autowiring_error("An attempt was made to add the same type to the same context more than once");
      }
      else {
        v.m_value = traits.value;
        v.m_local = true;
      }
    }

    // These are all trivial containers that we take the opportunity to update here while we are
    // under lock.  Changes to these containers do not cause any signals to be asserted so we are
    // safe to do this.
    m_concreteTypes.push_back(traits);
    if(traits.pContextMember)
      m_contextMembers.push_back(traits.pContextMember.get());
    if(traits.pFilter)
      m_filters.push_back(traits.pFilter.get());

    // Notify any autowiring field that is currently waiting that we have a new member to be considered.
    UpdateDeferredElements(std::move(lk), m_concreteTypes.back(), true);
  }

  // Tell anyone interested that we are done adding the type
  m_stateBlock->m_stateChanged.notify_all();

  // All bolts--this also performs retroactive bolting
  if (traits.pBoltBase)
    AddBolt(traits.pBoltBase);

  // Moving this outside the lock because AddCoreRunnable will perform the checks inside its function
  if(traits.pCoreRunnable)
    AddCoreRunnable(traits.pCoreRunnable);

  // Subscribers, if applicable:
  if(!traits.subscriber.empty())
    AddPacketSubscriber(traits.subscriber);

  // All configuration watchers
  if (traits.pConfigWatcher)
    AddConfigWatcher(traits.pConfigWatcher);

  // Signal listeners that a new object has been created
  newObject(traits);
}

void CoreContext::AddInternal(const AnySharedPointer& ptr) {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);

  // Verify that this type isn't already satisfied
  MemoEntry& entry = m_typeMemos[ptr.type()];
  if (entry.m_local && entry.m_value)
    throw autowiring_error("This interface is already present in the context");

  // Now we can satisfy it:
  entry.m_local = true;
  entry.m_value = ptr;
  UpdateDeferredElement(std::move(lk), entry);
}

MemoEntry& CoreContext::FindByType(auto_id type, bool nonrecursive) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  return FindByTypeUnsafe(type, nonrecursive);
}

MemoEntry& CoreContext::FindByTypeUnsafe(auto_id type, bool nonrecursive) const {
  // If we've attempted to search for this type before, we will return the value of the memo immediately:
  auto q = m_typeMemos.find(type);
  if(q != m_typeMemos.end())
    // Done, can return here
    return q->second;

  // Ensure the memo at least receives a default value:
  MemoEntry& retVal = m_typeMemos[type];
  retVal.m_value = type;

  // Resolve based on iterated dynamic casts for each concrete type:
  for(const auto& concreteType : m_concreteTypes) {
    if (type == concreteType.type)
      // Exact match, no dynamic casting required:
      retVal.m_value = concreteType.value;
    else if(type.block->pFromObj) {
      // Dynamic match next
      auto fromObj = type.block->pFromObj(concreteType.pCoreObject);
      if (!fromObj)
        // No match, try the next entry
        continue;

      // Match!  Assign, and signal this entry preemptively
      retVal.onSatisfied = true;
      retVal.m_value = fromObj;
      retVal.m_local = true;
    }
    else
      // No caster available, we need to recycle
      continue;

    if (retVal.pObjTraits)
      // Resolution ambiguity, cannot proceed
      throw autowiring_error("An attempt was made to resolve a type which has multiple possible clients");

    // Update the object traits reference:
    retVal.pObjTraits = &concreteType;
  }

  if (nonrecursive || !m_pParent || retVal.m_value)
    return retVal;

  // Recurse to parent while holding lock
  auto& parentEntry = m_pParent->FindByType(type, nonrecursive);
  if (parentEntry.m_value) {
    // Memoize, nonlocal satisfaction
    retVal.m_value = parentEntry.m_value;
    retVal.pObjTraits = parentEntry.pObjTraits;
    retVal.m_local = false;
    retVal.onSatisfied = true;
    return parentEntry;
  }

  // Failure, return our own entry, the signal defined here will be satisfied
  return retVal;
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
  if (IsInitiated())
    return;

  // Get the beginning of the thread list that we have at the time of lock acquisition
  // New threads are added to the front of the thread list, which means that objects
  // after this iterator are the ones that will need to be started
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);

  // Now we can transition to initiated or running:
  switch (m_state) {
  case State::Initiated:
  case State::Running:
    // Double-check
    return;
  case State::NotStarted:
    if (IsGlobalContext())
      // Global context is permitted to transition directly to running
      m_state = State::Running;
    else
      // Transition to initiated state, can't start threads yet
      m_state = State::Initiated;
    break;
  case State::CanRun:
    // Parent already started, we can run if we want to
    m_state = State::Running;
    break;
  case State::Shutdown:
  case State::Abandoned:
    // Already in a terminal state
    return;
  }

  // Notify all child contexts that they can start if they want
  if (!IsRunning()) {
    lk.unlock();
    onInitiated();
    m_stateBlock->m_stateChanged.notify_all();

    // Need to inject a delayed context type so that this context will not be destroyed until
    // it has an opportunity to start.
    Inject<DelayedContextHold>(shared_from_this());
    return;
  }

  // Now we can recover the first thread that will need to be started
  auto beginning = m_threads.begin();
  lk.unlock();
  onInitiated();
  m_stateBlock->m_stateChanged.notify_all();

  if (beginning != m_threads.end()) {
    auto outstanding = m_stateBlock->IncrementOutstandingThreadCount(shared_from_this());
    for (auto q = beginning; q != m_threads.end(); ++q)
      (*q)->Start(outstanding);
  }

  // We assert this condition only after all threads have been at least notified that they can start
  onRunning();

  // Update state of children now that we are initated
  TryTransitionChildrenState();
}

void CoreContext::SignalShutdown(bool wait, ShutdownMode shutdownMode) {
  // As we signal shutdown, there may be a CoreRunnable that is in the "running" state.  If so,
  // then we will skip that thread as we signal the list of threads to shutdown.
  std::list<CoreRunnable*>::iterator firstThreadToStop;

  // Trivial return check
  if (IsShutdown())
    return;

  // Wipe out the junction box manager, notify anyone waiting on the state condition:
  {
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

    switch (m_state) {
    case State::NotStarted:
    case State::CanRun:
      // User never initiated
      m_state = State::Abandoned;
      break;
    case State::Initiated:
    case State::Running:
      // Initiate called, move to the shutdown state
      m_state = State::Shutdown;
      break;
    case State::Shutdown:
    case State::Abandoned:
      // Already shut down, no further work need be done
      return;
    }

    firstThreadToStop = m_threads.begin();
    if (m_beforeRunning)
      ++firstThreadToStop;
  }
  m_stateBlock->m_stateChanged.notify_all();
  onShutdown();

  // Teardown interleave assurance--all of these contexts will generally be destroyed
  // at the exit of this block, due to the behavior of SignalTerminate, unless exterior
  // context references (IE, related to snooping) exist.
  //
  // This is done in order to provide a stable collection that may be traversed during
  // teardown outside of a lock.
  std::vector<std::shared_ptr<CoreContext>> childrenInterleave;

  // Thread pool token and pool pointer
  std::shared_ptr<void> startToken;

  // Tear down all the children, evict thread pool:
  {
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

    startToken = std::move(m_startToken);
    m_startToken.reset();

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
    childrenInterleave[i]->SignalShutdown(false, shutdownMode);

  // Pass notice to all child threads:
  bool graceful = (shutdownMode == ShutdownMode::Graceful);
  for (auto itr = firstThreadToStop; itr != m_threads.end(); ++itr)
    (*itr)->Stop(graceful);

  // Wait if requested
  if(wait)
    Wait();
}

void CoreContext::Quiescent(void) const {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
  m_stateBlock->m_stateChanged.wait(lk, [this] { return m_stateBlock->m_outstanding.expired(); });
}

bool CoreContext::Quiescent(const std::chrono::nanoseconds duration) const {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
  return m_stateBlock->m_stateChanged.wait_for(lk, duration, [this] { return m_stateBlock->m_outstanding.expired(); });
}

bool CoreContext::IsQuiescent(void) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  return m_stateBlock->m_outstanding.expired();
}

void CoreContext::Wait(void) {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
  m_stateBlock->m_stateChanged.wait(lk, [this] { return IsShutdown() && m_stateBlock->m_outstanding.expired(); });
}

bool CoreContext::Wait(const std::chrono::nanoseconds duration) {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
  return m_stateBlock->m_stateChanged.wait_for(lk, duration, [this] { return IsShutdown() && m_stateBlock->m_outstanding.expired(); });
}

bool CoreContext::DelayUntilInitiated(void) {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
  m_stateBlock->m_stateChanged.wait(lk, [this] {return IsInitiated();});
  return !IsShutdown();
}

const std::shared_ptr<CoreContext>& CoreContext::CurrentContextOrNull(void) {
  static const std::shared_ptr<CoreContext> empty;
  auto retVal = autoCurrentContext.get();
  return retVal ? *retVal : empty;
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
    shouldRun = IsRunning();

    // Signal that we are in the "running"
    m_beforeRunning = true;
  }

  // Run this thread without the lock
  if(shouldRun)
    ptr->Start(m_stateBlock->IncrementOutstandingThreadCount(shared_from_this()));


  // Check if the stop signal was sent between the time we started running until now.  If so, then
  // we will stop the thread manually here.
  bool shouldStopHere;
  {
    std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);

    // Signal that we have stopped "running"
    m_beforeRunning = false;

    // If SignalShutdown() was invoked while we were "running", then we will need to stop this thread ourselves
    shouldStopHere = IsShutdown();
  }

  if(shouldStopHere)
    ptr->Stop(false);
}

void CoreContext::AddBolt(const std::shared_ptr<BoltBase>& pBase) {
  // Register all listeners as needed under lock
  {
    std::lock_guard<std::mutex> lk{ m_stateBlock->m_lock };
    for (auto cur = pBase->GetContextSigils(); *cur; cur++)
      m_nameListeners[*cur].push_back(pBase.get());
    if(!*pBase->GetContextSigils())
      m_nameListeners[{}].push_back(pBase.get());
  }

  // We intend to reset the context just once, when the function is done
  CurrentContextPusher pshr;

  // Post-hoc invocation of all already-created contexts:
  ContextEnumerator e{ shared_from_this() };
  for (auto q = e.begin(); ++q != e.end(); ) {
    if (!pBase->Matches(q->GetSigilType()))
      break;

    q->SetCurrent();
    pBase->ContextCreated();
  }
}

const AnySharedPointer& CoreContext::Await(auto_id id) {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
  MemoEntry& memo = FindByTypeUnsafe(id);
  if (!memo.m_value)
    // Need to wait for this value to be satisfied
    m_stateBlock->m_stateChanged.wait(
      lk,
      [this, &memo] {
        if(IsShutdown())
          throw dispatch_aborted_exception{};
        return memo.m_value != nullptr;
      }
    );

  return memo.m_value;
}

AnySharedPointer CoreContext::Await(auto_id id, std::chrono::nanoseconds timeout) {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);
  MemoEntry& memo = FindByTypeUnsafe(id);
  if (!memo.m_value)
    // Need to wait for this value to be satisfied
    if(!m_stateBlock->m_stateChanged.wait_for(
        lk,
        timeout,
        [this, &memo] {
          if(IsShutdown())
            throw dispatch_aborted_exception{};
          return memo.m_value != nullptr;
        }
      )
    )
      return{};

  return memo.m_value;
}

std::vector<const autowiring::CoreObjectDescriptor*> CoreContext::BuildObjectState(void) const {
  std::vector<const autowiring::CoreObjectDescriptor*> retVal;
  retVal.reserve(m_concreteTypes.size());

  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);
  for(const auto& obj : m_concreteTypes)
    retVal.push_back(&obj);
  return retVal;
}

void CoreContext::Dump(std::ostream& os) const {
  std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

  for(const auto& entry : m_typeMemos) {
    os << demangle(entry.first);
    const void* pObj = entry.second.m_value.ptr();
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

void CoreContext::BroadcastContextCreationNotice(auto_id sigil) const {
  auto listeners = m_nameListeners.find(sigil);
  if(listeners != m_nameListeners.end()) {
    // Iterate through all listeners:
    for(BoltBase* bolt : listeners->second)
      bolt->ContextCreated();
  }

  // In the case of an anonymous sigil type, we do not notify the all-types
  // listeners a second time.
  if (sigil != auto_id{}) {
    listeners = m_nameListeners.find({});
    if(listeners != m_nameListeners.end())
      for(BoltBase* bolt : listeners->second)
        bolt->ContextCreated();
  }

  // Notify the parent next:
  if(m_pParent)
    m_pParent->BroadcastContextCreationNotice(sigil);
}

void CoreContext::UpdateDeferredElement(std::unique_lock<std::mutex>&& lk, MemoEntry& entry) {
  // Satisfy what needs to be satisfied:
  lk.unlock();
  entry.onSatisfied();

  // Give children a chance to also update their deferred elements:
  lk.lock();
  for (const auto& weak_child : m_children) {
    // Hold reference to prevent this iterator from becoming invalidated:
    auto ctxt = weak_child.lock();
    if (!ctxt)
      continue;

    // Reverse lock before satisfying children:
    lk.unlock();
    ctxt->UpdateDeferredElement(
      std::unique_lock<std::mutex>(ctxt->m_stateBlock->m_lock),
      entry
    );
    lk.lock();
  }
  lk.unlock();
}

void CoreContext::UpdateDeferredElements(std::unique_lock<std::mutex>&& lk, const CoreObjectDescriptor& entry, bool local) {
  {
    std::vector<MemoEntry*> entries;

    // Notify any autowired field whose autowiring was deferred.  We do this by processing each entry
    // in the entire type memos collection.
    for (auto& cur : m_typeMemos) {
      MemoEntry& value = cur.second;

      if (value.m_value && value.m_local)
        // This entry is already satisfied locally, no need to process it
        continue;

      // Determine whether the current candidate element satisfies the autowiring we are considering.
      // This is done internally via a dynamic cast on the interface type for which this polymorphic
      // base type was constructed.
      if (!value.m_value.try_assign(entry.pCoreObject))
        continue;

      entries.push_back(&value);

      // Success, assign the traits
      value.pObjTraits = &entry;

      // Store if it was injected from the local context or not
      value.m_local = local;
    }

    lk.unlock();

    // Fire off notifications that satisfaction has taken place
    for (auto e: entries)
      e->onSatisfied();
  }

  // Give children a chance to also update their deferred elements:
  lk.lock();
  for (const auto& weak_child : m_children) {
    // Hold reference to prevent this iterator from becoming invalidated:
    auto ctxt = weak_child.lock();
    if(!ctxt)
      continue;

    // Reverse lock before handing off control:
    lk.unlock();
    ctxt->UpdateDeferredElements(
      std::unique_lock<std::mutex>(ctxt->m_stateBlock->m_lock),
      entry,
      false
    );
    lk.lock();
  }
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

void CoreContext::FilterFiringException(const JunctionBoxBase* pProxy, CoreObject* pRecipient) {
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

void CoreContext::AddSnooper(const CoreObjectDescriptor& traits) {
  // Add to collections of snoopers
  InsertSnooper(traits.value);

  // Add PacketSubscriber;
  if (!traits.subscriber.empty())
    AddPacketSubscriber(traits.subscriber);
}

void CoreContext::RemoveSnooper(const CoreObjectDescriptor& traits) {
  RemoveSnooper(traits.value);

  // Cleanup if its a packet listener
  if (!traits.subscriber.empty())
    UnsnoopAutoPacket(traits);
}

void CoreContext::InsertSnooper(const AnySharedPointer& snooper) {
  (std::lock_guard<std::mutex>)m_stateBlock->m_lock,
  m_snoopers.insert(snooper);
}

void CoreContext::RemoveSnooper(const AnySharedPointer& snooper) {
  (std::lock_guard<std::mutex>)m_stateBlock->m_lock,
  m_snoopers.erase(snooper);
}

void CoreContext::AddPacketSubscriber(const AutoFilterDescriptor& rhs) {
  Inject<AutoPacketFactory>()->AddSubscriber(rhs);
}

void CoreContext::AddConfigWatcher(const std::shared_ptr<autowiring::ConfigWatcherBase>& rhs) {
  rhs->SetSelf(rhs);
  Config.When(rhs);
}

void CoreContext::TryTransitionChildrenState(void) {
  std::unique_lock<std::mutex> lk(m_stateBlock->m_lock);

  // We have to hold this to prevent dtors from running in a synchronized context
  std::shared_ptr<CoreContext> prior;
  for (auto childWeak : m_children) {
    // Obtain child pointer and lock it down so our iterator stays stable
    auto child = childWeak.lock();
    if (!child)
      // Expiration while we were attempting to dereference, circle around
      continue;

    // Cannot hold a lock safely if we hand off control to a child context
    lk.unlock();
    {
      // Get lock of child while we're modifying it's state
      std::unique_lock<std::mutex> childLk(child->m_stateBlock->m_lock);

      switch (child->m_state) {
        case State::Initiated:
          // Can transition to the running state now
          {
            auto q = child->m_threads.begin();
            child->m_state = State::Running;

            // Raise the run condition in the child
            childLk.unlock();

            // Child had it's state changed
            child->m_stateBlock->m_stateChanged.notify_all();

            auto outstanding = child->m_stateBlock->IncrementOutstandingThreadCount(child);
            while (q != child->m_threads.end()) {
              (*q)->Start(outstanding);
              q++;
            }
            child->onRunning();
          }

          // Recursivly try to transition children
          child->TryTransitionChildrenState();

          break;
        case State::NotStarted:
          // Children can run now that their parent has been initiated
          child->m_state = State::CanRun;

          // Child had it's state changed
          childLk.unlock();
          child->m_stateBlock->m_stateChanged.notify_all();
          break;
        case State::CanRun:
        case State::Running:
        case State::Shutdown:
        case State::Abandoned:
          // No action need be taken for these states
          break;
      }
    }

    // Permit a prior context to expire if needed
    prior.reset();

    // Need to preserve current child context pointer before it goes out of scope in order to
    // preserve our iterator.
    lk.lock();
    prior = child;
  }

  // Release our lock before letting `prior` expire, we can't hold a lock through such an event
  lk.unlock();
}

void CoreContext::UnsnoopAutoPacket(const CoreObjectDescriptor& traits) {
  {
    std::lock_guard<std::mutex> lk(m_stateBlock->m_lock);

    // If the passed value is currently a snooper, then the caller has snooped a context and also
    // one of its parents.  End here.
    if (m_snoopers.count(traits.value))
      return;
  }

  // Always remove from this context's PacketFactory:
  Inject<AutoPacketFactory>()->RemoveSubscriber(traits.subscriber);
}

std::ostream& operator<<(std::ostream& os, const CoreContext& rhs) {
  rhs.Dump(os);
  return os;
}

std::shared_ptr<CoreContext> CoreContext::SetCurrent(const std::shared_ptr<CoreContext>& ctxt) {
  const auto& currentContext = CurrentContextOrNull();

  // Short-circuit test, no need to proceed if we aren't changing the context:
  if (currentContext == ctxt)
    return currentContext;

  // Value is changing, update:
  auto retVal = currentContext;
  if (ctxt)
    autoCurrentContext.reset(new std::shared_ptr<CoreContext>(ctxt));
  else
    autoCurrentContext.reset();
  return retVal;
}

std::shared_ptr<CoreContext> CoreContext::SetCurrent(std::shared_ptr<CoreContext>&& ctxt) {
  const auto& currentContext = CurrentContextOrNull();

  // Short-circuit test, no need to proceed if we aren't changing the context:
  if (currentContext == ctxt)
    return currentContext;

  // Value is changing, update:
  auto retVal = currentContext;
  if (ctxt)
    autoCurrentContext.reset(new std::shared_ptr<CoreContext>(std::move(ctxt)));
  else
    autoCurrentContext.reset();
  return retVal;
}

std::shared_ptr<CoreContext> CoreContext::SetCurrent(void) {
  return CoreContext::SetCurrent(shared_from_this());
}

void CoreContext::EvictCurrent(void) {
  autoCurrentContext.reset();
}
