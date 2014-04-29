#include "stdafx.h"
#include "GlobalCoreContext.h"

GlobalCoreContext::GlobalCoreContext(void):
  CoreContextT<GlobalCoreContext>(std::shared_ptr<CoreContext>())
{
  // Guard against multi-initialization:
  assert(!getGlobalContextSharedPtr());

  // Set up the global shared pointer:
  getGlobalContextSharedPtr().reset(this);

  // Make ourselves the current context before filling it:
  SetCurrent();
}

GlobalCoreContext::~GlobalCoreContext(void) {
}

// Obtains the currently generated global context:
std::shared_ptr<GlobalCoreContext> GlobalCoreContext::Get() {
  if(getGlobalContextSharedPtr())
    return getGlobalContextSharedPtr();

  boost::lock_guard<boost::mutex> lk(getInitLock());
  if(getGlobalContextSharedPtr())
    // Multi-init by another thread, just short-circuit here
    return getGlobalContextSharedPtr();

  // Create a new core context.  Constructor will automatically set the shared pointer.
  GlobalCoreContext* pPtr = new GlobalCoreContext();
  assert(getGlobalContextSharedPtr());
  assert(getGlobalContextSharedPtr().get() == pPtr);

  // Unreferenced, force a ref here to avoid complaints.
  (void)pPtr;

  // We return a copy of getGlobalContextSharedPtr() to prevent the principal from being modified
  // due to rvalue optimization.
  std::shared_ptr<GlobalCoreContext> ptr(getGlobalContextSharedPtr());
  return ptr;
}

std::shared_ptr<GlobalCoreContext> GetGlobalContext(void) {
  return GlobalCoreContext::Get();
}
