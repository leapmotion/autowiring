// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CoreContext.h"
#include "Autowired.h"
#include "BoltBase.h"
#include "CoreThread.h"
#include "GlobalCoreContext.h"
#include "OutstandingCountTracker.h"
#include "ThreadStatusMaintainer.h"
#include <algorithm>
#include <memory>

using namespace boost;

thread_specific_ptr<std::shared_ptr<CoreContext> > CoreContext::s_curContext;

CoreContext::CoreContext(std::shared_ptr<CoreContext> pParent):
  Autowirer(std::static_pointer_cast<Autowirer, CoreContext>(pParent)),
  m_shouldStop(true),
  m_refCount(0)
{
  ASSERT(pParent.get() != this);
}

CoreContext::~CoreContext(void) {
  // The s_curContext pointer holds a shared_ptr to this--if we're in a dtor, and our caller
  // still holds a reference to us, then we have a serious problem.
  ASSERT(
    !s_curContext.get() ||
    !s_curContext.get()->use_count() ||
    s_curContext.get()->get() != this
  );
}

void CoreContext::BroadcastContextCreationNotice(const char* contextName, const std::shared_ptr<CoreContext>& context) const {
  auto nameIter = m_nameListeners.find(contextName);
  if(nameIter != m_nameListeners.end()) {
    // Context creation notice requires that the created context be set before invocation:
    CurrentContextPusher pshr(context);

    // Iterate through all listeners:
    const std::list<BoltBase*>& list = nameIter->second;
    for(auto q = list.begin(); q != list.end(); q++)
      (**q).ContextCreated();
  }

  // Pass the broadcast to all listening children:
  for(auto q = m_children.begin(); q != m_children.end(); q++) {
    std::shared_ptr<CoreContext> child = q->lock();
    if(child)
      child->BroadcastContextCreationNotice(contextName, context);
  }
}

std::shared_ptr<OutstandingCountTracker> CoreContext::IncrementOutstandingThreadCount(void) {
  std::shared_ptr<OutstandingCountTracker> retVal = m_outstanding.lock();
  if(!m_outstanding.expired())
    return retVal;

  boost::lock_guard<boost::mutex> lk(m_outstandingLock);
  retVal.reset(
    new OutstandingCountTracker(
      std::static_pointer_cast<CoreContext, Autowirer>(m_self.lock())
    )
  );
  m_outstanding = retVal;
  return retVal;
}

std::shared_ptr<CoreContext> CoreContext::Create(void) {
  t_childList::iterator childIterator;
  {
    // Lock the child list while we insert
    lock_guard<mutex> lk(m_childrenLock);
  
    // Reserve a place in the list for the child
    childIterator = m_children.insert(m_children.end(), std::weak_ptr<CoreContext>());
  }
  
  // Create the child context
  CoreContext* pContext =
    new CoreContext(
      std::static_pointer_cast<CoreContext, Autowirer>(m_self.lock())
    );
  
  // Create the shared pointer for the context--do not add the context to itself,
  // this creates a dangerous cyclic reference.
  std::shared_ptr<CoreContext> retVal(
    pContext,
    [this, childIterator] (CoreContext* pContext) {
      lock_guard<mutex> lk(m_childrenLock);
      this->m_children.erase(childIterator);
      delete pContext;
    }
  );
  pContext->m_self = retVal;
  *childIterator = retVal;
  return retVal;
}

void CoreContext::InitiateCoreThreads(void) {
  // Self-reference to ensure the context is not destroyed until all threads are gone
  std::shared_ptr<CoreContext> self = std::static_pointer_cast<CoreContext, Autowirer>(m_self.lock());

  // Because the caller was able to invoke a method on this CoreContext, it must have
  // a shared_ptr to it.  Thus, we can assert that the above lock operation succeeded.
  ASSERT(self);

  {
    lock_guard<mutex> lk(m_coreLock);
    if(m_refCount++)
      // Already running
      return;
  }

  if(m_pParent)
    // Start parent threads first
    // Parent MUST be a core context
    std::static_pointer_cast<CoreContext, Autowirer>(m_pParent)->InitiateCoreThreads();

  // Set our stop flag before kicking off any threads:
  m_shouldStop = false;

  // Hold another lock to prevent m_threads from being modified while we sit on it
  lock_guard<mutex> lk(m_coreLock);
  for(t_threadList::iterator q = m_threads.begin(); q != m_threads.end(); ++q)
    (*q)->Start();
}

void CoreContext::SignalShutdown(void) {
  lock_guard<mutex> lk(m_coreLock);
  if(m_refCount == 0 || --m_refCount)
    // Someone else still depends on this
    return;

  // Global context is now "stop":
  m_shouldStop = true;
  m_stopping.notify_all();
    
  // Also pass notice to all child threads:
  for(t_threadList::iterator q = m_threads.begin(); q != m_threads.end(); ++q)
    (*q)->Stop();

  // Pass notice to the parent.  This is required because we use reference counts to decide
  // when to shut down, and it makes no sense to keep a parent context running when we were
  // the ones who got it going in the first place.
  if(m_pParent)
    std::static_pointer_cast<CoreContext, Autowirer>(m_pParent)->SignalShutdown();
}

void CoreContext::SignalTerminate(void) {
  // We're stopping now.
  m_shouldStop = true;
  
  { // Tear down all the children.
    lock_guard<mutex> lk(m_childrenLock);
    for (t_childList::iterator it = m_children.begin(); it != m_children.end(); ++it) {
      std::shared_ptr<CoreContext> ptr = it->lock();
      if(ptr)
      {
        ptr->SignalTerminate();
      }
    }
  }
  
  // Shutmyself down.
  SignalShutdown();
  
  // I shouldn't be referenced anywhere now.
  ASSERT(m_refCount == 0);

  // Wait for the treads to finish before returning.
  for (t_threadList::iterator it = m_threads.begin(); it != m_threads.end(); ++it) {
    (*it)->Wait();
  }
}

std::shared_ptr<CoreContext> CoreContext::CurrentContext(void) {
  if(!s_curContext.get())
    return std::static_pointer_cast<CoreContext, GlobalCoreContext>(GetGlobalContext());

  std::shared_ptr<CoreContext>* retVal = s_curContext.get();
  ASSERT(retVal);
  ASSERT(*retVal);
  return *retVal;
}

void CoreContext::AddCoreThread(CoreThread* ptr, bool allowNotReady) {
  // We don't allow the insertion of a thread that isn't ready unless the user really
  // wants that behavior.
  ASSERT(allowNotReady || ptr->IsReady());

  // Insert into the linked list of threads first:
  lock_guard<mutex> lk(m_coreLock);
  m_threads.push_front(ptr);

  if(!m_shouldStop)
    // We're already running, this means we're late to the game and need to start _now_.
    ptr->Start();
}

void CoreContext::AddBolt(BoltBase* pBase) {
  m_nameListeners[pBase->GetContextName()].push_back(pBase);
}

std::shared_ptr<CoreContext> GetCurrentContext() {
  return CoreContext::CurrentContext();
}

void CoreContext::Dump(std::ostream& os) const {
  Autowirer::Dump(os);

  boost::lock_guard<boost::mutex> lk(m_lock);
  for(auto q = m_threads.begin(); q != m_threads.end(); q++) {
    CoreThread* pThread = *q;
    const char* name = pThread->GetName();
    os << "Thread " << pThread << " " << (name ? name : "(no name)") << std::endl;
  }
}

std::ostream& operator<<(std::ostream& os, const CoreContext& context) {
  context.Dump(os);
  return os;
}

void FilterFiringException(const EventManagerBase* pSender, EventReceiver* pRecipient) {
  // Obtain the current context and pass control:
  CoreContext::CurrentContext()->FilterFiringException(std::current_exception(), pSender, pRecipient);
}