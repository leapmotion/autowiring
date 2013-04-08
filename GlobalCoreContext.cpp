#include "stdafx.h"
#include "GlobalCoreContext.h"

// We use a shared pointer, we never want the global context to go away once
// we've created it.  We also have an initializer lock to prevent multi-init
boost::mutex GlobalCoreContext::s_initLock;
cpp11::shared_ptr<GlobalCoreContext> GlobalCoreContext::s_globalContext;

// Initially, there are no instantiators to be run
InstantiatorLink* GlobalCoreContext::s_instantiator = nullptr;

GlobalCoreContext::GlobalCoreContext(void):
  CoreContext(cpp11::shared_ptr<CoreContext>())
{
  // Guard against multi-initialization:
  ASSERT(!s_globalContext);

  // Set up the global shared pointer:
  s_globalContext.reset(this);
  CoreContext::m_self = s_globalContext;

  // Make ourselves the current context before filling it:
  SetCurrent();

  // Run all instantiators:
  for(InstantiatorLink* pCur = s_instantiator; pCur; pCur = pCur->pFlink)
    pCur->fn();
}

GlobalCoreContext::~GlobalCoreContext(void) {
}

// Obtains the currently generated global context:
cpp11::shared_ptr<GlobalCoreContext> GlobalCoreContext::Get() {
  if(s_globalContext)
    return s_globalContext;
  
  boost::lock_guard<boost::mutex> lk(s_initLock);
  if(s_globalContext)
    // Multi-init by another thread, just short-circuit here
    return s_globalContext;

  // Create a new core context.  Constructor will automatically set the shared pointer.
  GlobalCoreContext* pPtr = new GlobalCoreContext();
  ASSERT(s_globalContext);
  ASSERT(s_globalContext.get() == pPtr);

  // We return a copy of s_globalContext to prevent the principal from being modified
  // due to rvalue optimization.
  cpp11::shared_ptr<GlobalCoreContext> ptr(s_globalContext);
  return ptr;
}

cpp11::shared_ptr<GlobalCoreContext> GetGlobalContext(void) {
  return GlobalCoreContext::Get();
}