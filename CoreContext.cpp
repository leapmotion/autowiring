#include "stdafx.h"
#include "CoreContext.h"
#include "Autowired.h"
#include "CoreThread.h"
#include "GlobalCoreContext.h"
#include "ThreadStatusMaintainer.h"
#include <boost/thread.hpp>
#include <algorithm>
#include <memory>

using namespace boost;

thread_specific_ptr<cpp11::shared_ptr<CoreContext>> CoreContext::s_curContext;

CoreContext::CoreContext(cpp11::shared_ptr<CoreContext> pParent):
  Autowirer(cpp11::static_pointer_cast<Autowirer, CoreContext>(pParent)),
  m_shouldStop(true),
  m_refCount(0)
{
  ASSERT(pParent.get() != this);
}

CoreContext::~CoreContext(void) {
  // The s_curContext pointer holds a shared_ptr to this--if we're in a ctor, and our caller
  // still holds a reference to us, then we have a serious problem.
  ASSERT(
    !s_curContext.get() ||
    !s_curContext.get()->use_count() ||
    s_curContext.get()->get() != this
  );
}

cpp11::shared_ptr<CoreContext> CoreContext::IncrementOutstandingThreadCount(void) {
  cpp11::shared_ptr<CoreContext> retVal = m_outstanding.lock();
  if(!m_outstanding.expired())
    return retVal;

  boost::lock_guard<boost::mutex> lk(m_outstandingLock);
  retVal = cpp11::shared_ptr<CoreContext>(
    this,
    [this] (CoreContext*) {
      boost::lock_guard<boost::mutex> lk(this->m_coreLock);
      this->m_stop.notify_all();
    }
  );
  m_outstanding = retVal;
  return retVal;
}

cpp11::shared_ptr<CoreContext> CoreContext::NewContext(const cpp11::shared_ptr<CoreContext>& pParent) {
  // Create the context, first
  CoreContext* pContext(new CoreContext(pParent));

  // Create the shared pointer for the context--do not add the context to itself,
  // this creates a dangerous cyclic reference.
  cpp11::shared_ptr<CoreContext> retVal(pContext);
  pContext->m_self = retVal;
  return retVal;
}

void CoreContext::InitiateCoreThreads(void) {
  // Self-reference to ensure the context is not destroyed until all threads are gone
  cpp11::shared_ptr<CoreContext> self = cpp11::static_pointer_cast<CoreContext, Autowirer>(m_self.lock());

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
    cpp11::static_pointer_cast<CoreContext, Autowirer>(m_pParent)->InitiateCoreThreads();

  // Set our stop flag before kicking off any threads:
  m_shouldStop = false;

  // Hold another lock to prevent m_threads from being modified while we sit on it
  lock_guard<mutex> lk(m_coreLock);
  for(t_threadList::iterator q = m_threads.begin(); q != m_threads.end(); q++)
    (*q)->Start();
}

void CoreContext::SignalShutdown(void) {
  {
    lock_guard<mutex> lk(m_coreLock);
    if(--m_refCount)
      // Someone else still depends on this
      return;

    // Global context is now "stop":
    {
      boost::lock_guard<boost::mutex> lk(this->m_coreLock);
      m_shouldStop = true;
      m_stopping.notify_all();
    }
    
    // Also pass notice to all children:
    std::for_each(
      m_threads.begin(),
      m_threads.end(),
      [] (CoreThread* pThread) {
        pThread->Stop();
      }
    );
  }

  if(m_pParent)
    // Signal parent that it's time to shut down
    cpp11::static_pointer_cast<CoreContext, Autowirer>(m_pParent)->SignalShutdown();
}

cpp11::shared_ptr<CoreContext> CoreContext::CurrentContext(void) {
  if(!s_curContext.get())
    return cpp11::static_pointer_cast<CoreContext, GlobalCoreContext>(GetGlobalContext());

  cpp11::shared_ptr<CoreContext>* retVal = s_curContext.get();
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
    boost::thread([this, ptr] () {
      ptr->DelayUntilReady();
      ptr->Run();
    });
}

cpp11::shared_ptr<CoreContext> GetCurrentContext() {
  return CoreContext::CurrentContext();
}
