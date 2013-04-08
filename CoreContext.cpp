#include "stdafx.h"
#include "CoreContext.h"
#include "Autowired.h"
#include "CoreThread.h"
#include "GlobalCoreContext.h"
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
  m_outstanding = cpp11::shared_ptr<CoreContext>(this, NullOp<CoreContext*>);
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
  
  class Lambda {
  public:
    Lambda(const cpp11::shared_ptr<CoreThread>& pCur, const cpp11::shared_ptr<CoreContext>& outstanding, const cpp11::shared_ptr<CoreContext>& self):
      pCur(pCur),
      outstanding(outstanding),
      self(self)
    {}

    cpp11::shared_ptr<CoreThread> pCur;
    cpp11::shared_ptr<CoreContext> outstanding;
    cpp11::shared_ptr<CoreContext> self;

    void operator()() {
      ASSERT(pCur);
      pCur->DelayUntilReady();
      pCur->Run();
    }
  };

  // Hold another lock to prevent m_threads from being modified while we sit on it
  lock_guard<mutex> lk(m_coreLock);
  for(t_threadList::iterator q = m_threads.begin(); q != m_threads.end(); q++) {
    // The following magic is pretty complicated.  It's a lambda, and it has a copy
    // capture on the "outstanding" pointer.  This means that we will hold a reference
    // for as long as there are active threads running.  As soon as the last active
    // thread disappears, the context starts the destruction sequence, _unless_
    // there's someone out there who has an outstanding reference to this context.
    //
    // As a consequence, in order to prevent memory leaks, it's important that members
    // of this context only retain weak_ptr to their enclosing context.  If any
    // members of this context have a cpp11::shared_ptr to this context, the result will be a
    // cyclic reference and a subsequent memory leak.
    boost::thread(Lambda(*q, m_outstanding, self));
  }
}

void CoreContext::SignalShutdown(void) {
  {
    lock_guard<mutex> lk(m_coreLock);
    if(--m_refCount)
      // Someone else still depends on this
      return;
  }

  m_shouldStop = true;
  m_stopping.notify_all();

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

cpp11::shared_ptr<CoreThread> CoreContext::Add(CoreThread* pCoreThread, bool allowNotReady) {
  // We don't allow the insertion of a thread that isn't ready unless the user really
  // wants that behavior.
  ASSERT(allowNotReady || pCoreThread->IsReady());

  // Give the base class a chance first:
  cpp11::shared_ptr<CoreThread> interior = Autowirer::Add(pCoreThread);

  // Insert into the linked list of threads first:
  list<cpp11::shared_ptr<CoreThread>>::iterator q;

  {
    lock_guard<mutex> lk(m_coreLock);
    m_threads.push_front(cpp11::shared_ptr<CoreThread>());
    q = m_threads.begin();

    if(!m_shouldStop)
      // We're already running, this means we're late to the game and need to start _now_.
      boost::thread([this, pCoreThread] () {
        pCoreThread->DelayUntilReady();
        pCoreThread->Run();
      });
  }

  cpp11::weak_ptr<Autowirer> self = m_self;
  
  // This is done so that, when the returned cpp11::shared_ptr is released, we are notified
  // and can release the corresponding iterator.  If we didn't do this, we would have
  // to require the corresponding implementors of pCoreThread to call a Release function
  // manually, and for that to work right they would always have to have a pointer
  *q = cpp11::shared_ptr<CoreThread>(
      pCoreThread,
      [this, q, self] (CoreThread*) {
        // Verify that our context is still around, and lock it in place if so.
        cpp11::shared_ptr<Autowirer> temp = self.lock();
        if(!temp)
          return;

        // Remove the elment we inserted earlier
        lock_guard<mutex> lk(m_coreLock);
        m_threads.erase(q);
      }
    );
  return *q;
}

cpp11::shared_ptr<CoreContext> GetCurrentContext() {
  return CoreContext::CurrentContext();
}
