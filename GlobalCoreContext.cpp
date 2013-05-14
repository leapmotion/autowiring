#include "stdafx.h"
#include "GlobalCoreContext.h"

// We use a shared pointer, we never want the global context to go away once
// we've created it.  We also have an initializer lock to prevent multi-init
std::shared_ptr<GlobalCoreContext> GlobalCoreContext::s_globalContext;

GlobalCoreContext::GlobalCoreContext(void):
  CoreContext(std::shared_ptr<CoreContext>())
{
  // Guard against multi-initialization:
  ASSERT(!s_globalContext);

  // Set up the global shared pointer:
  s_globalContext.reset(this);
  CoreContext::m_self = s_globalContext;

  // Make ourselves the current context before filling it:
  SetCurrent();
}

GlobalCoreContext::~GlobalCoreContext(void) {
}

// Obtains the currently generated global context:
std::shared_ptr<GlobalCoreContext> GlobalCoreContext::Get() {
  if(s_globalContext)
    return s_globalContext;
  
  boost::lock_guard<boost::mutex> lk(getInitLock());
  if(s_globalContext)
    // Multi-init by another thread, just short-circuit here
    return s_globalContext;

  // Create a new core context.  Constructor will automatically set the shared pointer.
  GlobalCoreContext* pPtr = new GlobalCoreContext();
  ASSERT(s_globalContext);
  ASSERT(s_globalContext.get() == pPtr);

  // Unreferenced, force a ref here to avoid complaints.
  (void)pPtr;

  // We return a copy of s_globalContext to prevent the principal from being modified
  // due to rvalue optimization.
  std::shared_ptr<GlobalCoreContext> ptr(s_globalContext);
  return ptr;
}

std::shared_ptr<GlobalCoreContext> GetGlobalContext(void) {
  return GlobalCoreContext::Get();
}
